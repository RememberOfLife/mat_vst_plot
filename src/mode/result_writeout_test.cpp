#include <cstdint>
#include <map>
#include <vector>

#include "visual_info.hpp"
#include "util.hpp"

#include "mode/modes.hpp"

void ec_push(visual_info& vinfo, uint64_t addr, uint64_t time2, bool write_no_read)
{
    vinfo.push_ec(addr, time2, 0, 0, write_no_read);
}

void mode_result_writeout_test(const char* out_path)
{
    visual_info vinfo;

    // test data
    vinfo.start_time = 0;
    vinfo.stop_time = 200;

    const bool EC_WRITE = true;
    const bool EC_READ = false;

    ec_push(vinfo, 1, 10, EC_WRITE);
    ec_push(vinfo, 2, 20, EC_READ);
    ec_push(vinfo, 3, 100, EC_READ);
    ec_push(vinfo, 1, 120, EC_WRITE);

    ec_push(vinfo, 1, 130, EC_READ);
    ec_push(vinfo, 2, 135, EC_READ);
    ec_push(vinfo, 1, 140, EC_READ);
    ec_push(vinfo, 2, 145, EC_READ);
    ec_push(vinfo, 3, 160, EC_WRITE);
    ec_push(vinfo, 1, 170, EC_READ);

    vinfo.push_sample(3, 100, 3, PRT_OK);
    vinfo.push_sample(3, 100, 3, PRT_SDC);

    vinfo.write_to_file(out_path);
}
