#include <cstdint>
#include <map>
#include <vector>

#include "visual_info.hpp"
#include "util.hpp"

#include "mode/modes.hpp"

void mode_output_test(const char* out_path)
{
    visual_info vinfo;
    //TODO some test data
    vinfo.write_to_file(out_path);
}
