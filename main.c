#include <stdio.h>
#include <sqlite3.h>
#include "database.h"

int main() {
    sqlite3 *db;
    int result_code;

    // Opening the database
    result_code = sqlite3_open("example.db", &db);
    if (result_code) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return result_code;
    }

    // Creating tables
    result_code = createTables(db);
    if (result_code != SQLITE_OK) {
        fprintf(stderr, "Table creation failed\n");
        return result_code;
    }

    // Adding random data
    result_code = insertRandomData(db, 10000);
    if (result_code != SQLITE_OK) {
        fprintf(stderr, "Data insertion failed\n");
        return result_code;
    }

    printf("Data inserted successfully\n");

    // Close the database
    sqlite3_close(db);
    return result_code;
}
