#include <cstdint>

#include "visual_info.hpp"
#include "util.hpp"

#include "mode/modes.hpp"

void mode_readin_draw(const char* in_path, const char* out_path)
{
    visual_info vinfo;
    vinfo.read_from_file(in_path);
    //TODO
}
