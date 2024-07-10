#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <map>
#include <vector>

#include "util.hpp"

#include "visual_info.hpp"

const char* pilot_result_str[PRT_COUNT] = {
    [PRT_OK] = "OK_MARKER",
    [PRT_SDC] = "SDC",
    [PRT_TIMEOUT] = "TIMEOUT",
    [PRT_TRAP] = "TRAP",
    [PRT_DETECTED] = "DETECTED_MARKER",
    [PRT_UNCORRECTED] = "UNCORRECTABLE_ECC",
    [PRT_META_ALL] = "ALL",
};

pilot_result_type pilot_result_type_from_str(const char* type_str)
{
    pilot_result_type ret = PRT_META_NOT_FOUND;
    for (int rtidx = 0; rtidx < PRT_COUNT; rtidx++) {
        if (strcmp(type_str, pilot_result_str[rtidx]) == 0) {
            ret = (pilot_result_type)rtidx;
            break;
        }
    }
    return ret;
}

void visual_info::serialize_u64(uint8_t* buf, uint64_t v)
{
    buf[0] = (uint8_t)((v >> 56) & 0xFF);
    buf[1] = (uint8_t)((v >> 48) & 0xFF);
    buf[2] = (uint8_t)((v >> 40) & 0xFF);
    buf[3] = (uint8_t)((v >> 32) & 0xFF);
    buf[4] = (uint8_t)((v >> 24) & 0xFF);
    buf[5] = (uint8_t)((v >> 16) & 0xFF);
    buf[6] = (uint8_t)((v >> 8) & 0xFF);
    buf[7] = (uint8_t)((v >> 0) & 0xFF);
}

uint64_t visual_info::deserialize_u64(uint8_t* buf)
{
    uint64_t r = 0;
    r |= (uint64_t)(buf[0]) << 56;
    r |= (uint64_t)(buf[1]) << 48;
    r |= (uint64_t)(buf[2]) << 40;
    r |= (uint64_t)(buf[3]) << 32;
    r |= (uint64_t)(buf[4]) << 24;
    r |= (uint64_t)(buf[5]) << 16;
    r |= (uint64_t)(buf[6]) << 8;
    r |= (uint64_t)(buf[7]) << 0;
    return r;
}

void visual_info::write_out_u64(FILE* fstream, uint64_t v)
{
    uint8_t format_buf[8];
    serialize_u64(format_buf, v);
    fwrite(&format_buf, sizeof(format_buf), 1, fstream);
}

uint64_t visual_info::read_in_u64(FILE* fstream)
{
    uint8_t format_buf[8];
    size_t read = fread(&format_buf, sizeof(format_buf), 1, fstream);
    if (read != 1) {
        errorf("ERROR: unable to read u64\n");
    }
    return deserialize_u64(format_buf);
}

bool visual_info::write_to_file(const char* filepath)
{
    FILE* fstream = fopen(filepath, "wb");
    if (fstream == NULL) {
        return false;
    }

    write_out_u64(fstream, start_time);
    write_out_u64(fstream, stop_time);
    write_out_u64(fstream, ecs.size());
    for (std::map<uint64_t, std::vector<ec_info_rw>>::iterator ec_it = ecs.begin(); ec_it != ecs.end(); ec_it++) {
        write_out_u64(fstream, ec_it->first); // addr
        std::vector<ec_info_rw>& ec_vec = ec_it->second;
        write_out_u64(fstream, ec_vec.size()); // vec len
        for (size_t i = 0; i < ec_vec.size(); i++) {
            // per ec info
            write_out_u64(fstream, ec_vec[i].time2);
            write_out_u64(fstream, ec_vec[i].instr2);
            write_out_u64(fstream, ec_vec[i].instr2_abs);
            write_out_u64(fstream, ec_vec[i].write_no_read ? 1 : 0);
        }
    }
    write_out_u64(fstream, samples.size());
    for (size_t i = 0; i < samples.size(); i++) {
        write_out_u64(fstream, samples[i].addr);
        write_out_u64(fstream, samples[i].idx);
        write_out_u64(fstream, samples[i].hitc);
        write_out_u64(fstream, samples[i].combination);
    }

    fclose(fstream);
    return true;
}

bool visual_info::read_from_file(const char* filepath)
{
    FILE* fstream = fopen(filepath, "rb");
    if (fstream == NULL) {
        return false;
    }

    start_time = read_in_u64(fstream);
    stop_time = read_in_u64(fstream);
    uint64_t map_size = read_in_u64(fstream);
    for (uint64_t i = 0; i < map_size; i++) {
        uint64_t addr = read_in_u64(fstream);
        std::vector<ec_info_rw>& ec_vec = ecs[addr];
        uint64_t vec_len = read_in_u64(fstream);
        ec_vec.reserve(vec_len);
        for (uint64_t j = 0; j < vec_len; j++) {
            // per ec info
            uint64_t time2 = read_in_u64(fstream);
            uint64_t instr2 = read_in_u64(fstream);
            uint64_t instr2_abs = read_in_u64(fstream);
            bool write_no_read = read_in_u64(fstream) == 0 ? false : true;
            ec_vec.push_back({time2, instr2, instr2_abs, write_no_read});
        }
    }
    uint64_t sample_count = read_in_u64(fstream);
    samples.reserve(sample_count);
    for (uint64_t i = 0; i < sample_count; i++) {
        uint64_t addr = read_in_u64(fstream);
        uint64_t idx = read_in_u64(fstream);
        uint64_t hitc = read_in_u64(fstream);
        uint64_t combination = read_in_u64(fstream);
        samples.push_back({addr, idx, hitc, combination});
    }

    fclose(fstream);
    return true;
}

void visual_info::push_ec(uint64_t addr, uint64_t time2, uint64_t instr2, uint64_t instr2_abs, bool write_no_read)
{
    std::vector<ec_info_rw>& ec_vec = ecs[addr];
    ec_vec.push_back({time2, instr2, instr2_abs, write_no_read});
}

void visual_info::push_sample(uint64_t addr, uint64_t time2, uint64_t hitc, uint64_t combination)
{
    // find sample within ec and just store the relevant extra info and an index into the ecs
    std::map<uint64_t, std::vector<ec_info_rw>>::iterator ec_it = ecs.find(addr);
    if (ec_it != ecs.end()) {
        std::vector<ec_info_rw>& ec_vec = ec_it->second;
        std::vector<ec_info_rw>::iterator target = std::lower_bound(ec_vec.begin(), ec_vec.end(), time2, ec_info_rw::ec_info_rw_cmp);
        if (target != ec_vec.end() && target->time2 == time2) {
            uint64_t ec_idx = std::distance(ec_vec.begin(), target); // idx of ec in ec_vec in ecs
            samples.push_back({addr, ec_idx, hitc, combination});
            return;
        }
    }
    errorf("ERROR: unable to find EC for sample\n");
}
