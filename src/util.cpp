#include <cstdarg>
#include <cstdint>

#include "util.hpp"

error_cleanup* set_cleanup = NULL;

void warnf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}

void errorf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    if (set_cleanup != NULL) {
        set_cleanup();
    }
    exit(-1);
}

void error_set_cleanup(error_cleanup* cb_cleanup)
{
    if (set_cleanup != NULL) {
        warnf("re-set cleanup, lost: %p\n", set_cleanup);
    }
    set_cleanup = cb_cleanup;
}

float general_f32_zto(uint32_t rn)
{
    const double ONE_OVER_MAX_UINT = (1.0 / (double)0xFFFFFFFF);
    return (float)(ONE_OVER_MAX_UINT * (double)rn);
}

double general_f64_zto(uint64_t rn)
{
    const double ONE_OVER_MAX_UINT = (1.0 / (double)0xFFFFFFFFFFFFFFFF);
    return (double)(ONE_OVER_MAX_UINT * (double)rn);
}

void fprng_srand(fast_prng* fprng, uint64_t seed)
{
    // splitmix pre-seed
    uint64_t z = (seed + UINT64_C(0x9E3779B97F4A7C15));
    z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
    z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
    fprng->state = z ^ (z >> 31);
    fprng->inc = 1;
}

uint32_t fprng_rand(fast_prng* fprng)
{
    uint64_t oldstate = fprng->state;
    // Advance internal state
    fprng->state = oldstate * 6364136223846793005ULL + (fprng->inc | 1);
    // Calculate output function (XSH RR), uses old state for max ILP
    uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

uint64_t fprng_rand64(fast_prng* fprng)
{
    return ((uint64_t)fprng_rand(fprng) << 32) | fprng_rand(fprng);
}

uint64_t fprng_uintn(fast_prng* fprng, uint64_t max_n)
{
    // https://funloop.org/post/2015-02-27-removing-modulo-bias-redux.html
    uint64_t r;
    uint64_t threshold = -max_n % max_n;
    if (max_n <= UINT32_MAX) {
        do {
            r = fprng_rand(fprng);
        } while (r < threshold);
    } else {
        do {
            r = fprng_rand64(fprng);
        } while (r < threshold);
    }
    return r % max_n;
}
