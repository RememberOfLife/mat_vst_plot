#include <cassert>
#include <cstdint>
#include <cstdio>
#include <map>
#include <vector>

struct visual_info {

    struct ec_info_rw {
        uint64_t time2;
        uint64_t instr2;
        uint64_t instr2_abs;
        bool write_no_read;
    };

    static bool ec_info_rw_cmp(const ec_info_rw& eci, uint64_t time)
    {
        return eci.time2 < time;
    }

    uint64_t start_time;
    uint64_t end_time;

    std::map<uint64_t, std::vector<ec_info_rw>> ecs;

    struct visual_sample {
        uint64_t addr;
        uint64_t idx;
        uint64_t hits;
        uint64_t combination;
    };

    std::vector<visual_sample> samples;

    /*
    if you want to fill this you need these

    void push_ec(uint64_t addr, uint64_t time2, uint64_t instr2, uint64_t instr2_abs, bool write_no_read)
    {
        std::vector<ec_info_rw>& ec_vec = ecs[addr];
        ec_vec.push_back({time2, instr2, instr2_abs, write_no_read});
    }

    void push_sample(ReservoirSampler::Sample s)
    {
        // find sample within ec and just store the relevant extra info and an index into the ecs
        std::map<uint64_t, std::vector<ec_info_rw>>::iterator ec_it = ecs.find(s.addr);
        if (ec_it != ecs.end()) {
            std::vector<ec_info_rw>& ec_vec = ec_it->second;
            std::vector<ec_info_rw>::iterator target = std::lower_bound(ec_vec.begin(), ec_vec.end(), s.time2, ec_info_rw_cmp);
            if (target != ec_vec.end() && target->time2 == s.time2) {
                uint64_t ec_idx = std::distance(ec_vec.begin(), target); // idx of ec in ec_vec in ecs
                samples.push_back({s.addr, ec_idx, s.hitc, s.bit_position_enumeration_idx});
                return;
            }
        }
        errorf("ERROR: unable to find EC for sample\n");
    }
    */

    void serialize_u64(uint8_t* buf, uint64_t v)
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

    uint64_t deserialize_u64(uint8_t* buf)
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

    void write_out_u64(FILE* fstream, uint64_t v)
    {
        uint8_t format_buf[8];
        serialize_u64(format_buf, v);
        fwrite(&format_buf, sizeof(format_buf), 1, fstream);
    }

    uint64_t read_in_u64(FILE* fstream, uint64_t v)
    {
        uint8_t format_buf[8];
        size_t read = fread(&format_buf, sizeof(format_buf), 1, fstream);
        if (read != sizeof(format_buf)) {
            assert(0);
        }
        return deserialize_u64(format_buf);
    }

    bool write_to_file(const char* filepath)
    {
        FILE* fstream = fopen(filepath, "wb");
        if (fstream == NULL) {
            return false;
        }

        write_out_u64(fstream, ecs.size());
        for (std::map<uint64_t, std::vector<ec_info_rw>>::iterator ec_it = ecs.begin(); ec_it != ecs.end(); ec_it++) {
            write_out_u64(fstream, ec_it->first); // addr
            write_out_u64(fstream, ec_it->second.size()); // vec len
            for (size_t i = 0; i < ec_it->second.size(); i++) {
                // per ec info
                write_out_u64(fstream, ec_it->second[i].time2);
                write_out_u64(fstream, ec_it->second[i].instr2);
                write_out_u64(fstream, ec_it->second[i].instr2_abs);
                write_out_u64(fstream, ec_it->second[i].write_no_read ? 1 : 0);
            }
        }
        write_out_u64(fstream, samples.size());
        for (size_t i = 0; i < samples.size(); i++) {
            write_out_u64(fstream, samples[i].addr);
            write_out_u64(fstream, samples[i].idx);
            write_out_u64(fstream, samples[i].hits);
            write_out_u64(fstream, samples[i].combination);
        }

        fclose(fstream);
        return true;
    }

    void read_from_file(const char* filepath)
    {
    }
};

int main(int argc, char** argv)
{
}
