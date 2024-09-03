
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>


static void print_hello (void);
static void db_test (const char *filename);
static sqlite3 *db_open (const char *filename);
static void db_close (sqlite3 *db);
static void _Noreturn db_abort (sqlite3 *db, int errorcode);


int
main (int argc, char **argv)
{
    db_test ("./test.db");
    print_hello ();
    return 0;
}


static void
print_hello (void)
{
    printf ("Hello World\n");
    return;
}


static sqlite3 *
db_open (const char *filename)
{
    int errorcode;
    sqlite3 *db = NULL;

    errorcode = sqlite3_open (filename, &db);
    if ((SQLITE_OK != errorcode) || (NULL == db)) db_abort (db, errorcode);

    return db;
}

static void
db_close (sqlite3 *db)
{
    if (NULL == db) return;

    while (SQLITE_OK != sqlite3_close (db)) {}
    return;
}


static void _Noreturn
db_abort (sqlite3 *db, int errorcode)
{
    const char *ERRMSG = sqlite3_errmsg (db);
    fprintf (stderr, "Error SQLite3: %d: %s\n", errorcode, ERRMSG);
    db_close (db); db = NULL;
    exit (EXIT_FAILURE);
}

static int
db_print_results (void *cbdata, int coln, char **results, char **columns)
{
    if ((NULL == results) || (NULL == columns))
    {
        printf ("No Results");
        return SQLITE_ABORT;
    }
    
    printf ("{ ");
    for (int i = 0; i < coln; i++)
    {
       printf ("\"%s\":\"%s\", ", columns[i], results[i]);
    }
    printf ("},\n");

    return SQLITE_OK;
}


static void
db_test (const char *filename)
{
    sqlite3 *db = NULL;
    int errorcode;
    const char *SQL_MAKE_TABLES = 
        "CREATE TABLE IF NOT EXISTS test_table ("
        "    test_id INTEGER PRIMARY KEY,"
        "    name    TEXT"
        ");";
    const char *SQL_INSERT_DATA = 
        "INSERT INTO test_table (test_id, name)"
        "VALUES (NULL, \"test data\");";
    const char *SQL_RETRIEVE_DATA = 
        "SELECT * FROM test_table;";

    db = db_open (filename);

    errorcode = sqlite3_exec (db, SQL_MAKE_TABLES, NULL, NULL, NULL);
    if (SQLITE_OK != errorcode) db_abort (db, errorcode);
    
    errorcode = sqlite3_exec (db, SQL_INSERT_DATA, NULL, NULL, NULL);
    if (SQLITE_OK != errorcode) db_abort (db, errorcode);
    
    errorcode = sqlite3_exec (db, SQL_RETRIEVE_DATA, db_print_results, NULL, NULL);
    if (SQLITE_OK != errorcode) db_abort (db, errorcode);

    db_close (db);
}