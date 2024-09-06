/* HEMLOCK - a system independent package manager. */
/* <https://github.com/SoftFauna/HEMLOCK.git> */
/* Copyright (c) 2024 The SoftFauna Team */

#include "database_core.h"

#include <errno.h>
#include <sqlite3.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "string_utils.h"


static void log_sql_error (int errcode, const char *errmsg);


static void
log_sql_error (int errcode, const char *errmsg)
{
    fprintf (stderr, "SQLite3 Error: %d: %s\n", errcode, errmsg);
    return;
}


sqlite3 *
db_open (const char *filename)
{
    /* try to open filename as a sqlite3 database */
    sqlite3 *db = NULL;
    int retcode = sqlite3_open (filename, &db);
    if (SQLITE_OK != retcode)   /* if the database cannot be opened */
    {
        /* throw an error */
        log_sql_error (retcode, sqlite3_errmsg (db));
        if (NULL != db) 
        {
            /* and free db if allocated */
            db_close (db); db = NULL;
        }

        /* return NULL */
        return NULL;
    }

    /* return the database pointer */
    return db;
}


void
db_close (sqlite3 *db)
{
    /* guard against null */
    if (NULL == db) return;

    /* purpetually try to close the database, until it succeeds */
    while (SQLITE_OK != sqlite3_close (db)) {}

    return;
}


int
db_execute (sqlite3 *db, const char *SQL_SCRIPT, FILE *log)
{
    char *errmsg = NULL;
    int retcode;

    /* NULL deref guard */
    if ((NULL == db) || (NULL == SQL_SCRIPT))
    {
        errno = EINVAL;
        return -1;
    }

    /* log the statement to stdout */
    if (NULL != log) fprintf (log, "%s\n", SQL_SCRIPT);

    /* process the SQL statement */
    retcode = sqlite3_exec (db, SQL_SCRIPT, NULL, NULL, &errmsg);
    if (SQLITE_OK != retcode)   /* handle sql errors */
    {
        log_sql_error (retcode, errmsg);
        sqlite3_free (errmsg); errmsg = NULL;
        return -1;
    }

    /* return successfully */
    return 0;
}


char *
db_escape_null (void)
{
    return string_clone ("NULL");
}


char *
db_escape_text (char *data)
{
    if (NULL == data) return db_escape_null ();

    char *escaped = string_replace (data, "'", "''");
    char *quoted  = string_quote (escaped, "'");

    free (escaped);
    return quoted;
}


char *
db_escape_boolean (bool data)
{
    return string_clone (((data) ? "TRUE" : "FALSE"));
}


char *
db_escape_integer (int data)
{
    return int_to_string (data);
}


int
db_get_column (sqlite3_stmt *stmt, int i, db_result_t *result_out)
{
    if ((NULL == stmt) || (result_out == NULL)) return -1;

    result_out->type = sqlite3_column_type (stmt, i);
    switch (result_out->type)
    {
    case SQLITE_INTEGER:
        result_out->i = sqlite3_column_int (stmt, i);
        break;
    case SQLITE_FLOAT:
        fprintf (stderr, "SQLITE_FLOAT Unimplemented\n");
        abort ();
        break;
    case SQLITE_TEXT:
        result_out->s = (char *)sqlite3_column_text (stmt, i);
        break;
    case SQLITE_BLOB:
        fprintf (stderr, "SQLITE_BLOB Unimplemented\n");
        abort ();
        break;
    case SQLITE_NULL:
        result_out->s = NULL;
        break;
    default:
        return -1;
    }

    return 0;
}

/* end of file */