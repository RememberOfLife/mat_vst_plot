#include <cstdint>
#include <cstdio>
#include <cstring>

#include "mode/modes.hpp"
#include "util.hpp"

int main(int argc, char** argv)
{
    enum OMODE {
        OMODE_NONE = 0,
        OMODE_SAMPLE_WRITEOUT_TEST,
        OMODE_SAMPLE_READIN_TEST,
        OMODE_SAMPLE_READIN_DRAW,
        OMODE_RESULT_WRITEOUT_DATABASE,
        OMODE_RESULT_READIN_DRAW,
    } mode = OMODE_NONE;

    const char* in_path;
    const char* out_path;
    const char* bench_name;
    const char* variant_name;

    if (argc >= 2) {
        if (strcmp(argv[1], "sample-writeout-test") == 0) {
            mode = OMODE_SAMPLE_WRITEOUT_TEST;
            if (argc >= 3) {
                out_path = argv[2];
            } else {
                errorf("ERROR: missing outpath for sample writeout test\n");
            }
        } else if (strcmp(argv[1], "sample-readin-test") == 0) {
            mode = OMODE_SAMPLE_READIN_TEST;
            if (argc >= 3) {
                in_path = argv[2];
            } else {
                errorf("ERROR: missing inpath for sample readin test\n");
            }
        } else if (strcmp(argv[1], "sample-readin-draw") == 0) {
            mode = OMODE_SAMPLE_READIN_DRAW;
            if (argc >= 4) {
                in_path = argv[2];
                out_path = argv[3];
            } else {
                errorf("ERROR: missing path(s) for sample readin draw\n");
            }
        } else if (strcmp(argv[1], "result-writeout-database") == 0) {
            mode = OMODE_RESULT_WRITEOUT_DATABASE;
            if (argc >= 6) {
                in_path = argv[2];
                out_path = argv[3];
                bench_name = argv[4];
                variant_name = argv[5];
            } else {
                errorf("ERROR: missing path(s) for result writeout database\n");
            }
        } else if (strcmp(argv[1], "result-readin-draw") == 0) {
            mode = OMODE_RESULT_READIN_DRAW;
            if (argc >= 4) {
                in_path = argv[2];
                out_path = argv[3];
            } else {
                errorf("ERROR: missing path(s) for result readin draw\n");
            }
        }
    }
    if (mode == OMODE_NONE) {
        printf("usage:\n");
        printf("\tvst_plot sample-output-test <outpath>\n");
        printf("\tvst_plot sample-readin-test <inpath>\n");
        printf("\tvst_plot sample-readin-draw <inpath> <outpath>\n");
        printf("\tvst_plot result-writeout-database <inpath> <outpath> <bench-name> <variant-name>\n");
        printf("\tvst_plot result-readin-draw <inpath> <outpath>\n");
        exit(1);
    }

    switch (mode) {
        case OMODE_SAMPLE_WRITEOUT_TEST: {
            mode_sample_writeout_test(out_path);
        } break;
        case OMODE_SAMPLE_READIN_TEST: {
            mode_sample_readin_test(in_path);
        } break;
        case OMODE_SAMPLE_READIN_DRAW: {
            mode_sample_readin_draw(in_path, out_path);
        } break;
        case OMODE_RESULT_WRITEOUT_DATABASE: {
            mode_result_writeout_database(in_path, out_path, bench_name, variant_name);
        } break;
        case OMODE_RESULT_READIN_DRAW: {
            mode_result_readin_draw(in_path, out_path);
        } break;
        default: {
            errorf("ERROR: unknown mode");
        } break;
    }

    return 0;
}
