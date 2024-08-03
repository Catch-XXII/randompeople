#include <stdio.h>
#include <sqlite3.h>
#include "database.h"

int main() {
    sqlite3 *db;
    int rc;

    // Opening the database
    rc = sqlite3_open("example.db", &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    // Creating tables
    rc = createTables(db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Table creation failed\n");
        return rc;
    }

    // Adding random data
    rc = insertRandomData(db, 100);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Data insertion failed\n");
        return rc;
    }

    printf("Data inserted successfully\n");

    // Close the database
    sqlite3_close(db);
    return rc;
}
