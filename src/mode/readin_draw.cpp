#include <cmath>
#include <cstdint>
#include <map>
#include <vector>

#include <cairo/cairo.h>

#include "visual_info.hpp"
#include "util.hpp"

#include "mode/modes.hpp"

void mode_readin_draw(const char* in_path, const char* out_path)
{
    visual_info vinfo;
    vinfo.read_from_file(in_path);
    //TODO

    const int wPx = 1800;
    const int hPx = 950;
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

    double total_time = (double)(vinfo.stop_time - vinfo.start_time);

    uint64_t word_count = vinfo.ecs.size();
    double word_space_height = (double)1 / (double)word_count;

    double line_width = 0.001; //TODO find proper linewidth dynamically

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
            if (ec_vec[ec_idx].write_no_read) {
                cairo_set_source_rgb(ctx, 1, 0, 0);
            } else {
                cairo_set_source_rgb(ctx, 0, 0, 0);
            }
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
    for (uint64_t i = 1; i < word_count; i++) {
        cairo_set_source_rgb(ctx, 0, 0, 0);
        cairo_move_to(ctx, 0, word_space_height * i);
        cairo_line_to(ctx, 1, word_space_height * i);
        cairo_set_line_width(ctx, line_width);
        cairo_stroke(ctx);
    }

    // cairo_set_line_width(ctx, 0.1);
    // cairo_set_source_rgb(ctx, 0, 0, 0);
    // cairo_rectangle(ctx, 0.25, 0.25, 0.5, 0.5);
    // cairo_stroke(ctx);

    // cairo_set_source_rgb(ctx, 1, 0, 0);
    // cairo_rectangle(ctx, 0.15, 0.15, 0.2, 0.2);
    // cairo_fill(ctx);

    // cairo_set_source_rgb(ctx, 0, 0, 1);
    // cairo_move_to(ctx, 0.1, 0.9);
    // cairo_line_to(ctx, 0.9, 0.1);
    // cairo_set_line_width(ctx, 0.05);
    // cairo_stroke(ctx);

    // cairo_set_source_rgb(ctx, 0, 1, 0);
    // cairo_arc(ctx, 0.75, 0.75, 0.15, 0 * M_PI, 2 * M_PI);
    // cairo_set_line_width(ctx, 0.075);
    // cairo_stroke(ctx);

    // drawing end
    //#####

    cairo_surface_write_to_png(surface, out_path);

    cairo_destroy(ctx);
    cairo_surface_destroy(surface);
}
