#include "sqlite3.h"
#include <stdio.h>
#include <stdlib.h>
#include "bioskop.h"


int openDatabase(sqlite3** db) {
    int rc;
    rc = sqlite3_open("korisnici.db", db);
    if (rc) {
        return 1;
    }
    return 0;
}

int insertIntoTable(Korisnik korisnici[], int brKorisnika, sqlite3_stmt** stmt, sqlite3* db) {
    char* query = "INSERT INTO korisnik(ime, prezime, film, cena, sediste) VALUES(?, ?, ?, ?, ?)";

    if (sqlite3_prepare_v2(db, query, -1, stmt, NULL) != SQLITE_OK) {
        printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    for (int i = 0; i < brKorisnika; i++) {
        sqlite3_bind_text(*stmt, 1, korisnici[i].ime, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(*stmt, 2, korisnici[i].prezime, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(*stmt, 3, korisnici[i].film, -1, SQLITE_TRANSIENT);
        sqlite3_bind_double(*stmt, 4, korisnici[i].cena);
        sqlite3_bind_int(*stmt, 5, korisnici[i].sediste);

        if (sqlite3_step(*stmt) != SQLITE_DONE) {
            printf("Failed to execute statement: %s\n", sqlite3_errmsg(db));
            sqlite3_finalize(*stmt);
            return 1;
        }

        sqlite3_reset(*stmt);
    }

    sqlite3_finalize(*stmt);

    printf("Data inserted successfully.\n");
    return 0;
}

void retrieve(sqlite3** db) {
    const char* query = "SELECT * FROM korisnik";
    sqlite3_stmt* stmt;
    int result = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    if(result != SQLITE_OK) {
        printf("Error: %s", sqlite3_errmsg(*db));
    }
    else {
        while((result = sqlite3_step(stmt)) == SQLITE_ROW) {
            printf("%s ", sqlite3_column_text(stmt, 0));
            printf("%s ", sqlite3_column_text(stmt, 1));
            printf("%s ", sqlite3_column_text(stmt, 2));
            printf("%s ", sqlite3_column_text(stmt, 3));
            printf("%s\n", sqlite3_column_text(stmt, 4));
        }
    }
}

int checkAndDeleteDuplicates(sqlite3* db) {
    sqlite3_stmt* stmt;
    const char* selectQuery = "SELECT id, ime, prezime, film, cena, sediste FROM korisnik";

    // Prepare select statement
    if (sqlite3_prepare_v2(db, selectQuery, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    Korisnik* korisnici = malloc(1000 * sizeof(Korisnik)); // Adjust size as needed
    int count = 0;

    // Execute the select statement and store the results
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        korisnici[count].id = sqlite3_column_int(stmt, 0);
        strcpy(korisnici[count].ime, (const char*)sqlite3_column_text(stmt, 1));
        strcpy(korisnici[count].prezime, (const char*)sqlite3_column_text(stmt, 2));
        strcpy(korisnici[count].film, (const char*)sqlite3_column_text(stmt, 3));
        korisnici[count].cena = sqlite3_column_double(stmt, 4);
        korisnici[count].sediste = sqlite3_column_int(stmt, 5);
        count++;
    }

    sqlite3_finalize(stmt); // Finalize the select statement

    // Identify and delete duplicates
    for (int i = 0; i < count; i++) {
        for (int j = i + 1; j < count; j++) {
            if (strcmp(korisnici[i].ime, korisnici[j].ime) == 0 &&
                strcmp(korisnici[i].prezime, korisnici[j].prezime) == 0 &&
                strcmp(korisnici[i].film, korisnici[j].film) == 0 &&
                korisnici[i].cena == korisnici[j].cena &&
                korisnici[i].sediste == korisnici[j].sediste) {

                // Duplicate found, delete it
                char deleteQuery[256];
                snprintf(deleteQuery, sizeof(deleteQuery), "DELETE FROM korisnik WHERE id = %d;", korisnici[j].id);

                char* errMsg = 0;
                int rc = sqlite3_exec(db, deleteQuery, 0, 0, &errMsg);
                if (rc != SQLITE_OK) {
                    printf("SQL error: %s\n", errMsg);
                    sqlite3_free(errMsg);
                    free(korisnici);
                    return 1;
                }
            }
        }
    }

    free(korisnici); // Free allocated memory
    printf("Duplicates deleted successfully.\n");
    return 0;
}
