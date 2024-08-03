#include "database.h"
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <time.h>

// Hata kontrolü için bir makro tanımlayalım
#define CHECK_ERR(rc, msg) if (rc != SQLITE_OK && rc != SQLITE_DONE) { fprintf(stderr, "%s: %s\n", msg, sqlite3_errmsg(db)); return rc; }

char *cities[] = {"İstanbul", "Ankara", "İzmir", "Bursa", "Antalya"};
char *countries[] = {"Türkiye", "ABD", "Almanya", "Fransa", "İngiltere"};
char *countryCodes[] = {"TR", "US", "DE", "FR", "UK"};

#define CITY_COUNT (sizeof(cities) / sizeof(cities[0]))
#define COUNTRY_COUNT (sizeof(countries) / sizeof(countries[0]))
// Diğer kodları buraya ekleyin
// createTables, insertRandomData, updateAgeColumn fonksiyonları burada olmalı
// Bu fonksiyon, veritabanında tabloları oluşturur

int createTables(sqlite3 *db) {
    char *errMsg = 0;
    int rc;

    const char *sqlCreatePerson = "CREATE TABLE IF NOT EXISTS Person ("
                                  "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                  "first_name TEXT NOT NULL,"
                                  "last_name TEXT NOT NULL,"
                                  "birth_date TEXT NOT NULL,"
                                  "email TEXT NOT NULL,"
                                  "phone_number TEXT NOT NULL,"
                                  "age INTEGER);";
    rc = sqlite3_exec(db, sqlCreatePerson, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to create Person table: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    const char *sqlCreateAddress = "CREATE TABLE IF NOT EXISTS Address ("
                                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                   "person_id INTEGER,"
                                   "street TEXT NOT NULL,"
                                   "city_id INTEGER,"
                                   "postal_code TEXT NOT NULL,"
                                   "FOREIGN KEY(person_id) REFERENCES Person(id),"
                                   "FOREIGN KEY(city_id) REFERENCES City(id));";
    rc = sqlite3_exec(db, sqlCreateAddress, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to create Address table: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    const char *sqlCreateCity = "CREATE TABLE IF NOT EXISTS City ("
                                "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                "name TEXT NOT NULL,"
                                "country_id INTEGER,"
                                "FOREIGN KEY(country_id) REFERENCES Country(id));";
    rc = sqlite3_exec(db, sqlCreateCity, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to create City table: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    const char *sqlCreateCountry = "CREATE TABLE IF NOT EXISTS Country ("
                                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                   "name TEXT NOT NULL,"
                                   "code TEXT NOT NULL);";
    rc = sqlite3_exec(db, sqlCreateCountry, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to create Country table: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    return SQLITE_OK;
}

// Bu fonksiyon, rastgele verileri veritabanına ekler
int insertRandomData(sqlite3 *db, int personCount) {
    sqlite3_stmt *stmt;
    char *errMsg = 0;
    int rc;

    srand(time(NULL)); // Rastgele sayı üreticisini başlat

    // Rastgele ülkeleri ekleme
    for (int i = 0; i < COUNTRY_COUNT; i++) {
        const char *sqlInsertCountry = "INSERT INTO Country (name, code) VALUES (?, ?);";
        rc = sqlite3_prepare_v2(db, sqlInsertCountry, -1, &stmt, 0);
        CHECK_ERR(rc, "Failed to prepare Country insert statement");
        sqlite3_bind_text(stmt, 1, countries[i], -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, countryCodes[i], -1, SQLITE_STATIC);
        rc = sqlite3_step(stmt);
        CHECK_ERR(rc, "Failed to insert into Country");
        sqlite3_finalize(stmt);
    }

    // Rastgele şehirleri ekleme
    for (int i = 0; i < CITY_COUNT; i++) {
        const char *sqlInsertCity = "INSERT INTO City (name, country_id) VALUES (?, 1);"; // Türkiye'yi varsayılan olarak kullanıyoruz
        rc = sqlite3_prepare_v2(db, sqlInsertCity, -1, &stmt, 0);
        CHECK_ERR(rc, "Failed to prepare City insert statement");
        sqlite3_bind_text(stmt, 1, cities[i], -1, SQLITE_STATIC);
        rc = sqlite3_step(stmt);
        CHECK_ERR(rc, "Failed to insert into City");
        sqlite3_finalize(stmt);
    }

    // Rastgele kişileri ekleme
    for (int i = 0; i < personCount; i++) {
        const char *firstName = getRandomFirstName();
        const char *lastName = getRandomLastName();
        char email[100];
        snprintf(email, sizeof(email), "%s.%s@example.com", firstName, lastName);
        const char *birthDate = getRandomBirthDate();
        const char *phoneNumber = getRandomPhoneNumber();

        // Yaşı hesapla
        int age = calculateAge(birthDate);

        const char *sqlInsertPerson = "INSERT INTO Person (first_name, last_name, birth_date, email, phone_number, age) VALUES (?, ?, ?, ?, ?, ?);";
        rc = sqlite3_prepare_v2(db, sqlInsertPerson, -1, &stmt, 0);
        CHECK_ERR(rc, "Failed to prepare Person insert statement");
        sqlite3_bind_text(stmt, 1, firstName, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, lastName, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, birthDate, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, email, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 5, phoneNumber, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 6, age); // Yaşı bind et
        rc = sqlite3_step(stmt);
        CHECK_ERR(rc, "Failed to insert into Person");
        sqlite3_finalize(stmt);
    }

    // Rastgele adresleri ekleme
    for (int i = 0; i < personCount; i++) {
        const char *street = getRandomStreet();
        const char *city = getRandomCity();

        const char *sqlInsertAddress = "INSERT INTO Address (person_id, street, city_id, postal_code) VALUES (?, ?, 1, '12345');";
        rc = sqlite3_prepare_v2(db, sqlInsertAddress, -1, &stmt, 0);
        CHECK_ERR(rc, "Failed to prepare Address insert statement");
        sqlite3_bind_int(stmt, 1, i + 1);
        sqlite3_bind_text(stmt, 2, street, -1, SQLITE_STATIC);
        rc = sqlite3_step(stmt);
        CHECK_ERR(rc, "Failed to insert into Address");
        sqlite3_finalize(stmt);
    }

    return SQLITE_OK;
}

int updateAgeColumn(sqlite3 *db) {
    char *errMsg = 0;
    int rc;

    // Yaş sütunu ekleme
    const char *sqlAddAgeColumn = "ALTER TABLE Person ADD COLUMN age INTEGER;";
    rc = sqlite3_exec(db, sqlAddAgeColumn, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to add age column: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    // Yaşları güncelleme
    const char *sqlUpdateAge = "UPDATE Person SET age = (strftime('%Y', 'now') - strftime('%Y', birth_date)) - "
                               "(strftime('%m-%d', 'now') < strftime('%m-%d', birth_date));";
    rc = sqlite3_exec(db, sqlUpdateAge, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to update age column: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    return SQLITE_OK;
}


// Kodunuzu buraya taşıyın
