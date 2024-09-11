/* HEMLOCK - a system independent package manager. */
/* <https://github.com/SoftFauna/HEMLOCK.git> */
/* Copyright (c) 2024 The SoftFauna Team */

#include "settings.h"

#include "config.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>


static required_t settings_valid_fields (settings_t settings);
static void fprintbits (FILE *fp, size_t n, uintmax_t v);


settings_t 
settings_default (void)
{
    settings_t settings;

    settings.debug   = false;
    settings.verbose = false;

    settings.database = HEMLOCK_DATABASE_FILE;
    settings.dry_run  = false;

    settings.name         = NULL;
    settings.version      = NULL;
    settings.homepage     = NULL;
    settings.maintainer   = NULL;
    settings.email        = NULL;
    settings.require_list = NULL;
    settings.file_list    = NULL;

    settings.as_dependency = false;
    settings.is_installed  = true;    

    return settings;
}


static void
fprintbits (FILE *fp, size_t n, uintmax_t v)
{
    for (; n >= 0; n--) 
    {
        putchar ('0' + ((v >> n) & 1));
    }
}


void 
settings_print (FILE *fp, settings_t settings)
{    
    fprintf (fp, "debug:         %d\n", settings.debug);
    fprintf (fp, "verbose:       %d\n", settings.verbose);
    fprintf (fp, "database:      %s\n", settings.database);
    fprintf (fp, "dry_run:       %d\n", settings.dry_run);
    fprintf (fp, "name:          %s\n", settings.name);
    fprintf (fp, "version:       %s\n", settings.version);
    fprintf (fp, "homepage:      %s\n", settings.homepage);
    fprintf (fp, "maintainer:    %s\n", settings.maintainer);
    fprintf (fp, "email:         %s\n", settings.email);
    fprintf (fp, "require_list:  %s\n", settings.require_list);
    fprintf (fp, "file_list:     %s\n", settings.file_list);
    fprintf (fp, "as_dependency: %d\n", settings.as_dependency);
    fprintf (fp, "is_installed:  %d\n", settings.is_installed);
    fprintf (fp, "valid_fields:  ");
    fprintbits (fp, 8, settings_valid_fields (settings));
    fprintf (fp, "\n");
    fflush (fp);

    return;
}


static required_t
settings_valid_fields (settings_t settings)
{
    required_t list = REQUIRE_NONE;

    if (NULL != settings.database)     list |= REQUIRE_DATABASE;
    if (NULL != settings.name)         list |= REQUIRE_NAME;
    if (NULL != settings.version)      list |= REQUIRE_VERSION;
    if (NULL != settings.homepage)     list |= REQUIRE_HOMEPAGE;
    if (NULL != settings.maintainer)   list |= REQUIRE_MAINTAINER;
    if (NULL != settings.email)        list |= REQUIRE_EMAIL;
    if (NULL != settings.require_list) list |= REQUIRE_REQUIRE_LIST;
    if (NULL != settings.file_list)    list |= REQUIRE_FILE_LIST;
    
    return list;
}


required_t
settings_validate (settings_t settings, required_t require)
{
    required_t valid_fields = settings_valid_fields (settings);

    /* return the fields that did NOT meet the requirements */
    return ((valid_fields & require) ^ require);
}


void
settings_log_required (FILE *fp, required_t missing)
{
    fprintf (fp, "error: %u: missing required field(s): ", missing);
    
    if (0 != (missing & REQUIRE_DATABASE))     fprintf (fp, "DBFILE ");
    if (0 != (missing & REQUIRE_NAME))         fprintf (fp, "NAME ");
    if (0 != (missing & REQUIRE_VERSION))      fprintf (fp, "VERSION ");
    if (0 != (missing & REQUIRE_HOMEPAGE))     fprintf (fp, "HOMEPAGE ");
    if (0 != (missing & REQUIRE_MAINTAINER))   fprintf (fp, "MAINTAINER ");
    if (0 != (missing & REQUIRE_EMAIL))        fprintf (fp, "EMAIL ");
    if (0 != (missing & REQUIRE_REQUIRE_LIST)) fprintf (fp, "PACKAGE_LIST ");
    if (0 != (missing & REQUIRE_FILE_LIST))    fprintf (fp, "FILE_LIST ");

    fprintf (fp, "\n");
    fflush (fp);
}


/* end of file */