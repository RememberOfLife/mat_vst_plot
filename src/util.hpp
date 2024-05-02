#pragma once

#include <cstdio>
#include <cstdint>
#include <cstdlib>

void warnf(const char* fmt, ...);
void errorf(const char* fmt, ...);
typedef void error_cleanup(void);
void error_set_cleanup(error_cleanup* cb_cleanup);

float general_f32_zto(uint32_t rn);
double general_f64_zto(uint64_t rn);

typedef struct fast_prng_t {
    uint64_t state;
    uint64_t inc;
} fast_prng;

void fprng_srand(fast_prng* fprng, uint64_t seed);
uint32_t fprng_rand(fast_prng* fprng);
uint64_t fprng_rand64(fast_prng* fprng);
uint64_t fprng_uintn(fast_prng* fprng, uint64_t max_n);
