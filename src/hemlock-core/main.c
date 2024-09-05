/* HEMLOCK - a system independent package manager. */
/* <https://github.com/SoftFauna/HEMLOCK.git> */
/* Copyright (c) 2024 The SoftFauna Team */

#include "config.h"
#include "database.h"
#include <stdio.h>

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
    db_package_t self;
    self.name = "feh";
    self.version = "3.61.2";
    self.as_dependency = false;
    self.is_installed  = false;
    self.email = NULL;
    self.maintainer = "testing this ain't work'in quite' right";
    self.homepage = NULL;
    self.package_id = -43221289;    /* unecessary */

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
    if (0 != db_update_package (db, &self, stdout))
    {
        fprintf (stderr, "Failed to update package\n");
    }

test_exit:
    db_close (db);

    return;
}


/* end of file */