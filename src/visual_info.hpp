#pragma once

#include <cstdint>
#include <cstdio>
#include <map>
#include <vector>

struct ec_info_rw {
    uint64_t time2;
    uint64_t instr2;
    uint64_t instr2_abs;
    bool write_no_read;

    static bool ec_info_rw_cmp(const ec_info_rw& eci, uint64_t time)
    {
        return eci.time2 < time;
    }
};

struct visual_sample {
    uint64_t addr;
    uint64_t idx;
    uint64_t hitc;
    uint64_t combination;
};

class visual_info {

  public:

    uint64_t start_time = 0;
    uint64_t stop_time = 0;

    std::map<uint64_t, std::vector<ec_info_rw>> ecs;

    std::vector<visual_sample> samples;

  private:

    void serialize_u64(uint8_t* buf, uint64_t v);
    uint64_t deserialize_u64(uint8_t* buf);
    void write_out_u64(FILE* fstream, uint64_t v);
    uint64_t read_in_u64(FILE* fstream);

  public:

    bool write_to_file(const char* filepath);
    bool read_from_file(const char* filepath);

    void push_ec(uint64_t addr, uint64_t time2, uint64_t instr2, uint64_t instr2_abs, bool write_no_read);
    void push_sample(uint64_t addr, uint64_t time2, uint64_t hitc, uint64_t combination);
};
