#include <cstdint>
#include <cstdio>
#include <cstring>
#include <map>
#include <vector>

#include "util.hpp"
#include "visual_info.hpp"

int main(int argc, char** argv)
{
    enum OMODE {
        OMODE_NONE = 0,
        OMODE_OUTPUT_TEST,
        OMODE_READIN_TEST,
        OMODE_READIN_DRAW,
    } mode = OMODE_NONE;

    const char* in_path;
    const char* out_path;

    if (argc >= 2) {
        if (strcmp(argv[1], "output-test") == 0) {
            mode = OMODE_OUTPUT_TEST;
            if (argc >= 3) {
                out_path = argv[2];
            } else {
                errorf("ERROR: missing outpath for output test\n");
            }
        } else if (strcmp(argv[1], "readin-test") == 0) {
            mode = OMODE_READIN_TEST;
            if (argc >= 3) {
                in_path = argv[2];
            } else {
                errorf("ERROR: missing inpath for readin test\n");
            }
        } else if (strcmp(argv[1], "readin-draw") == 0) {
            mode = OMODE_READIN_DRAW;
            if (argc >= 4) {
                in_path = argv[2];
                out_path = argv[3];
            } else {
                errorf("ERROR: missing path(s) for readin draw\n");
            }
        }
    }
    if (mode == OMODE_NONE) {
        printf("usage:\n");
        printf("\tvst_plot output-test <outpath>\n");
        printf("\tvst_plot readin-test <inpath>\n");
        printf("\tvst_plot readin-draw <inpath> <outpath>\n");
        exit(1);
    }

    switch (mode) {
        case OMODE_OUTPUT_TEST: {
            visual_info vinfo;
            //TODO some test data
            vinfo.write_to_file(out_path);
        } break;
        case OMODE_READIN_TEST: {
            visual_info vinfo;
            vinfo.read_from_file(in_path);
            printf("tracking %lu words\n", vinfo.ecs.size());
            for (std::map<uint64_t, std::vector<ec_info_rw>>::iterator ec_it = vinfo.ecs.begin(); ec_it != vinfo.ecs.end(); ec_it++) {
                std::vector<ec_info_rw>& ec_vec = ec_it->second;
                printf("\t[%lu]: %lu ECs\n", ec_it->first, ec_vec.size());
                for (size_t i = 0; i < ec_vec.size(); i++) {
                    printf(
                        "\t\tT2:%lu I2:%lu I2A:%lu %s\n",
                        ec_vec[i].time2,
                        ec_vec[i].instr2,
                        ec_vec[i].instr2_abs,
                        ec_vec[i].write_no_read ? "WR" : "RD"
                    );
                }
            }
            printf("\n");
            printf("sample count: %lu\n", vinfo.samples.size());
            for (size_t i = 0; i < vinfo.samples.size(); i++) {
                printf(
                    "\t[%lu]: ADDR:%lu IDX:%lu HITC:%lu COMB:%lu\n",
                    i,
                    vinfo.samples[i].addr,
                    vinfo.samples[i].idx,
                    vinfo.samples[i].hitc,
                    vinfo.samples[i].combination
                );
            }
        } break;
        case OMODE_READIN_DRAW: {
            visual_info vinfo;
            vinfo.read_from_file(in_path);
            //TODO
        } break;
        default: {
            errorf("ERROR: unknown mode");
        } break;
    }

    return 0;
}
