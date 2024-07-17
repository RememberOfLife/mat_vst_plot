#pragma once

#include <cstdint>

void mode_sample_writeout_test(const char* out_path);

void mode_sample_readin_test(const char* in_path);

void mode_sample_readin_draw(const char* in_path, const char* out_path);

void mode_result_writeout_test(const char* out_path);

void mode_result_writeout_database(const char* in_path, const char* out_path, const char* bench_name, const char* variant_name);

void mode_result_readin_draw(const char* in_path, const char* out_path, const char* draw_type);
