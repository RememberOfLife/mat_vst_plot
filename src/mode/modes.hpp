#pragma once

#include <cstdint>

void mode_sample_writeout_test(const char* out_path);

void mode_sample_readin_test(const char* in_path);

#ifndef NO_CAIRO
void mode_sample_readin_draw(const char* in_path, const char* out_path);
#endif

void mode_result_writeout_database(const char* out_path);

#ifndef NO_CAIRO
void mode_result_readin_draw(const char* in_path_samples, const char* in_path_results, const char* out_path);
#endif
