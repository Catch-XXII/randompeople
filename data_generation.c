#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <time.h>
#include "database.h"

// Global variables definitions
char *firstNames[] = {"Ahmet", "Mehmet", "Ayşe", "Fatma", "Ali", "Hasan", "Hüseyin", "Zeynep", "Emine", "Mustafa"};
char *lastNames[] = {"Yılmaz", "Kaya", "Demir", "Şahin", "Çelik", "Öztürk", "Arslan", "Doğan", "Korkmaz", "Özdemir"};
char *streets[] = {"Atatürk Cad.", "Cumhuriyet Cad.", "İstiklal Cad.", "Bağdat Cad.", "Barbaros Bulv."};
char *cities[] = {"İstanbul", "Ankara", "İzmir", "Bursa", "Antalya"};
char *countries[] = {"Türkiye", "ABD", "Almanya", "Fransa", "İngiltere"};
char *countryCodes[] = {"TR", "US", "DE", "FR", "UK"};
const char *turkTelekomCodes[] = {"501", "505", "506", "507", "551", "552", "553", "554", "555", "559"};
const char *turkcellCodes[] = {"516", "530", "531", "532", "533", "534", "535", "536", "537", "538", "539", "561"};
const char *vodafoneCodes[] = {"540", "541", "542", "543", "544", "545", "546", "547", "548", "549"};

// Global variables declaration
extern char *firstNames[];
extern char *lastNames[];
extern char *streets[];
extern char *cities[];
extern char *countries[];
extern char *countryCodes[];
extern const char *turkTelekomCodes[];
extern const char *turkcellCodes[];
extern const char *vodafoneCodes[];

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


#define FIRST_NAME_COUNT (sizeof(firstNames) / sizeof(firstNames[0]))
#define LAST_NAME_COUNT (sizeof(lastNames) / sizeof(lastNames[0]))
#define STREET_COUNT (sizeof(streets) / sizeof(streets[0]))
#define CITY_COUNT (sizeof(cities) / sizeof(cities[0]))
#define COUNTRY_COUNT (sizeof(countries) / sizeof(countries[0]))

#define TURK_TELEKOM_COUNT (sizeof(turkTelekomCodes) / sizeof(turkTelekomCodes[0]))
#define TURKCELL_COUNT (sizeof(turkcellCodes) / sizeof(turkcellCodes[0]))
#define VODAFONE_COUNT (sizeof(vodafoneCodes) / sizeof(vodafoneCodes[0]))

// Define a macro for error checking
#define CHECK_ERR(result_code, msg) if (result_code != SQLITE_OK && result_code != SQLITE_DONE) { fprintf(stderr, "%s: %s\n", msg, sqlite3_errmsg(db)); return result_code; }



// This function creates tables in the database
int createTables(sqlite3 *db) {
    char *errMsg = 0;
    int result_code;

    const char *sqlCreatePerson = "CREATE TABLE IF NOT EXISTS Person ("
                                  "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                  "first_name TEXT NOT NULL,"
                                  "last_name TEXT NOT NULL,"
                                  "birth_date TEXT NOT NULL,"
                                  "email TEXT NOT NULL,"
                                  "phone_number TEXT NOT NULL,"
                                  "age INTEGER);";
    result_code = sqlite3_exec(db, sqlCreatePerson, 0, 0, &errMsg);
    if (result_code != SQLITE_OK) {
        fprintf(stderr, "Failed to create Person table: %s\n", sqlite3_errmsg(db));
        return result_code;
    }

    const char *sqlCreateAddress = "CREATE TABLE IF NOT EXISTS Address ("
                                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                   "person_id INTEGER,"
                                   "street TEXT NOT NULL,"
                                   "city_id INTEGER,"
                                   "postal_code TEXT NOT NULL,"
                                   "FOREIGN KEY(person_id) REFERENCES Person(id),"
                                   "FOREIGN KEY(city_id) REFERENCES City(id));";
    result_code = sqlite3_exec(db, sqlCreateAddress, 0, 0, &errMsg);
    if (result_code != SQLITE_OK) {
        fprintf(stderr, "Failed to create Address table: %s\n", sqlite3_errmsg(db));
        return result_code;
    }

    const char *sqlCreateCity = "CREATE TABLE IF NOT EXISTS City ("
                                "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                "name TEXT NOT NULL,"
                                "country_id INTEGER,"
                                "FOREIGN KEY(country_id) REFERENCES Country(id));";
    result_code = sqlite3_exec(db, sqlCreateCity, 0, 0, &errMsg);
    if (result_code != SQLITE_OK) {
        fprintf(stderr, "Failed to create City table: %s\n", sqlite3_errmsg(db));
        return result_code;
    }

    const char *sqlCreateCountry = "CREATE TABLE IF NOT EXISTS Country ("
                                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                   "name TEXT NOT NULL,"
                                   "code TEXT NOT NULL);";
    result_code = sqlite3_exec(db, sqlCreateCountry, 0, 0, &errMsg);
    if (result_code != SQLITE_OK) {
        fprintf(stderr, "Failed to create Country table: %s\n", sqlite3_errmsg(db));
        return result_code;
    }

    return SQLITE_OK;
}

// Pick a random firstName
const char* getRandomFirstName() {
    return firstNames[rand() % FIRST_NAME_COUNT];
}

// Pick a random lastName
const char* getRandomLastName() {
    return lastNames[rand() % LAST_NAME_COUNT];
}

// Pick a random street
const char* getRandomStreet() {
    return streets[rand() % STREET_COUNT];
}

// Pick a random city
const char* getRandomCity() {
    return cities[rand() % CITY_COUNT];
}

// Pick a random country and country code
void getRandomCountry(char** name, char** code) {
    int index = rand() % COUNTRY_COUNT;
    *name = countries[index];
    *code = countryCodes[index];
}

// Random Birthdate generator
char* getRandomBirthDate() {
    static char birthDate[11];
    int year = (rand() % 50) + 1970;  // 1970-2019 arası bir yıl
    int month = (rand() % 12) + 1;    // 1-12 arası bir ay
    int day = (rand() % 28) + 1;      // 1-28 arası bir gün (şubat ayı için güvenli)
    snprintf(birthDate, sizeof(birthDate), "%04d-%02d-%02d", year, month, day);
    return birthDate;
}

// Age calculation function
int calculateAge(const char *birthDate) {
    int year, month, day;
    int age;
    time_t now;
    struct tm *tm_now;

    // Get current date
    time(&now);
    tm_now = localtime(&now);

    // Analyze date of birth
    sscanf(birthDate, "%4d-%2d-%2d", &year, &month, &day);

    // Calculate the age
    age = tm_now->tm_year + 1900 - year; // Year difference
    if (tm_now->tm_mon + 1 < month || (tm_now->tm_mon + 1 == month && tm_now->tm_mday < day)) {
        age--; // If the birthday hasn't come yet, reduce it by one year
    }

    return age;
}

int updateAgeColumn(sqlite3 *db) {
    char *errMsg = 0;
    int result_code;

    // Adding an age column
    const char *sqlAddAgeColumn = "ALTER TABLE Person ADD COLUMN age INTEGER;";
    result_code = sqlite3_exec(db, sqlAddAgeColumn, 0, 0, &errMsg);
    if (result_code != SQLITE_OK) {
        fprintf(stderr, "Failed to add age column: %s\n", sqlite3_errmsg(db));
        return result_code;
    }

    // Update ages
    const char *sqlUpdateAge = "UPDATE Person SET age = (strftime('%Y', 'now') - strftime('%Y', birth_date)) - "
                               "(strftime('%m-%d', 'now') < strftime('%m-%d', birth_date));";
    result_code = sqlite3_exec(db, sqlUpdateAge, 0, 0, &errMsg);
    if (result_code != SQLITE_OK) {
        fprintf(stderr, "Failed to update age column: %s\n", sqlite3_errmsg(db));
        return result_code;
    }

    return SQLITE_OK;
}

char* getRandomPhoneNumber() {
    static char phoneNumber[15]; 

    // Randomize GSM operators
    int operatorType = rand() % 3;
    const char **selectedCodes;
    int codeCount;

    switch (operatorType) {
        case 0:
            selectedCodes = turkTelekomCodes;
            codeCount = TURK_TELEKOM_COUNT;
            break;
        case 1:
            selectedCodes = turkcellCodes;
            codeCount = TURKCELL_COUNT;
            break;
        case 2:
            selectedCodes = vodafoneCodes;
            codeCount = VODAFONE_COUNT;
            break;
        default:
            selectedCodes = turkTelekomCodes;
            codeCount = TURK_TELEKOM_COUNT;
            break;
    }

    // Generate a random prefix and phone number
    const char *code = selectedCodes[rand() % codeCount];
    snprintf(phoneNumber, sizeof(phoneNumber), "%s%07d", code, rand() % 10000000);

    return phoneNumber;
}

// This function inserts random data into the database
int insertRandomData(sqlite3 *db, int personCount) {
    sqlite3_stmt *stmt;
    char *errMsg = 0;
    int result_code;

    srand(time(NULL)); // Initialize random number generator

    // Add random country
    for (int i = 0; i < COUNTRY_COUNT; i++) {
        const char *sqlInsertCountry = "INSERT INTO Country (name, code) VALUES (?, ?);";
        result_code = sqlite3_prepare_v2(db, sqlInsertCountry, -1, &stmt, 0);
        CHECK_ERR(result_code, "Failed to prepare Country insert statement");
        sqlite3_bind_text(stmt, 1, countries[i], -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, countryCodes[i], -1, SQLITE_STATIC);
        result_code = sqlite3_step(stmt);
        CHECK_ERR(result_code, "Failed to insert into Country");
        sqlite3_finalize(stmt);
    }

    // Add random city
    for (int i = 0; i < CITY_COUNT; i++) {
        const char *sqlInsertCity = "INSERT INTO City (name, country_id) VALUES (?, 1);"; // Türkiye'yi varsayılan olarak kullanıyoruz
        result_code = sqlite3_prepare_v2(db, sqlInsertCity, -1, &stmt, 0);
        CHECK_ERR(result_code, "Failed to prepare City insert statement");
        sqlite3_bind_text(stmt, 1, cities[i], -1, SQLITE_STATIC);
        result_code = sqlite3_step(stmt);
        CHECK_ERR(result_code, "Failed to insert into City");
        sqlite3_finalize(stmt);
    }

    // Add random people
    for (int i = 0; i < personCount; i++) {
        const char *firstName = getRandomFirstName();
        const char *lastName = getRandomLastName();
        char email[100];
        snprintf(email, sizeof(email), "%s.%s@example.com", firstName, lastName);
        const char *birthDate = getRandomBirthDate();
        const char *phoneNumber = getRandomPhoneNumber();

        // Calculate age
        int age = calculateAge(birthDate);

        const char *sqlInsertPerson = "INSERT INTO Person (first_name, last_name, birth_date, email, phone_number, age) VALUES (?, ?, ?, ?, ?, ?);";
        result_code = sqlite3_prepare_v2(db, sqlInsertPerson, -1, &stmt, 0);
        CHECK_ERR(result_code, "Failed to prepare Person insert statement");
        sqlite3_bind_text(stmt, 1, firstName, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, lastName, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, birthDate, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, email, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 5, phoneNumber, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 6, age); // Yaşı bind et
        result_code = sqlite3_step(stmt);
        CHECK_ERR(result_code, "Failed to insert into Person");
        sqlite3_finalize(stmt);
    }

    // Adding random addresses
    for (int i = 0; i < personCount; i++) {
        const char *street = getRandomStreet();
        const char *city = getRandomCity();

        const char *sqlInsertAddress = "INSERT INTO Address (person_id, street, city_id, postal_code) VALUES (?, ?, 1, '12345');";
        result_code = sqlite3_prepare_v2(db, sqlInsertAddress, -1, &stmt, 0);
        CHECK_ERR(result_code, "Failed to prepare Address insert statement");
        sqlite3_bind_int(stmt, 1, i + 1);
        sqlite3_bind_text(stmt, 2, street, -1, SQLITE_STATIC);
        result_code = sqlite3_step(stmt);
        CHECK_ERR(result_code, "Failed to insert into Address");
        sqlite3_finalize(stmt);
    }

    return SQLITE_OK;
}