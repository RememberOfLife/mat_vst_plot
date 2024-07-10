#include <cstdint>
#include <cstring>
#include <vector>
#include <mysql.h>

#include "visual_info.hpp"
#include "util.hpp"

#include "mode/modes.hpp"

MYSQL* conn;

void mysql_graceful_shutdown()
{
    mysql_close(conn);
}

void mode_result_writeout_database(const char* in_path, const char* out_path, const char* bench_name, const char* variant_name)
{
    conn = mysql_init(NULL);
    if (conn == NULL) {
        errorf("mysql init error\n");
    }
    error_set_cleanup(mysql_graceful_shutdown);
    char conf_file_path[512];
    snprintf(conf_file_path, 512, "%s/.my.cnf", getenv("HOME"));
    if (mysql_options(conn, MYSQL_READ_DEFAULT_FILE, conf_file_path) != 0) {
        errorf("mysql options error: file\n");
    }
    if (mysql_options(conn, MYSQL_READ_DEFAULT_GROUP, "client") != 0) {
        errorf("mysql options error: group\n");
    }
    if (mysql_real_connect(conn, NULL, NULL, NULL, NULL, 0, NULL, 0) == NULL) {
        errorf("mysql connect error\n");
    }

    visual_info vinfo;
    vinfo.read_from_file(in_path);
    // drop samples from vinfo, we will now push results instead, and use the existing ec information in place
    vinfo.samples.clear();

    char query_str_buf[1024];

    uint64_t variant_id;
    {
        sprintf(query_str_buf, "SELECT v.id FROM variant AS v WHERE v.variant = '%s' AND v.benchmark = '%s'", variant_name, bench_name);
        if (mysql_query(conn, query_str_buf)) {
            errorf("mysql variant id: query error\n");
        }
        MYSQL_RES* result = mysql_store_result(conn);
        if (result == NULL) {
            errorf("mysql variant id: store result error\n");
        }
        if (mysql_num_rows(result) > 1) {
            errorf("mysql variant id: too many rows error\n");
        }
        MYSQL_ROW row = mysql_fetch_row(result);
        variant_id = strtoull(row[0], NULL, 10);
        printf("variant id: %lu\n", variant_id);
        mysql_free_result(result);
    }
    {
        sprintf(
            query_str_buf,
            "SELECT t.data_address, t.time2, g.weight, r.resulttype"
            " FROM fspgroup g"
            " JOIN result_GenericExperimentMessage r"
            " ON g.pilot_id = r.pilot_id"
            " JOIN trace t"
            " ON g.variant_id = t.variant_id AND g.data_address = t.data_address AND g.instr2 = t.instr2"
            " WHERE g.variant_id = %lu"
            ";",
            variant_id
        );
        if (mysql_query(conn, query_str_buf)) {
            errorf("mysql result types: query error\n");
        }
        MYSQL_RES* result = mysql_store_result(conn);
        if (result == NULL) {
            errorf("mysql result types: store result error\n");
        }
        if (mysql_num_rows(result) == 0) {
            errorf("mysql result types: no rows error\n");
        }
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(result))) {
            uint64_t r_addr = strtoull(row[0], NULL, 10);
            uint64_t r_time2 = strtoull(row[1], NULL, 10);
            uint64_t r_weight = strtoull(row[2], NULL, 10);
            const char* r_type_str = row[3];
            uint64_t r_type = PRT_COUNT;
            for (int rtidx = 0; rtidx < PRT_COUNT; rtidx++) {
                if (strcmp(r_type_str, pilot_result_str[rtidx]) == 0) {
                    r_type = rtidx;
                    break;
                }
            }
            if (r_type == PRT_COUNT) {
                errorf("mysql result types: type unknown '%s'\n", r_type_str);
            }
            vinfo.push_sample(r_addr, r_time2, r_weight, r_type);
        }
        mysql_free_result(result);
    }

    vinfo.write_to_file(out_path);
    mysql_close(conn);
}
