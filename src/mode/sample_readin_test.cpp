#include <cstdint>
#include <map>
#include <vector>

#include "visual_info.hpp"
#include "util.hpp"

#include "mode/modes.hpp"

void mode_sample_readin_test(const char* in_path)
{
    visual_info vinfo;
    vinfo.read_from_file(in_path);
    printf("start time: %lu\n", vinfo.start_time);
    printf("end time: %lu\n", vinfo.stop_time);
    printf("tracking %lu words\n", vinfo.ecs.size());
    uint64_t ec_count = 0;
    for (std::map<uint64_t, std::vector<ec_info_rw>>::iterator ec_it = vinfo.ecs.begin(); ec_it != vinfo.ecs.end(); ec_it++) {
        std::vector<ec_info_rw>& ec_vec = ec_it->second;
        ec_count += ec_vec.size();
        printf("\t[%lu]: %lu ECs\n", ec_it->first, ec_vec.size());
        for (size_t i = 0; i < ec_vec.size(); i++) {
            printf(
                "\t\tT2:%lu I2:%lu I2A:%lu %s\n",
                ec_vec[i].time2,
                ec_vec[i].instr2,
                ec_vec[i].instr2_abs,
                ec_vec[i].write_no_read ? "W" : "R"
            );
        }
    }
    printf("\n");
    printf("sample count: %lu\n", vinfo.samples.size());
    uint64_t acc_samples = 0;
    for (size_t i = 0; i < vinfo.samples.size(); i++) {
        acc_samples += vinfo.samples[i].hitc;
        printf(
            "\t[%lu]: ADDR:%lu IDX:%lu HITC:%lu COMB:%lu\n",
            i,
            vinfo.samples[i].addr,
            vinfo.samples[i].idx,
            vinfo.samples[i].hitc,
            vinfo.samples[i].combination
        );
    }
    printf("\n");
    printf("stats:\n");
    printf("total cycles: %lu\n", vinfo.stop_time - vinfo.start_time);
    printf("memory words: %lu\n", vinfo.ecs.size());
    printf("ec count: %lu\n", ec_count);
    printf("sample count (unique): %lu\n", vinfo.samples.size());
    printf("sample count (accumulated): %lu\n", acc_samples);
}
