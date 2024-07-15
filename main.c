#include <stdio.h>
#include "sqlite3.h"
#include <string.h>
#include "bioskop.h"




int main() {
    sqlite3* db;
    sqlite3_stmt* stmt;

    int rc = openDatabase(&db);
    if(rc) {
        printf("Error: %s", sqlite3_errmsg(db));
        return 0;
    }

    char* createTableQuery = "CREATE TABLE IF NOT EXISTS korisnik ("
                             "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                             "ime TEXT NOT NULL,"
                             "prezime TEXT NOT NULL,"
                             "film TEXT NOT NULL,"
                             "cena REAL NOT NULL,"
                             "sediste INTEGER NOT NULL);";

    rc = sqlite3_exec(db, createTableQuery, 0, 0, 0);
    if (rc != SQLITE_OK) {
        printf("SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    Korisnik korisnici[50];
    FILE* fin = fopen("bioskop.txt", "r");
    if(fin == NULL) {
        fprintf(stderr, "Failed to open file bioskop.txt\n");
        sqlite3_close(db);
        return 1;
    }

    int n;
    fscanf(fin, "%d", &n);
    int i = 0;
    while(i < n) {
        fscanf(fin, "%s %s %s %lf %d", korisnici[i].ime, korisnici[i].prezime, korisnici[i].film, &korisnici[i].cena, &korisnici[i].sediste);
        i++;
    }
    fclose(fin);

    if (insertIntoTable(korisnici, n, &stmt, db) != 0) {
        printf("Failed to insert data.\n");
    }
    checkAndDeleteDuplicates(db);

    sqlite3_close(db);

    return 0;
}
