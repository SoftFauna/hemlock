/* HEMLOCK - a system independent package manager. */
/* <https://github.com/SoftFauna/HEMLOCK.git> */
/* Copyright (c) 2024 The SoftFauna Team */

#include "database.h"

#include "database_core.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_utils.h"


static char *gen_package_values (db_package_t *package);
static char *gen_package_sets (db_package_t *package);


int
db_create_tables (sqlite3 *db, FILE *log)
{
    const char *SQL_CREATE_TABLES = 
    {
        "CREATE TABLE IF NOT EXISTS packages (\n"
        "    package_id INTEGER PRIMARY KEY,\n"
        "    name TEXT NOT NULL,\n"
        "    version TEXT NOT NULL,\n"
        "    homepage TEXT,\n"
        "    maintainer TEXT,\n"
        "    email TEXT,\n"
        "    as_dependency BOOLEAN,\n"
        "    is_installed BOOLEAN\n"
        ");\n"
        "CREATE TABLE IF NOT EXISTS dependencies (\n"
        "    dependency_id INTEGER PRIMARY KEY,\n"
        "    dependant_id INTEGER NOT NULL,\n"
        "    package_id INTEGER NOT NULL,\n"
        "    FOREIGN KEY(dependant_id) REFERENCES packages(package_id),\n"
        "    FOREIGN KEY(package_id)   REFERENCES packages(package_id)\n"
        ");\n"
        "CREATE TABLE IF NOT EXISTS filelogs (\n"
        "    filelog_id INTEGER PRIMARY KEY,\n"
        "    path TEXT NOT NULL,\n"
        "    package_id INTEGER NOT NULL,\n"
        "    FOREIGN KEY(package_id) REFERENCES package(package_id)\n"
        ");\n"
    };

    return db_execute (db, SQL_CREATE_TABLES, log);
}


static char *
gen_package_values (db_package_t *package)
{
    char *result = NULL;
    const enum
    {
        LIST_PACKAGE_ID,
        LIST_NAME,
        LIST_VERSION,
        LIST_HOMEPAGE,
        LIST_MAINTAINER,
        LIST_EMAIL,
        LIST_AS_DEPENDENCY,
        LIST_IS_INSTALLED,
        LIST_COUNT
    };
    char *escaped_list[LIST_COUNT] = {
        [LIST_PACKAGE_ID]    = db_escape_null (),
        [LIST_NAME]          = db_escape_text (package->name),
        [LIST_VERSION]       = db_escape_text (package->version),
        [LIST_HOMEPAGE]      = db_escape_text (package->homepage),
        [LIST_MAINTAINER]    = db_escape_text (package->maintainer),
        [LIST_EMAIL]         = db_escape_text (package->email),
        [LIST_AS_DEPENDENCY] = db_escape_boolean (package->as_dependency),
        [LIST_IS_INSTALLED]  = db_escape_boolean (package->is_installed),
    };

    /* validate all string */
    for (int i = 0; i < LIST_COUNT; i++)
    {
        if (NULL == escaped_list[i]) goto generate_values_free;
    }

    /* generate the concatenated string */
    result = string_join (escaped_list, LIST_COUNT, ", ");

generate_values_free:
    /* free allocated memory */
    for (int i = 0; i < LIST_COUNT; i++) {
        free (escaped_list[i]); escaped_list[i] = NULL;
    }

    return result;
}


int
db_insert_package (sqlite3 *db, db_package_t *package, FILE *log)
{
    int retcode = -1;
    char *escaped_values = NULL;
    char *insert_statement = NULL;
    
    if ((NULL == db) || (NULL == package)) 
    {
        errno = EINVAL;
        goto insert_early_exit;
    }

    escaped_values = gen_package_values (package);
    if (NULL == escaped_values) 
    {
        goto insert_early_exit;
    }

    char *format_arr[] = 
    {
        "INSERT INTO packages (package_id,name,version,homepage,maintainer,\n"
        "                      email,as_dependency,is_installed)\n"
        "VALUES ( ", escaped_values, " );\n"
    };
    const size_t format_count = sizeof (format_arr) / sizeof (*format_arr);

    insert_statement = string_join (format_arr, format_count, "");
    if (NULL == insert_statement)
    {
        goto insert_early_exit;
    }

    retcode = db_execute (db, insert_statement, log);

insert_early_exit:
    free (insert_statement); insert_statement = NULL;
    free (escaped_values);   escaped_values = NULL;

    return retcode;

}


static char *
gen_package_sets (db_package_t *package)
{
    char *result = NULL;
    const enum
    {
        LIST_NAME,
        LIST_VERSION,
        LIST_HOMEPAGE,
        LIST_MAINTAINER,
        LIST_EMAIL,
        LIST_AS_DEPENDENCY,
        LIST_IS_INSTALLED,
        LIST_COUNT
    };
    char *escaped_list[LIST_COUNT] = {
        [LIST_NAME]          = db_escape_text (package->name),
        [LIST_VERSION]       = db_escape_text (package->version),
        [LIST_HOMEPAGE]      = db_escape_text (package->homepage),
        [LIST_MAINTAINER]    = db_escape_text (package->maintainer),
        [LIST_EMAIL]         = db_escape_text (package->email),
        [LIST_AS_DEPENDENCY] = db_escape_boolean (package->as_dependency),
        [LIST_IS_INSTALLED]  = db_escape_boolean (package->is_installed),
    };
    char *set_format[] = {
        "name=", escaped_list[LIST_NAME], ", ",
        "version=", escaped_list[LIST_VERSION], ", ",
        "homepage=", escaped_list[LIST_HOMEPAGE], ", ",
        "maintainer=", escaped_list[LIST_MAINTAINER], ", ",
        "email=", escaped_list[LIST_EMAIL], ", ",
        "as_dependency=", escaped_list[LIST_AS_DEPENDENCY], ", "
        "is_installed=", escaped_list[LIST_IS_INSTALLED]
    };
    const size_t FORMAT_LEN = sizeof (set_format) / sizeof (*set_format);

    /* validate all entries */
    for (int i = 0; i < LIST_COUNT; i++)
    {
        if (NULL == escaped_list[i]) goto generate_sets_free;
    }

    /* generate the concatenated string */
    result = string_join (set_format, FORMAT_LEN, "");

generate_sets_free:
    /* free allocated memory */
    for (int i = 0; i < LIST_COUNT; i++) {
        free (escaped_list[i]); escaped_list[i] = NULL;
    }

    return result;
}


int
db_update_package (sqlite3 *db, db_package_t *package, FILE *log)
{
    int retcode = -1;
    char *package_id = NULL;
    char *escaped_sets = NULL;
    char *update_statement = NULL;
    
    if ((NULL == db) || (NULL == package)) 
    {
        errno = EINVAL;
        goto update_early_exit;
    }

    package_id = int_to_string (package->package_id);
    escaped_sets = gen_package_sets (package);
    if ((NULL == package_id) || (NULL == escaped_sets)) 
    {
        goto update_early_exit;
    }

    char *format_arr[] = 
    {
        "UPDATE packages\n"
        "SET ", escaped_sets, "\n"
        "WHERE package_id = ", package_id, ";\n"
    };
    const size_t format_count = sizeof (format_arr) / sizeof (*format_arr);

    update_statement = string_join (format_arr, format_count, "");
    if (NULL == update_statement)
    {
        goto update_early_exit;
    }

    retcode = db_execute (db, update_statement, log);

update_early_exit:
    free (update_statement); update_statement = NULL;
    free (escaped_sets);     escaped_sets = NULL;
    free (package_id);       package_id = NULL;

    return retcode;
}


db_package_t *
db_search_package_id (sqlite3 *db, int id)
{ 
    char *select_statement = NULL;
    char *package_id = NULL;

    db_package_t *match = NULL;
    size_t match_count = 0;
    const size_t MATCH_MAX = 1;

    if (NULL == db)
    {
        errno = EINVAL;
        goto search_id_early_exit;
    }    
    
    package_id = int_to_string (id);
    if (NULL == package_id)
    {
        goto search_id_early_exit;
    }

    char *format_arr[] = 
    {
        "FROM packages\n"
        "SELECT *\n"
        "WHERE package_id = ", package_id, ";\n"
    };
    const size_t format_count = sizeof (format_arr) / sizeof (*format_arr);

    select_statement = string_join (format_arr, format_count, "");
    if (NULL == select_statement)
    {
        goto search_id_early_exit;
    }

    match = db_select_packages (db, select_statement, MATCH_MAX, &match_count);

search_id_early_exit:
    free (select_statement); select_statement = NULL;
    free (package_id);       package_id = NULL;

    return match;
}


static db_package_t *
db_select_package (sqlite3 *db, char *sql_statement, size_t max, 
                   size_t *found_count_out)
{
    if ((NULL == db) || (NULL == sql_statement))
    {
        errno = EINVAL;
        return NULL;
    }

    

}

/* end of file */