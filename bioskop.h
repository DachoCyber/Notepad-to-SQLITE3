#ifndef _BIOSKOP_H_
#define _BIOSKOP_H_

#include "sqlite3.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int id;
    char ime[50];
    char prezime[50];
    char film[100];
    double cena;
    int sediste;
} Korisnik;

void retrieve(sqlite3** db);
int insertIntoTable(Korisnik korisnici[], int brKorisnika, sqlite3_stmt** stmt, sqlite3* db);
int checkAndDeleteDuplicates(sqlite3* db);

#endif
