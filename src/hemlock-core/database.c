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
static db_package_t *select_packages (sqlite3 *db, char *sql_statement, 
                                      size_t max_n, size_t *n_out, FILE *log);


static char *
gen_package_values (db_package_t *package)
{
    char *result = NULL;
    char *escaped_list[] = {
        db_escape_null (),
        db_escape_text (package->name),
        db_escape_text (package->version),
        db_escape_text (package->homepage),
        db_escape_text (package->maintainer),
        db_escape_text (package->email),
        db_escape_boolean (package->as_dependency),
        db_escape_boolean (package->is_installed)
    };
    const size_t LIST_COUNT = sizeof (escaped_list) / sizeof (*escaped_list);

    /* validate all string */
    for (size_t i = 0; i < LIST_COUNT; i++)
    {
        if (NULL == escaped_list[i]) goto generate_values_free;
    }

    /* generate the concatenated string */
    result = string_join (escaped_list, LIST_COUNT, ", ");

generate_values_free:
    /* free allocated memory */
    for (size_t i = 0; i < LIST_COUNT; i++) {
        free (escaped_list[i]); escaped_list[i] = NULL;
    }

    return result;
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


char *
db_human_readable_package (db_package_t *package)
{
    return gen_package_sets (package);
}


void
db_free_package (db_package_t *package)
{
    if (NULL == package) return;

    free (package->name);       package->name       = NULL;
    free (package->email);      package->email      = NULL;
    free (package->maintainer); package->maintainer = NULL;
    free (package->version);    package->version    = NULL;
    free (package->homepage);   package->homepage   = NULL;
    package->valid = PACKAGE_INVALID;
    
    return;
}


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
db_search_package_id (sqlite3 *db, int id, FILE *log)
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
        "SELECT *\n"
        "FROM packages\n"
        "WHERE package_id = ", package_id, ";\n"
    };
    const size_t format_count = sizeof (format_arr) / sizeof (*format_arr);

    select_statement = string_join (format_arr, format_count, "");
    if (NULL == select_statement)
    {
        goto search_id_early_exit;
    }

    match = select_packages (db, select_statement, MATCH_MAX, 
                             &match_count, log);
    if ((0 == match_count) || (NULL == match))
    {
        fprintf (stderr, "Could not select package by id: %s\n", package_id);
    }

search_id_early_exit:
    free (select_statement); select_statement = NULL;
    free (package_id);       package_id = NULL;

    return match;
}


db_package_t *
db_search_packages (sqlite3 *db, char *name, char *version, size_t *n_out, 
                    FILE *log)
{
    char *select_statement = NULL;
    char *escaped_name = NULL, *escaped_version = NULL;

    db_package_t *match_arr = NULL;
    size_t match_count = 0;

    const char *DEFAULT_VERSION = "%";

    if ((NULL == db) || (NULL == name) || (NULL == n_out))
    {
        errno = EINVAL;
        goto search_name_early_exit;
    }

    version = (char *)(NULL == version ? DEFAULT_VERSION : version);

    escaped_version = db_escape_text (version);
    escaped_name    = db_escape_text (name);
    if ((NULL == escaped_name) ||
        (NULL == escaped_version))
    {
        goto search_name_early_exit;
    }

    char *format_arr[] = 
    {
        "SELECT *\n"
        "FROM packages\n"
        "WHERE name    like ", escaped_name, " AND\n"
        "      version like ", escaped_version, ";\n"
    };
    const size_t FORMAT_LEN = sizeof (format_arr) / sizeof (*format_arr);

    select_statement = string_join (format_arr, FORMAT_LEN, "");
    if (NULL == select_statement)
    {
        goto search_name_early_exit;
    }

    match_arr = select_packages (db, select_statement, SIZE_MAX, 
                                 &match_count, log);
    if ((0 == match_count) || (NULL == match_arr))
    {
        fprintf (stderr, "No matches for name:\"%s\", version:\"%s\"\n", 
                 escaped_name, escaped_version);
    }

search_name_early_exit:
    free (escaped_version);  escaped_version  = NULL;
    free (escaped_name);     escaped_name     = NULL;
    free (select_statement); select_statement = NULL;

    *n_out = match_count;
    return match_arr;
}

static db_package_t *
select_packages (sqlite3 *db, char *sql_statement, size_t max_n, 
                 size_t *n_out, FILE *log)
{
    int retcode = 0;
    size_t statement_length = 0;
    sqlite3_stmt *stmt = NULL;

    void *temp = NULL;
    const size_t DEFAULT_ALLOC = 3;
    size_t min_initial_alloc = 0;
    size_t result_count = 0;
    size_t result_alloc = 0;
    db_package_t *result = NULL;
    db_package_t *iter = NULL;

    db_result_t out = { .type = SQLITE_NULL, .s = NULL };
    char *col_name = NULL;
    size_t col_n = 0;

    if ((NULL == db) || (NULL == sql_statement) || (0 == max_n)
     || (NULL == n_out))
    {
        errno = EINVAL;
        goto select_package_exit;
    }

    if (NULL != log) fprintf (log, "%s", sql_statement);
    statement_length = strlen (sql_statement);
    retcode = sqlite3_prepare_v2 (db, sql_statement, (int)statement_length, 
                                  &stmt, NULL);
    if ((SQLITE_OK != retcode) || (NULL == stmt))
    {
        fprintf (stderr, "SQLite3 Error: %d: Failed to prepare statement\n", 
                 retcode);
        goto select_package_exit;
    }

    min_initial_alloc = min (max_n, DEFAULT_ALLOC);
    result = malloc (min_initial_alloc * sizeof (db_package_t));
    if (NULL == result)
    {
        errno = ENOMEM;
        goto select_package_exit;
    }
    result_alloc = min_initial_alloc;

    while (SQLITE_ROW == (retcode = sqlite3_step (stmt)))
    {
        /* check if max_n count has been reached */
        if (result_count == max_n) break;

        /* ensure that the result array is large enough */
        if (result_count == result_alloc)
        {
            temp = realloc (result, (result_alloc * 2) 
                                    * sizeof (db_package_t));
            /* if realloc fails, cleanup result and abort */
            if (NULL == temp)
            {
                free (result); result = NULL;
                result_count = 0;
                result_alloc = 0;
                errno = ENOMEM;
                goto select_package_exit;
            }
            result = temp;
            result_alloc *= 2;
        }

        /* define iterator */
        iter = result + result_count;

        /* define default values */
        (void)memset (iter, 0, sizeof (db_package_t));
        iter->valid = PACKAGE_INVALID;

        /* get values */
        col_n = sqlite3_column_count (stmt);
        for (int i = 0; i < col_n; i++)
        {
            (void)db_get_column (stmt, i, &out);
    
            col_name = (char *)sqlite3_column_name (stmt, i);
            if (NULL == col_name)
            {
                fprintf (stderr, "panic col_name == NULL\n");
            }

            if ((0 == strcmp (col_name, "package_id")) 
             && (SQLITE_INTEGER == out.type)) 
            {
                iter->package_id = out.i;
                iter->valid |= PACKAGE_VALID_PACKAGE_ID;
            }
            else if ((0 == strcmp (col_name, "name"))
                  && (SQLITE_TEXT == out.type))
            {
                iter->name = string_clone (out.s);
                iter->valid |= PACKAGE_VALID_NAME;
            }
            else if ((0 == strcmp (col_name, "version"))
                  && (SQLITE_TEXT == out.type))
            {
                iter->version = string_clone (out.s);
                iter->valid |= PACKAGE_VALID_VERSION;
            }
            else if ((0 == strcmp (col_name, "homepage"))
                  && ((SQLITE_TEXT == out.type)
                   || (SQLITE_NULL == out.type)))
            {
                iter->homepage = string_clone (out.s);
                iter->valid |= PACKAGE_VALID_HOMEPAGE;
            }
            else if ((0 == strcmp (col_name, "maintainer"))
                  && ((SQLITE_TEXT == out.type)
                   || (SQLITE_NULL == out.type)))
            {
                iter->maintainer = string_clone (out.s);
                iter->valid |= PACKAGE_VALID_MAINTAINER;
            }
            else if ((0 == strcmp (col_name, "email"))
                  && ((SQLITE_TEXT == out.type)
                   || (SQLITE_NULL == out.type)))
            {
                iter->email = string_clone (out.s);
                iter->valid |= PACKAGE_VALID_EMAIL;
            }
            else if ((0 == strcmp (col_name, "as_dependency")) 
                  && (SQLITE_INTEGER == out.type)) 
            {
                iter->as_dependency = (out.i == 1 ? true : false);
                iter->valid |= PACKAGE_VALID_AS_DEPENDENCY;
            }
            else if ((0 == strcmp (col_name, "is_installed")) 
                  && (SQLITE_INTEGER == out.type)) 
            {
                iter->is_installed = (out.i == 1 ? true : false);
                iter->valid |= PACKAGE_VALID_IS_INSTALLED;
            }
            else
            {
                fprintf (stderr, "SQLite Warning: skipping bad column\n");
            }

            
        }

        /* increment */
        result_count++;
    }

select_package_exit:
    /* clean up temporary memory */
    (void)sqlite3_reset (stmt);
    (void)sqlite3_finalize (stmt); stmt = NULL;

    /* return values */
    *n_out = result_count;
    return result;
}


/* end of file */