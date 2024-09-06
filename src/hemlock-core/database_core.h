/* HEMLOCK - a system independent package manager. */
/* <https://github.com/SoftFauna/HEMLOCK.git> */
/* Copyright (c) 2024 The SoftFauna Team */

#ifndef HEMLOCK_DATABASE_CORE_HEADER
#define HEMLOCK_DATABASE_CORE_HEADER
#ifdef __cplusplus  /* C++ compatibility */
extern "C" {
#endif
/* code start */

#include <sqlite3.h>
#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>


typedef struct 
{
    int type;
    union
    {
        int i;
        float f;
        char *s;
        unsigned char *b;
    };
} db_result_t;


sqlite3 *db_open (const char *filename);
void db_close (sqlite3 *db);

int db_execute (sqlite3 *db, const char *SQL_SCRIPT, FILE *log);

char *db_escape_null (void);
char *db_escape_text (char *data);
char *db_escape_boolean (bool data);
char *db_escape_integer (int data);

int db_get_column (sqlite3_stmt *stmt, int i, db_result_t *result_out);

/* code end */
#ifdef __cplusplus  /* C++ compatibility */
}
#endif
#endif /* header guard */
/* end of file */