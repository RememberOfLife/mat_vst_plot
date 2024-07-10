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

void mode_result_writeout_database(const char* out_path)
{
    conn = mysql_init(NULL);
    if (conn == NULL) {
        errorf("#TODO\n");
    }
    error_set_cleanup(mysql_graceful_shutdown);
    char conf_file_path[512];
    snprintf(conf_file_path, 512, "%s/.my.cnf", getenv("HOME"));
    if (mysql_options(conn, MYSQL_READ_DEFAULT_FILE, conf_file_path) != 0) {
        errorf("#TODO\n");
    }
    if (mysql_options(conn, MYSQL_READ_DEFAULT_GROUP, "client") != 0) {
        errorf("#TODO\n");
    }
    if (mysql_real_connect(conn, NULL, NULL, NULL, NULL, 0, NULL, 0) == NULL) {
        errorf("#TODO\n");
    }
    // read from here

    if (mysql_query(conn, "SHOW TABLES")) {
        fprintf(stderr, "SHOW TABLES failed: %s\n", mysql_error(conn));
        mysql_close(conn);
        exit(EXIT_FAILURE);
    }

    MYSQL_RES* result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "mysql_store_result() failed: %s\n", mysql_error(conn));
        mysql_close(conn);
        exit(EXIT_FAILURE);
    }

    printf("Tables in the database:\n");
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        printf("%s\n", row[0]);
    }

    mysql_free_result(result);

    // done reading
    mysql_close(conn);
}
