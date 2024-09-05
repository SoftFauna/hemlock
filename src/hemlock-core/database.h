/* HEMLOCK - a system independent package manager. */
/* <https://github.com/SoftFauna/HEMLOCK.git> */
/* Copyright (c) 2024 The SoftFauna Team */

#ifndef HEMLOCK_DATABASE_HEADER
#define HEMLOCK_DATABASE_HEADER
#ifdef __cplusplus  /* C++ compatibility */
extern "C" {
#endif
/* code start */

#include "database_core.h"      /* not necessary */
#include <sqlite3.h>
#include <stdio.h>


typedef struct
{
    char *name;
    char *version;
    char *homepage;
    char *maintainer;
    char *email;
    int package_id;
    bool as_dependency;
    bool is_installed;
} db_package_t;


typedef struct 
{
    int dependency_id;
    int dependant_id;
    int package_id;
} db_dependency_t;


typedef struct 
{
    char *path;
    int filelog_id;
    int package_id;
} db_filelog_t;


int db_create_tables (sqlite3 *db, FILE *log);
int db_insert_package (sqlite3 *db, db_package_t *package, FILE *log);
int db_update_package (sqlite3 *db, db_package_t *package, FILE *log);
db_package_t *db_search_package (sqlite3 *db, char *name, char *version);
db_package_t *db_search_package_id (sqlite3 *db, int id);


/* code end */
#ifdef __cplusplus  /* C++ compatibility */
}
#endif
#endif /* header guard */
/* end of file */