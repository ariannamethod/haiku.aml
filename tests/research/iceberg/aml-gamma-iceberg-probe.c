#include "ariannamethod.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

static int model_calls, gamma_calls;
static int model_cb(const char *path) {
    assert(!strcmp(path, "missing.pen7"));
    model_calls++;
    return -1;
}
static int gamma_cb(const char *name, const char *path) {
    assert(!strcmp(name, "penelope"));
    assert(!strcmp(path, "missing.pen7"));
    gamma_calls++;
    return -1;
}
static void configure(const char *a, const char *b, float out[4]) {
    char script[256];
    am_init();
    snprintf(script, sizeof(script),
        "GAMMA %s 0.8\nGAMMA %s 0.6\nJANUS %s %s\n"
        "ESSENCE 0.7\nJANUS_BLEND 0.3\n", a, b, a, b);
    assert(am_exec(script) == 0);
    float x[4] = {1, 2, 3, 4};
    am_apply_gamma_to_logits(x, 4);
    memcpy(out, x, sizeof(x));
    assert(fabsf(am_gamma_get_blend() - 0.74f) < 1e-6f);
}
int main(void) {
    float named[4], renamed[4];
    configure("penelope", "nanojanus", named);
    configure("other_a", "other_b", renamed);
    assert(!memcmp(named, renamed, sizeof(named)));
    printf("gamma blend=%.6f, logits=%.6f %.6f %.6f %.6f; renamed identity bytes equal\n",
        am_gamma_get_blend(), named[0], named[1], named[2], named[3]);
    am_init();
    am_janus_register(model_cb, NULL, NULL, gamma_cb, NULL, NULL,
                      NULL, NULL, NULL, NULL);
    int rc = am_exec("LOAD_MODEL missing.pen7\nLOAD_GAMMA penelope missing.pen7\n");
    assert(model_calls == 1 && gamma_calls == 1);
    assert(rc == 0 && am_get_state()->n_gamma == 1);
    printf("callback failures: model=%d gamma=%d, am_exec rc=%d, configured gamma slots=%d\n",
        model_calls, gamma_calls, rc, am_get_state()->n_gamma);
    return 0;
}
