#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>

int createTables(sqlite3 *db);
int insertRandomData(sqlite3 *db, int personCount);
int updateAgeColumn(sqlite3 *db);

#endif
