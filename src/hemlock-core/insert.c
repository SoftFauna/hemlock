/* HEMLOCK - a system independent package manager. */
/* <https://github.com/SoftFauna/HEMLOCK.git> */
/* Copyright (c) 2024 The SoftFauna Team */

#include "insert.h"

#include "arguement.h"
#include "config.h"
#include "database.h"
#include "database_core.h"
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    char *name;
    char *version;
    char *homepage;
    char *maintainer;
    char *email;
    bool verbose;
    bool as_dependency;
    bool is_installed;
} insert_settings_t;



static void log_insert_help (FILE *log);
static insert_settings_t proccess_arguements (int arg_count, char **arg_list);
static insert_settings_t get_default_settings (void);
static bool validate_required_settings (insert_settings_t settings);
static void log_settings (insert_settings_t settings);
static void add_to_database (insert_settings_t settings);


static insert_settings_t
get_default_settings (void)
{
    insert_settings_t settings;

    settings.verbose = false;

    settings.name       = NULL;
    settings.version    = NULL;
    settings.homepage   = NULL;
    settings.maintainer = NULL;
    settings.email      = NULL;
    
    settings.as_dependency = false;
    settings.is_installed  = true;    

    return settings;
}


static bool
validate_required_settings (insert_settings_t settings)
{
    if ((NULL == settings.name) ||
        (NULL == settings.version))
    {
        return false;
    }

    return true;
}


static insert_settings_t
proccess_arguements (int arg_count, char **arg_list)
{
    insert_settings_t settings = get_default_settings ();

    char **arg_iter = arg_list;
    int remaining   = arg_count;

    const enum 
    {
        INSERT_NAME = CONARG_ID_CUSTOM,
        INSERT_VERSION,
        INSERT_HOMEPAGE,
        INSERT_MAINTAINER,
        INSERT_EMAIL,
        INSERT_DEPENDENCY,
        INSERT_STANDALONE,
        INSERT_INSTALLED,
        INSERT_UNINSTALLED,
        INSERT_VERBOSE,
        INSERT_TERSE,
        INSERT_HELP,
    };

    const conarg_t ARG_LIST[] = 
    {
        { INSERT_NAME,       "-n", "--name",       CONARG_PARAM_REQUIRED },
        { INSERT_VERSION,    "-V", "--version",    CONARG_PARAM_REQUIRED },
        { INSERT_HOMEPAGE,   "-p", "--homepage",   CONARG_PARAM_REQUIRED },
        { INSERT_MAINTAINER, "-m", "--maintainer", CONARG_PARAM_REQUIRED },
        { INSERT_EMAIL,      "-e", "--email",      CONARG_PARAM_REQUIRED },

        { INSERT_DEPENDENCY, "-d", "--dependency", CONARG_PARAM_NONE },
        { INSERT_STANDALONE, "-D", "--standalone", CONARG_PARAM_NONE },

        { INSERT_INSTALLED,   "-i", "--installed",   CONARG_PARAM_NONE },
        { INSERT_UNINSTALLED, "-I", "--uninstalled", CONARG_PARAM_NONE },

        { INSERT_VERBOSE, "-v", "--verbose", CONARG_PARAM_NONE },
        { INSERT_TERSE,   "-t", "--terse",   CONARG_PARAM_NONE },
        { INSERT_HELP,    "-h", "--help",    CONARG_PARAM_NONE },
    };
    const size_t ARG_COUNT = sizeof (ARG_LIST) / sizeof (*ARG_LIST);
    int id;
    conarg_status_t param_stat;

    while (remaining > 0)
    {
        param_stat = CONARG_STATUS_NA;
        id = conarg_check (ARG_LIST, ARG_COUNT, arg_iter, remaining, 
                           &param_stat);
        
        switch (id)
        {
        case INSERT_NAME:
            CONARG_STEP (arg_iter, remaining);
            settings.name = conarg_get_param (arg_iter, remaining);
            break;

        case INSERT_VERSION:
            CONARG_STEP (arg_iter, remaining);
            settings.version = conarg_get_param (arg_iter, remaining);
            break;

        case INSERT_HOMEPAGE:
            CONARG_STEP (arg_iter, remaining);
            settings.homepage = conarg_get_param (arg_iter, remaining);
            break;

        case INSERT_MAINTAINER:
            CONARG_STEP (arg_iter, remaining);
            settings.maintainer = conarg_get_param (arg_iter, remaining);
            break;

        case INSERT_EMAIL:
            CONARG_STEP (arg_iter, remaining);
            settings.email = conarg_get_param (arg_iter, remaining);
            break;

        case INSERT_DEPENDENCY:
            settings.as_dependency = true;
            break;

        case INSERT_STANDALONE:
            settings.as_dependency = false;
            break;

        case INSERT_INSTALLED:
            settings.is_installed = true;
            break;

        case INSERT_UNINSTALLED:
            settings.is_installed = false;
            break;

        case INSERT_VERBOSE:
            settings.verbose = true;
            break;

        case INSERT_TERSE:
            settings.verbose = false;
            break;

        case INSERT_HELP:
            log_insert_help (stdout);
            exit (EXIT_SUCCESS);

        /* error states */
        case CONARG_ID_UNKNOWN:
        case CONARG_ID_PARAM_ERROR:
        default:
            log_insert_help (stderr);
            exit (EXIT_FAILURE);
        }

        CONARG_STEP (arg_iter, remaining);
    }

    return settings;
}


static void 
log_settings (insert_settings_t settings)
{
    const char *FORMAT = 
    {
        "verbose: %d\n"
        "name: %s\n"
        "version: %s\n"
        "homepage: %s\n"
        "maintainer: %s\n"
        "email: %s\n"
        "is_installed: %d\n"
        "as_dependency: %d\n"
    };
    fprintf (stderr, FORMAT, settings.verbose, settings.name, 
             settings.version, settings.homepage, settings.maintainer,
             settings.email, settings.is_installed, settings.as_dependency);
    fflush (stderr);
    
}


static void
add_to_database (insert_settings_t settings)
{
    sqlite3 *db = NULL;
    db_package_t package;
    db_package_t *package_list = NULL;
    size_t list_count = 0;

    package.package_id = 0;
    package.name = settings.name;
    package.version = settings.version;
    package.homepage = settings.homepage;
    package.maintainer = settings.maintainer;
    package.email = settings.email;
    package.as_dependency = settings.as_dependency;
    package.is_installed = settings.is_installed;

    db = db_open (HEMLOCK_DATABASE_FILE);
    (void)db_create_tables (db, NULL);
    package_list = db_search_package (db, package.name, package.version,
                                      &list_count, NULL); 
    if (0 != list_count)
    {
        fprintf (stderr, "error: package exists\n");
        goto add_to_db_exit;
    }

    (void)db_insert_package (db, &package, NULL);

add_to_db_exit:
    for (size_t i = 0; i < list_count; i++)
    {
        db_free_package (package_list + i);
    }
    list_count = 0;
    free (package_list); package_list = NULL;
    db_close (db); db = NULL;

    return;
}


void _Noreturn
insert_wrapper (char *opt_pkgname, int arg_count, char **arg_list)
{
    insert_settings_t settings;

    /* proccess console arguement */
    settings = proccess_arguements (arg_count, arg_list);
    if ((NULL == settings.name) && (NULL != opt_pkgname)) 
    {
        settings.name = opt_pkgname;
    }
    
    /* validate settings */
    if (!validate_required_settings (settings))
    {
        fprintf (stderr, "error: <name> and <version> are required parameters\n");
        log_insert_help (stderr);
        exit (EXIT_FAILURE);
    }

    /* log the settings to console */
    log_settings (settings);
    add_to_database (settings);

    /* exit */
    exit (EXIT_SUCCESS);
}


static void
log_insert_help (FILE *log)
{
    const char *HELP_MESSAGE = {
        "Usage: hemlock-core insert ([--name] <pkgname>) (--version <version>) [arguements...]\n"
        "or     hemlock-core insert --help\n"
        "\n"
        "note: requirements for long flags are required for short flags\n" 
        "aswell\n"
        "arguements:\n"
        "  -n, --name <pkgname>       set the package's name\n"
        "  -V, --version <version>    set the package's version\n"
        "  -p, --homepage <url>       set the package's homepage\n"
        "  -m, --maintainer <name>    set the package maintainers name\n"
        "  -e, --email <email>        set the package maintainers email\n"
        "\n"
        "  -d, --dependency           mark the package as dependency\n"
        "  -D, --standalone           mark the package as standalone\n" 
        "                             package; not as a dependency)\n"
        "\n"
        "  -i, --installed            mark the package as installed\n"
        "  -I, --uninstalled          mark the package as non-installed\n"
        "\n"
        "  -v, --verbose              log extra information\n"
        "  -t, --terse                only log errors\n"
        "\n"
        "  -h, --help                 show this message\n"
        "\n"
        "key where:\n"
        "  ()   represents a required field\n"
        "  []   represents an optional field\n"
        "  <>   represents a substitutable peice of data\n"
        "  ...  represents a variables number of inputs\n"
        "\n"
    };

    fprintf (log, HELP_MESSAGE);
    fflush (log);
}

/* end of file */