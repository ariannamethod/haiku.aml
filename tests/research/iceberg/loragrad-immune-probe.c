#include "loragrad.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

static float dot(const float *a, const float *b) {
    double s = 0.0;
    for (int i = 0; i < LG_SIG_DIM; ++i) s += (double)a[i] * b[i];
    return (float)s;
}

int main(void) {
    lg_field_t f;
    float origin[LG_SIG_DIM] = {1}, boundary[LG_SIG_DIM] = {0, 1};
    float alpha;
    assert(lg_field_init(&f, 8, 42) == 0);
    assert(lg_field_vote(&f, origin, &alpha) == LG_FREEZE && alpha == 0);
    lg_field_set_origin_unit(&f, origin);
    lg_field_set_boundary_unit(&f, boundary);
    lg_field_calibrate_experts(&f, 42);
    printf("origin=%s boundary=%s\n", lg_verdict_name(lg_field_vote(&f, origin, &alpha)),
           lg_verdict_name(lg_field_vote(&f, boundary, &alpha)));
    assert(lg_field_vote(&f, origin, &alpha) == LG_PASS);
    assert(lg_field_vote(&f, boundary, &alpha) == LG_DARK);

    lg_field_record(&f, LG_SCAR, origin);
    assert(lg_field_vote(&f, origin, &alpha) == LG_SCAR && alpha == 0);
    lg_field_reset_memory(&f);
    assert(lg_field_vote(&f, origin, &alpha) == LG_PASS);
    puts("recorded scar overrides PASS; reset restores PASS");

    for (int i = 0; i < 256; ++i) lg_field_record(&f, LG_SCAR, boundary);
    lg_field_record(&f, LG_SCAR, origin);
    assert(f.scar_count == 256);
    assert(lg_field_vote(&f, origin, &alpha) == LG_PASS);
    printf("scar saturation=%d, post-cap novel scar not recalled\n", f.scar_count);

    float old_w[8 * LG_SIG_DIM], old_b[8];
    memcpy(old_w, f.expert_w, sizeof(old_w));
    memcpy(old_b, f.expert_b, sizeof(old_b));
    for (int i = 0; i < 100; ++i) {
        lg_field_update_experts(&f, origin, 1, 0.01f);
        lg_field_update_experts(&f, boundary, 0, 0.01f);
    }
    assert(!memcmp(old_w, f.expert_w, sizeof(old_w)));
    assert(!memcmp(old_b, f.expert_b, sizeof(old_b)));
    float min = f.expert_credit[0], max = min;
    for (int i = 1; i < 8; ++i) {
        min = fminf(min, f.expert_credit[i]);
        max = fmaxf(max, f.expert_credit[i]);
    }
    printf("after 200 credit updates directions/bias unchanged; credits=[%.6f, %.6f]\n", min, max);

    float x = 0.5f, y = nextafterf(x, INFINITY);
    float sigx[LG_SIG_DIM], sigy[LG_SIG_DIM];
    lg_signature_from_buffer(&x, 1, sigx);
    lg_signature_from_buffer(&y, 1, sigy);
    printf("buffer tiny change %.9g -> %.9g: cosine %.6f\n", x, y, dot(sigx, sigy));
    int discontinuous = 0;
    for (int i = 1; i <= 100; ++i) {
        x = (float)i / 100.0f;
        y = nextafterf(x, INFINITY);
        lg_signature_from_buffer(&x, 1, sigx);
        lg_signature_from_buffer(&y, 1, sigy);
        if (dot(sigx, sigy) < 0.99f) ++discontinuous;
    }
    printf("one-ULP adjacent metric signatures cosine < .99: %d/100\n", discontinuous);
    lg_field_free(&f);
    return 0;
}
