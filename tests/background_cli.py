#!/usr/bin/env python3
"""Exercise actual CLI publication, idle workers and crash recovery as processes."""
import argparse
import os
from pathlib import Path
import re
import selectors
import shlex
import subprocess
import tempfile
import time

ROOT = Path(__file__).resolve().parents[1]
TIMEOUT = float(os.environ.get("HAIKU_BACKGROUND_TIMEOUT", "180"))


def replies(output):
    return re.findall(rb"\nHAiKU:\n(.*?)\n\n", output, re.S)


def require(condition, detail):
    if not condition:
        raise AssertionError(detail)


class Chat:
    def __init__(self, command, directory, environment):
        self.directory = directory
        self.process = subprocess.Popen(command, cwd=directory, env=environment,
                                        stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                                        stderr=subprocess.PIPE)
        self.selector = selectors.DefaultSelector()
        self.output = bytearray()
        self.errors = bytearray()
        self.selector.register(self.process.stdout, selectors.EVENT_READ, self.output)
        self.selector.register(self.process.stderr, selectors.EVENT_READ, self.errors)

    def pump(self, timeout=0.05):
        for key, _ in self.selector.select(timeout):
            data = os.read(key.fileobj.fileno(), 65536)
            if data:
                key.data.extend(data)
            else:
                self.selector.unregister(key.fileobj)

    def wait_for(self, predicate, detail):
        deadline = time.monotonic() + TIMEOUT
        while not predicate():
            self.pump()
            if self.process.poll() is not None:
                self.pump(0)
                require(predicate(), detail + "\n" + self.output.decode(errors="replace")
                        + self.errors.decode(errors="replace"))
            require(time.monotonic() < deadline, "Timed out: " + detail)

    def send(self, text):
        self.process.stdin.write(text.encode())
        self.process.stdin.flush()

    def eof(self):
        self.process.stdin.close()

    def finish(self, success=True):
        self.wait_for(lambda: self.process.poll() is not None, "CLI exit")
        while self.selector.get_map():
            self.pump()
        (self.directory / "stdout.txt").write_bytes(self.output)
        (self.directory / "stderr.txt").write_bytes(self.errors)
        self.selector.close()
        for stream in (self.process.stdin, self.process.stdout, self.process.stderr):
            if not stream.closed:
                stream.close()
        if success:
            require(self.process.returncode == 0, self.errors.decode(errors="replace"))
        else:
            require(self.process.returncode != 0, "Faulted worker exited successfully")
        return bytes(self.output)

    def kill(self):
        self.process.kill()
        self.finish(False)


class Suite:
    def __init__(self, work, mode, profile, shim):
        self.work, self.mode, self.profile, self.shim = work, mode, profile, shim
        self.runner = Path(os.environ.get("HAIKU_AML", ROOT.parent / "ariannamethod.ai/runner/aml-notorch")).resolve()
        self.compiler = Path(os.environ.get("HAIKU_AMLC", ROOT.parent / "ariannamethod.ai/tools/amlc")).resolve()
        self.source = (ROOT / (profile + ".aml")).read_text()
        self.active = []
        self.reference_replies = {}
        self.final_checkpoints = {}

    def create(self, name, serial=False, broken=False):
        directory = self.work / self.mode / self.profile / name
        directory.mkdir(parents=True)
        for item in ("src", "models"):
            (directory / item).symlink_to(ROOT / item, target_is_directory=True)
        source = self.source.replace("utc_day()", "20746")
        # Absence of weight symlinks exercises the real missing-body path.
        if serial:
            source, count = re.subn(r"^background_life = 1$", "background_life = 0", source, flags=re.M)
            require(count == 1, "CLI must expose background_life for its serial control")
        if broken:
            source, count = re.subn(r"^( +)SPAWN ([^\n]+):$",
                                   r'\1SPAWN \2:\n\1    assert(0, "Injected background worker failure")',
                                   source, count=1, flags=re.M)
            require(count == 1, "CLI worker injection point not found")
        script = directory / (self.profile + ".aml")
        script.write_text(source)
        command = [str(self.runner), str(script)]
        if self.mode == "compiled":
            binary = directory / "chat"
            result = subprocess.run([str(self.compiler), str(script), "--scalar", "-o", str(binary)],
                                    env=os.environ, capture_output=True, timeout=TIMEOUT)
            (directory / "compile.out").write_bytes(result.stdout + result.stderr)
            require(result.returncode == 0, result.stderr.decode(errors="replace"))
            command = [str(binary)]
        return directory, command

    def launch(self, case, gate=False, fail_save=None, fail_launch=False):
        directory, command = case
        environment = os.environ.copy()
        environment["LD_PRELOAD"] = str(self.shim)
        for key in ("HAIKU_TEST_GATE", "HAIKU_TEST_STARTED", "HAIKU_TEST_FAIL_SAVE", "HAIKU_TEST_FAIL_LAUNCH"):
            environment.pop(key, None)
        if gate:
            os.mkfifo(directory / "worker.fifo")
            environment["HAIKU_TEST_GATE"] = str(directory / "worker.fifo")
            environment["HAIKU_TEST_STARTED"] = str(directory / "worker.started")
        if fail_save:
            environment["HAIKU_TEST_FAIL_SAVE"] = str(fail_save)
        if fail_launch:
            environment["HAIKU_TEST_FAIL_LAUNCH"] = "1"
        chat = Chat(command, directory, environment)
        self.active.append(chat)
        return chat

    def state(self, case):
        return case[0] / (self.profile + ".state")

    def early(self, chat, case):
        marker = case[0] / "worker.started"
        chat.wait_for(lambda: marker.exists() and len(replies(chat.output)) > 0,
                      "Spoken response while background worker is blocked")
        pending = self.state(case).read_bytes()
        require(b"background-pending" in pending, "Response lacks durable pending job")
        require(chat.process.poll() is None, "CLI exited with its worker still blocked")
        return pending

    def release(self, case):
        with (case[0] / "worker.fifo").open("wb", buffering=0) as pipe:
            pipe.write(b"go")

    def baseline(self, name, text):
        case = self.create(name, serial=True)
        chat = self.launch(case)
        chat.send(text)
        chat.eof()
        output = chat.finish()
        self.reference_replies[name] = replies(output)
        value = self.state(case).read_bytes()
        self.final_checkpoints[self.state(case)] = value
        return value

    def run(self):
        one = self.baseline("serial-one", "rain\n")
        two = self.baseline("serial-two", "rain\nmoon\n")
        case = self.create("idle")
        chat = self.launch(case, gate=True)
        chat.send("rain\n")
        self.early(chat, case)
        self.release(case)
        chat.wait_for(lambda: self.state(case).read_bytes() == one,
                      "Completed checkpoint while stdin remains idle")
        require(chat.process.poll() is None, "CLI stopped instead of waiting for input")
        chat.send("quit\n")
        require(replies(chat.finish()) == self.reference_replies["serial-one"], "Idle response differs from serial")
        self.final_checkpoints[self.state(case)] = one
        print(self.mode, self.profile, "early reply and idle durable completion", flush=True)

        for ending in ("next", "quit", "eof"):
            case = self.create(ending)
            chat = self.launch(case, gate=True)
            chat.send("rain\n" + {"next": "moon\nquit\n", "quit": "quit\n", "eof": ""}[ending])
            if ending == "eof":
                chat.eof()
            self.early(chat, case)
            require(chat.output.count(b"\nHAiKU:\n") == 1,
                    "Next turn displayed before predecessor worker drained")
            self.release(case)
            output = chat.finish()
            require(self.state(case).read_bytes() == (two if ending == "next" else one),
                    ending + " drain changed complete checkpoint bytes")
            self.final_checkpoints[self.state(case)] = two if ending == "next" else one
            require(replies(output) == self.reference_replies["serial-two" if ending == "next" else "serial-one"],
                    ending + " changed, duplicated or lost a response")
        print(self.mode, self.profile, "next input, quit and EOF drain", flush=True)

        for fault in ("kill", "worker", "save"):
            case = self.create(fault, broken=fault == "worker")
            chat = self.launch(case, gate=True, fail_save=2 if fault == "save" else None)
            chat.send("rain\n")
            pending = self.early(chat, case)
            require(replies(chat.output) == self.reference_replies["serial-one"], fault + " changed early response")
            if fault == "kill":
                chat.kill()
            else:
                self.release(case)
                chat.send("quit\n")
                chat.finish(False)
                wanted = b"Injected background worker failure" if fault == "worker" else b"temporary create failed"
                require(wanted in chat.errors, "Wrong worker failure: " + chat.errors.decode(errors="replace"))
            require(self.state(case).read_bytes() == pending, fault + " damaged durable pending")
            self.final_checkpoints[self.state(case)] = pending
            # Resume through the uninstrumented production entrypoint.
            restored = self.create(fault + "-restart")
            self.state(restored).write_bytes(pending)
            replay = self.launch(restored)
            replay.send("quit\n")
            output = replay.finish()
            require(self.state(restored).read_bytes() == one, fault + " restart changed complete bytes")
            self.final_checkpoints[self.state(restored)] = one
            require(b"\nHAiKU:\n" not in output, fault + " restart repeated the spoken response")
        print(self.mode, self.profile, "kill, worker failure and save failure recover exactly", flush=True)

        case = self.create("pending-save-failure")
        chat = self.launch(case, fail_save=1)
        chat.send("rain\n")
        chat.eof()
        output = chat.finish(False)
        require(not self.state(case).exists() and b"\nHAiKU:\n" not in output,
                "Failed pending save displayed or persisted an accepted turn")
        self.launch_refusal(one)
        self.completed_crash(one)

    def launch_refusal(self, expected):
        case = self.create("launch-failure")
        chat = self.launch(case, fail_launch=True)
        chat.send("rain\n")
        chat.eof()
        output = chat.finish(False)
        require(b"cannot launch worker" in chat.errors, "Missing worker launch diagnostic")
        require(replies(output) == self.reference_replies["serial-one"], "Launch failure changed spoken response")
        pending = self.state(case).read_bytes()
        require(b"background-pending" in pending, "Launch failure lost durable intent")
        self.final_checkpoints[self.state(case)] = pending
        restored = self.create("launch-restart")
        self.state(restored).write_bytes(pending)
        replay = self.launch(restored)
        replay.send("quit\n")
        output = replay.finish()
        require(self.state(restored).read_bytes() == expected, "Launch failure recovery changed complete bytes")
        self.final_checkpoints[self.state(restored)] = expected
        require(not replies(output), "Launch failure recovery repeated the response")
        print(self.mode, self.profile, "worker launch failure preserves and recovers pending", flush=True)

    def completed_crash(self, expected):
        case = self.create("complete-kill")
        chat = self.launch(case)
        chat.send("rain\n")
        state = self.state(case)
        chat.wait_for(lambda: state.exists() and state.read_bytes() == expected,
                      "Durable worker completion before main joins")
        require(replies(chat.output) == self.reference_replies["serial-one"], "Pre-join completion changed response")
        chat.kill()
        require(state.read_bytes() == expected, "Killing after durable completion damaged checkpoint")
        self.final_checkpoints[state] = expected
        restored = self.create("complete-kill-restart")
        self.state(restored).write_bytes(expected)
        replay = self.launch(restored)
        replay.send("quit\n")
        output = replay.finish()
        require(self.state(restored).read_bytes() == expected, "Completed restart repeated a tail")
        self.final_checkpoints[self.state(restored)] = expected
        require(not replies(output), "Completed restart repeated a response")
        print(self.mode, self.profile, "kill after durable completion and before join publishes once", flush=True)

    def retain_checkpoints(self):
        directory = self.work / self.mode / self.profile
        for state in directory.glob("*/" + self.profile + ".state"):
            self.final_checkpoints.setdefault(state, state.read_bytes())

    def unchanged_checkpoints(self):
        for state, expected in self.final_checkpoints.items():
            require(state.read_bytes() == expected, "Later process rewrote a finished checkpoint: " + str(state))

    def cleanup(self):
        for chat in self.active:
            if chat.process.poll() is None:
                chat.kill()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--work", type=Path)
    parser.add_argument("--modes", nargs="+", choices=("interpreted", "compiled"), default=["interpreted", "compiled"])
    parser.add_argument("--profiles", nargs="+", choices=("haiku", "iceberg"), default=["haiku", "iceberg"])
    args = parser.parse_args()
    work = (args.work or Path(tempfile.mkdtemp(prefix="haiku-background-cli-"))).resolve()
    work.mkdir(parents=True, exist_ok=True)
    if "compiled" in args.modes and "AML_PREFIX" not in os.environ:
        prefix = work / "prefix"
        (prefix / "lib").mkdir(parents=True, exist_ok=True)
        libraries = {
            "libaml.a": ("HAIKU_AML_LIB", ROOT.parent / "ariannamethod.ai/libaml.a"),
            "libaml_notorch.a": ("HAIKU_AML_BRIDGE_LIB", ROOT.parent / "ariannamethod.ai/libaml_notorch.a"),
            "libnotorch.a": ("HAIKU_NOTORCH_LIB", ROOT.parent / "notorch/libnotorch.a"),
        }
        for name, (variable, default) in libraries.items():
            (prefix / "lib" / name).symlink_to(Path(os.environ.get(variable, default)).resolve())
        os.environ["AML_PREFIX"] = str(prefix)
    shim = work / "background_gate.so"
    subprocess.run(shlex.split(os.environ.get("CC", "cc")) +
                   ["-std=c11", "-O2", "-Wall", "-Wextra", "-Werror", "-shared", "-fPIC",
                    str(ROOT / "tests/background_gate.c"), "-ldl", "-lpthread", "-o", str(shim)], check=True)
    print("Background CLI evidence:", work, flush=True)
    suites = []
    for mode in args.modes:
        for profile in args.profiles:
            suite = Suite(work, mode, profile, shim)
            suites.append(suite)
            try:
                suite.run()
                suite.retain_checkpoints()
            finally:
                suite.cleanup()
    for suite in suites:
        suite.unchanged_checkpoints()
    print("HAIKU_BACKGROUND_CLI_OK", flush=True)


if __name__ == "__main__":
    main()
