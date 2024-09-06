/* HEMLOCK - a system independent package manager. */
/* <https://github.com/SoftFauna/HEMLOCK.git> */
/* Copyright (c) 2024 The SoftFauna Team */

#include "config.h"
#include "database.h"
#include <stdio.h>
#include <stdlib.h>


static void test_package_gen (void);


int
main (int argc, char **argv)
{
    test_package_gen ();
    return 0;
}


static void
test_package_gen (void)
{
    db_package_t *out = NULL;
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
    if (0 != db_update_package (db, &self, NULL))
    {
        fprintf (stderr, "Failed to update package\n");
    }

    out = db_search_package_id (db, self.package_id, NULL);
    if (NULL == out)
    {
        fprintf (stderr, "Failed to find package\n");
        goto test_exit;
    }
    out_string = db_human_readable_package (out);
    printf ("before: { %s }\n", out_string);
    free (out_string);     out_string = NULL;
    db_free_package (out); out = NULL;

    self.maintainer = "updated mantainer";
    if (0 != db_update_package (db, &self, NULL))
    {
        fprintf (stderr, "Failed to update package\n");
    }

    out = db_search_package_id (db, self.package_id, NULL);
    if (NULL == out)
    {
        fprintf (stderr, "Failed to find package\n");
        goto test_exit;
    }
    out_string = db_human_readable_package (out);
    printf ("after: { %s }\n", out_string);
    free (out_string);     out_string = NULL;
    db_free_package (out); out = NULL;

test_exit:
    db_free_package (out); out = NULL;
    free (out_string); out_string = NULL;
    db_close (db); db = NULL;

    return;
}


/* end of file */