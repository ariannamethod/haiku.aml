/* Test-only scheduler and filesystem fault control for the real CLI process. */
#define _GNU_SOURCE
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <unistd.h>

static atomic_int workers, saves, launches;
struct launch { void *(*run)(void *); void *argument; };

static void *gated_worker(void *opaque) {
    struct launch launch = *(struct launch *)opaque;
    free(opaque);
    const char *marker = getenv("HAIKU_TEST_STARTED");
    const char *gate = getenv("HAIKU_TEST_GATE");
    if (marker && gate && atomic_fetch_add(&workers, 1) == 0) {
        int fd = open(marker, O_WRONLY | O_CREAT | O_TRUNC, 0600);
        if (fd >= 0) {
            ssize_t written = write(fd, "started", 7);
            if (written != 7) _exit(91);
            (void)close(fd);
        }
        fd = open(gate, O_RDONLY);
        if (fd >= 0) {
            char token;
            while (read(fd, &token, 1) < 0 && errno == EINTR) {}
            (void)close(fd);
        }
    }
    return launch.run(launch.argument);
}

int pthread_create(pthread_t *thread, const pthread_attr_t *attributes,
                   void *(*run)(void *), void *argument) {
    int (*original)(pthread_t *, const pthread_attr_t *, void *(*)(void *), void *) =
        dlsym(RTLD_NEXT, "pthread_create");
    if (getenv("HAIKU_TEST_FAIL_LAUNCH") && atomic_fetch_add(&launches, 1) == 0) return EAGAIN;
    struct launch *launch = malloc(sizeof(*launch));
    if (!launch) return ENOMEM;
    launch->run = run; launch->argument = argument;
    int status = original(thread, attributes, gated_worker, launch);
    if (status) free(launch);
    return status;
}

int mkstemp(char *pattern) {
    int (*original)(char *) = dlsym(RTLD_NEXT, "mkstemp");
    const char *failure = getenv("HAIKU_TEST_FAIL_SAVE");
    int number = atomic_fetch_add(&saves, 1) + 1;
    if (failure && number == atoi(failure)) { errno = EACCES; return -1; }
    return original(pattern);
}
