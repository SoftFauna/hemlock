/* HEMLOCK - a system independent package manager. */
/* <https://github.com/SoftFauna/HEMLOCK.git> */
/* Copyright (c) 2024 The SoftFauna Team */

#include "settings.h"

#include <stdbool.h>


static bool validate (require_list_t require, require_key_t key, void *ptr);


settings_t 
settings_default (void);
{

}


void 
settings_print (settings_t settings);
{    
    const char *FORMAT = 
    {
    printf ("debug: %d\n", settings.debug);
    printf ("verbose: %d\n", settings.verbose);
    printf ("database: %s\n", settings.database);
    printf ("dry_run: %d\n", settings.dry_run);
    printf ("name: %d\n", settings.debug);
    printf ("version: %d\n", settings.debug);
    printf ("homepage: %d\n", settings.debug);
    printf ("debug: %d\n", settings.debug);
    printf ("debug: %d\n", settings.debug);
    printf ("debug: %d\n", settings.debug);
    printf ("debug: %d\n", settings.debug);
    printf ("debug: %d\n", settings.debug);
    printf ("debug: %d\n", settings.debug);
        "verbose: %d\n"
        "database: %s\n"
        "dry_run: %d\n"
        "name: %s\n"
        "version: %s\n"
        "homepage: %s\n"
        "maintainer: %s\n"
        "email: %s\n"
        "is_installed: %d\n"
        "as_dependency: %d\n"
    };

}


static bool
validate (require_list_t require, require_key_t key, void *ptr)
{
    /* bool not_required = (0 == (require & key));
     * bool valid_ptr    = (NULL != ptr) 
     *
     * if (not_required) return true;
     * 
     * return valid_ptr;
     * */

    return ((0 == (require & key)) || (NULL != ptr));
}


bool 
settings_validate (settings_t self, require_list_t require)
{
    if (REQUIRE_NONE == require) return true;

    return ((validate (require, REQUIRE_DATABASE,     self.database))
         && (validate (require, REQUIRE_NAME,         self.name))
         && (validate (require, REQUIRE_VERSION,      self.version))
         && (validate (require, REQUIRE_HOMEPAGE,     self.homepage))
         && (validate (require, REQUIRE_MAINTAINER,   self.maintainer))
         && (validate (require, REQUIRE_EMAIL,        self.email))
         && (validate (require, REQUIRE_REQUIRE_LIST, self.require_list))
         && (validate (require, REQUIRE_FILE_LIST,    self.file_list)));
}


/* end of file */