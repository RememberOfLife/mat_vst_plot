#include <cstdint>
#include <map>
#include <vector>

#include "visual_info.hpp"
#include "util.hpp"

#include "mode/modes.hpp"

void mode_sample_writeout_test(const char* out_path)
{
    visual_info vinfo;

    // test data
    vinfo.start_time = 0;
    vinfo.stop_time = 200;
    vinfo.push_ec(1, 10, 0, 0, 1);
    vinfo.push_ec(2, 20, 0, 0, 0);
    vinfo.push_ec(3, 100, 0, 0, 0);
    vinfo.push_ec(1, 120, 0, 0, 1);

    vinfo.push_ec(1, 130, 0, 0, 0);
    vinfo.push_ec(2, 135, 0, 0, 0);
    vinfo.push_ec(1, 140, 0, 0, 0);
    vinfo.push_ec(2, 145, 0, 0, 0);
    vinfo.push_ec(3, 160, 0, 0, 1);
    vinfo.push_ec(1, 170, 0, 0, 0);

    vinfo.write_to_file(out_path);
}
