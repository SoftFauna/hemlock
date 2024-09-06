/* HEMLOCK - a system independent package manager. */
/* <https://github.com/SoftFauna/HEMLOCK.git> */
/* Copyright (c) 2024 The SoftFauna Team */

#include "config.h"
#include "database.h"
#include <errno.h>
#include "mode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static void test_package_gen (void);


int
main (int argc, char **argv)
{
    mode_t mode;

#ifdef _DEBUG   /* hard code debug mode */
    mode = MODE_VERSION;
#else
    mode = mode_select (argc, argv);
#endif

    mode_execute (mode, argc, argv);
    return 0;
}



static void
test_package_gen (void)
{
    db_package_t *out = NULL, *out_arr = NULL;
    size_t out_n = 0;
    char *out_string = NULL;
    

    db_package_t self;
    self.name = "feh";
    self.version = "3.61.2";
    self.as_dependency = false;
    self.is_installed  = false;
    self.email = NULL;
    self.maintainer = NULL;
    self.homepage = NULL;
    self.package_id = 3;

    sqlite3 *db = db_open ("./test.db");
    if (NULL == db)
    {
        fprintf (stderr, "Failed to open database\n");
        return;
    }
    if (0 != db_create_tables (db, NULL))
    {
        fprintf (stderr, "Failed to create tables\n");
        goto test_exit;
    }

    self.maintainer = "initial mantainer";
    if (0 != db_insert_package (db, &self, NULL))
    {
        fprintf (stderr, "Failed to insert package\n");
    }

    self.maintainer = "updated mantainer";
    if (0 != db_insert_package (db, &self, NULL))
    {
        fprintf (stderr, "Failed to insert package\n");
    }

    out_arr = db_search_packages (db, self.name, self.version, &out_n, NULL);
    if ((NULL == out_arr) || (0 == out_n))
    {
        fprintf (stderr, "Failed to find a match\n");
        goto test_exit;
    }

    for (size_t i = 0; i < out_n; i++)
    {
        out_string = db_human_readable_package (out_arr + i);
        printf ("out_arr[%d] = %s\n", i, out_string);
        free (out_string); out_string = NULL;
    }

test_exit:
    for (size_t i = 0; i < out_n; i++) db_free_package (out_arr + i);
    free (out_arr); out_arr = NULL;
    db_free_package (out); free (out); out = NULL;
    free (out_string); out_string = NULL;
    db_close (db); db = NULL;

    return;
}


/* end of file */