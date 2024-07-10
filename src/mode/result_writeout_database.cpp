#include <cstdint>
#include <map>
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

    // query: select v.id from variant as v where v.variant LIKE "base_binarysearch_50000_none_8/0_L1_R1_tecs" and v.benchmark LIKE "base_binarysearch";
    // should return exactyl one match, otherwise error, that is the id column with one row, the variant id.

    /* query: 
SELECT 
    t.data_address,
    t.time2,
    g.weight,
    r.resulttype
FROM 
    fspgroup g
JOIN 
    result_GenericExperimentMessage r ON g.pilot_id = r.pilot_id
JOIN 
    trace t ON g.variant_id = t.variant_id AND g.data_address = t.data_address AND g.instr2 = t.instr2
WHERE 
    g.variant_id = 1
;
    */
    // should return list with exactly those infos

    char query_str_buf[1024];
    {
        sprintf(query_str_buf, "SELECT v.id FROM variant AS v WHERE v.variant = '%s' AND v.benchmark = '%s'", variant_name, bench_name);
        if (mysql_query(conn, query_str_buf)) {
            errorf("mysql variant id: query error");
        }
        MYSQL_RES* result = mysql_store_result(conn);
        if (result == NULL) {
            errorf("mysql variant id: result error");
        }
        if (mysql_num_rows(result) > 1) {
            errorf("mysql variant id: too many rows error");
        }
        MYSQL_ROW row = mysql_fetch_row(result);
        uint64_t variant_id = strtoull(row[0], NULL, 10);
        printf("variant id: %lu\n", variant_id);
        mysql_free_result(result);
    }
    {
    }

    vinfo.write_to_file(out_path);
    mysql_close(conn);
}
