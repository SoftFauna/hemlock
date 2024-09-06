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
#include <stdint.h>
#include <stdio.h>


typedef struct
{
    char *name;
    char *version;
    char *homepage;
    char *maintainer;
    char *email;
    int package_id;
    uint32_t valid;
    bool as_dependency;
    bool is_installed;
} db_package_t;

const enum
{
    PACKAGE_INVALID             = 0x0000,
    PACKAGE_VALID_NAME          = 0x0001,
    PACKAGE_VALID_VERSION       = 0x0002,
    PACKAGE_VALID_HOMEPAGE      = 0x0004,
    PACKAGE_VALID_MAINTAINER    = 0x0008,
    PACKAGE_VALID_EMAIL         = 0x0010,
    PACKAGE_VALID_PACKAGE_ID    = 0x0020,
    PACKAGE_VALID_AS_DEPENDENCY = 0x0040,
    PACKAGE_VALID_IS_INSTALLED  = 0x0080 
};


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
db_package_t *db_search_package_id (sqlite3 *db, int id, FILE *log);

char *db_human_readable_package (db_package_t *package);
void db_free_package (db_package_t *package);

/* code end */
#ifdef __cplusplus  /* C++ compatibility */
}
#endif
#endif /* header guard */
/* end of file */