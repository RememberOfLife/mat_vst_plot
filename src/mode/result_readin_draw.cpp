#include <cmath>
#include <cstdint>
#include <map>
#include <unordered_map>
#include <vector>

#include <cairo/cairo.h>

#include "visual_info.hpp"
#include "util.hpp"

#include "mode/modes.hpp"

void mode_result_readin_draw(const char* in_path, const char* out_path, const char* draw_type)
{
    pilot_result_type tdraw_type = pilot_result_type_from_str(draw_type);
    if (tdraw_type == PRT_META_NOT_FOUND) {
        errorf("draw type not found\n");
    }

    fast_prng rand;
    fprng_srand(&rand, 42);

    visual_info vinfo;
    vinfo.read_from_file(in_path);

    double total_time = (double)(vinfo.stop_time - vinfo.start_time);
    uint64_t word_count = vinfo.ecs.size();

    const int max_dim = 10000;

    const int wPx = std::min(max_dim, (int)total_time * 3);
    const int hPx = std::min(max_dim, (int)word_count * 6);
    const double wPxF = wPx;
    const double hPxF = hPx;
    const double dPxF = hypot(wPxF, hPxF);

    cairo_surface_t* surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, wPx, hPx);
    cairo_t* ctx = cairo_create(surface);

    cairo_set_source_rgb(ctx, 1, 1, 1);
    cairo_paint_with_alpha(ctx, 1);

    cairo_scale(ctx, wPxF, hPxF); //TODO want scale still, or no?

    //#####
    // drawing start

    double word_space_height = (double)1 / (double)word_count;

    double line_width = 1 / hPxF; //0.0001

    // draw colors for discarded no
    uint64_t word_line_i = 0;
    for (std::map<uint64_t, std::vector<ec_info_rw>>::iterator ec_it = vinfo.ecs.begin(); ec_it != vinfo.ecs.end(); ec_it++) {
        std::vector<ec_info_rw>& ec_vec = ec_it->second;
        for (size_t ec_idx = 0; ec_idx < ec_vec.size(); ec_idx++) {
            double adj_time = (double)(ec_vec[ec_idx].time2 - vinfo.start_time);
            if (ec_vec[ec_idx].write_no_read) {
                // grayout if this is a write
                double prev_adj_time = ec_idx == 0 ? 0 : (double)(ec_vec[ec_idx - 1].time2 - vinfo.start_time);
                double gray = 0.3;
                cairo_set_source_rgb(ctx, gray, gray, gray);
                cairo_rectangle(ctx, prev_adj_time / total_time, word_space_height * word_line_i, (adj_time - prev_adj_time) / total_time, word_space_height);
                cairo_fill(ctx);
            } else if (ec_idx == 0) {
                //TODO dont need this actually
                // blue if entry read
                // cairo_set_source_rgb(ctx, 0.38, 0.77, 1);
                // cairo_rectangle(ctx, 0, word_space_height * word_line_i, adj_time / total_time, word_space_height * (word_line_i + 1));
                // cairo_fill(ctx);
            }
            // linemark reads
            double x_pos = adj_time / total_time;
            cairo_set_source_rgb(ctx, 0, 0, 0);
            cairo_move_to(ctx, x_pos, word_space_height * word_line_i);
            cairo_line_to(ctx, x_pos, word_space_height * (word_line_i + 1));
            cairo_set_line_width(ctx, line_width);
            cairo_stroke(ctx);
        }
        // right margin grayout
        double last_adj_time = (double)(ec_vec[ec_vec.size() - 1].time2 - vinfo.start_time);
        // cairo_set_source_rgb(ctx, 1, 0.93, 0.38); // gold
        double gray = 0.3;
        cairo_set_source_rgb(ctx, gray, gray, gray);
        cairo_rectangle(ctx, last_adj_time / total_time, word_space_height * word_line_i, 1, word_space_height);
        cairo_fill(ctx);
        word_line_i++;
    }

    // horiztonal lines separating the addresses
    if (word_count < 2000) {
        for (uint64_t i = 1; i < word_count; i++) {
            cairo_set_source_rgb(ctx, 0, 0, 0);
            cairo_move_to(ctx, 0, word_space_height * i);
            cairo_line_to(ctx, 1, word_space_height * i);
            cairo_set_line_width(ctx, line_width * (word_count < 1000 ? 1 : 0.5));
            cairo_stroke(ctx);
        }
    }

    double type_colors[PRT_COUNT][3] = {
        [PRT_OK] = {0, 1, 0}, // green
        [PRT_SDC] = {1, 0, 0}, // red
        [PRT_TIMEOUT] = {0, 0, 1}, // blue
        [PRT_TRAP] = {0, 1, 1}, // cyan
        [PRT_DETECTED] = {1, 1, 0}, // yellow
        [PRT_UNCORRECTED] = {0.5, 0, 1}, // magenta
        [PRT_META_ALL] = {0, 0, 0}, // black
    };

    const bool use_grouped = true;
    if (use_grouped) {
        // group results by ec..
        std::unordered_map<uint64_t, std::unordered_map<uint64_t, std::vector<visual_sample>>> memory_grouped_results;
        for (uint64_t i = 0; i < vinfo.samples.size(); i++) {
            visual_sample& sample = vinfo.samples[i];
            if (sample.combination == PRT_META_ALL) {
                errorf("invalid sample with type all: idx %lu\n", i);
            }
            memory_grouped_results[sample.addr][sample.idx].push_back(sample);
        }

        for (std::map<uint64_t, std::vector<ec_info_rw>>::iterator ec_it = vinfo.ecs.begin(); ec_it != vinfo.ecs.end(); ec_it++) {
            std::vector<ec_info_rw>& ec_vec = ec_it->second;
            for (size_t ec_idx = 0; ec_idx < ec_vec.size(); ec_idx++) {
                // for this addr and ec idx, draw grouped results
                std::vector<visual_sample>& grouped_results = memory_grouped_results[ec_it->first][ec_idx];
                uint64_t result_hit_total = 0;
                for (size_t result_idx = 0; result_idx < grouped_results.size(); result_idx++) {
                    result_hit_total += grouped_results[result_idx].hitc; // could also precalc in this while grouping above, if this is too slow
                }
                if (result_hit_total == 0) {
                    continue;
                }
                visual_sample& example_sample = grouped_results[0];
                std::map<uint64_t, std::vector<ec_info_rw>>::iterator ec_it = vinfo.ecs.find(example_sample.addr);
                word_line_i = std::distance(vinfo.ecs.begin(), ec_it);
                double x_start = example_sample.idx == 0 ? 0 : (double)(ec_it->second[example_sample.idx - 1].time2 - vinfo.start_time);
                double x_end = (double)(ec_it->second[example_sample.idx].time2 - vinfo.start_time);
                double max_width = x_end - x_start;
                double effective_line_width = line_width * (hPxF / 1000);
                if (effective_line_width * wPxF < 1) {
                    effective_line_width = 1 / wPxF;
                }
                double max_marker_nr = max_width / effective_line_width;
                uint64_t target_marker_count = result_hit_total > max_marker_nr ? max_marker_nr : result_hit_total;
                std::vector<uint64_t> unique_locations;
                for (size_t location_idx = 0; location_idx < target_marker_count; location_idx++) {
                    unique_locations.push_back(location_idx);
                }
                for (size_t result_idx = 0; result_idx < grouped_results.size(); result_idx++) {
                    visual_sample& draw_result = grouped_results[result_idx];
                    for (size_t result_hit = 0; result_hit < draw_result.hitc; result_hit++) {
                        if (tdraw_type != PRT_META_ALL && draw_result.combination != tdraw_type) {
                            continue;
                        }
                        uint64_t rn = fprng_uintn(&rand, unique_locations.size());
                        uint64_t draw_location = unique_locations[rn];
                        {
                            // swap erase location from vec
                            if (unique_locations.size() > 1) {
                                std::iter_swap(unique_locations.begin() + rn, unique_locations.end() - 1);
                                unique_locations.pop_back();
                            } else {
                                unique_locations.clear();
                            }
                        }
                        double x_pos = (x_start + max_width * ((double)draw_location / (double)target_marker_count)) / total_time;
                        cairo_set_source_rgb(ctx, type_colors[draw_result.combination][0], type_colors[draw_result.combination][1], type_colors[draw_result.combination][2]);
                        // cairo_arc(ctx, x_pos, word_space_height * word_line_i + word_space_height * 0.5, 0.003, 0, M_PI * 2);
                        cairo_move_to(ctx, x_pos, word_space_height * word_line_i);
                        cairo_line_to(ctx, x_pos, word_space_height * (word_line_i + 1) + 0.002);
                        cairo_set_line_width(ctx, effective_line_width);
                        cairo_stroke(ctx);
                    }
                }
            }
        }
    } else {
        // sampling dots
        for (uint64_t i = 0; i < vinfo.samples.size(); i++) {
            visual_sample& sample = vinfo.samples[i];
            if (tdraw_type != PRT_META_ALL && sample.combination != tdraw_type) {
                continue;
            }
            std::map<uint64_t, std::vector<ec_info_rw>>::iterator ec_it = vinfo.ecs.find(sample.addr);
            word_line_i = std::distance(vinfo.ecs.begin(), ec_it);
            double x_start = sample.idx == 0 ? 0 : (double)(ec_it->second[sample.idx - 1].time2 - vinfo.start_time);
            double x_end = (double)(ec_it->second[sample.idx].time2 - vinfo.start_time);
            double max_width = x_end - x_start;
            double effective_line_width = line_width * (hPxF / 1000);
            double max_marker_nr = max_width / effective_line_width;
            for (uint64_t hitid = 0; hitid < sample.hitc; hitid++) {
                double rn = general_f64_zto(fprng_rand64(&rand));
                double x_pos = (x_start + max_width * rn) / total_time;
                cairo_set_source_rgb(ctx, type_colors[sample.combination][0], type_colors[sample.combination][1], type_colors[sample.combination][2]);
                // cairo_arc(ctx, x_pos, word_space_height * word_line_i + word_space_height * 0.5, 0.003, 0, M_PI * 2);
                cairo_move_to(ctx, x_pos, word_space_height * word_line_i);
                cairo_line_to(ctx, x_pos, word_space_height * (word_line_i + 1) + 0.002);
                cairo_set_line_width(ctx, effective_line_width);
                cairo_stroke(ctx);
            }
        }
    }

    // drawing end
    //#####

    cairo_surface_write_to_png(surface, out_path);

    cairo_destroy(ctx);
    cairo_surface_destroy(surface);
}
