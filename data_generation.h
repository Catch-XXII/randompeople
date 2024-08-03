#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>

// Global değişkenlerin bildirilmesi
extern char *firstNames[];
extern char *lastNames[];
extern char *streets[];
extern char *cities[];
extern char *countries[];
extern char *countryCodes[];
extern const char *turkTelekomCodes[];
extern const char *turkcellCodes[];
extern const char *vodafoneCodes[];

// Fonksiyon prototipleri
int createTables(sqlite3 *db);
int insertRandomData(sqlite3 *db, int personCount);
int updateAgeColumn(sqlite3 *db);

const char* getRandomFirstName();
const char* getRandomLastName();
const char* getRandomStreet();
const char* getRandomCity();
void getRandomCountry(char** name, char** code);
char* getRandomBirthDate();
char* getRandomPhoneNumber();
int calculateAge(const char *birthDate);

#endif
