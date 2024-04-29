#include <cstdint>
#include <cstdio>
#include <cstring>

#include "mode/modes.hpp"
#include "util.hpp"

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
            mode_output_test(out_path);
        } break;
        case OMODE_READIN_TEST: {
            mode_readin_test(in_path);
        } break;
        case OMODE_READIN_DRAW: {
            mode_readin_draw(in_path, out_path);
        } break;
        default: {
            errorf("ERROR: unknown mode");
        } break;
    }

    return 0;
}
