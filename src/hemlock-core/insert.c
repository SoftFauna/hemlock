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
    char *database;
    char *name;
    char *version;
    char *homepage;
    char *maintainer;
    char *email;
    char *require_list;
    char *file_list;
    bool dry_run;
    bool debug;
    bool verbose;
    bool as_dependency;
    bool is_installed;
} insert_settings_t;


static void log_insert_help (FILE *out);
static void log_settings (insert_settings_t settings);
static insert_settings_t proccess_arguements (int remaining, char **arg_iter);
static insert_settings_t get_default_settings (void);
static int get_sequence_arguements (insert_settings_t *settings, int remaining,
                                    char **arg_iter);
static int get_field_arguements (insert_settings_t *settings, int remaining,
                                 char **arg_iter);
static void validate_required_settings (insert_settings_t settings);
static void add_to_database (insert_settings_t settings);


void _Noreturn
insert_wrapper (int remaining, char **arg_iter)
{
    insert_settings_t settings;

    /* proccess console arguement */
    settings = proccess_arguements (remaining, arg_iter);

    /* log the settings to console */
    add_to_database (settings);

    /* exit */
    exit (EXIT_SUCCESS);
}


static insert_settings_t
proccess_arguements (int remaining, char **arg_iter)
{
    int n = 0;
    insert_settings_t settings = get_default_settings ();
    
    /* get [NAME [VERSION]] */
    n = get_sequence_arguements (&settings, remaining, arg_iter);
    CONARG_STEP_N_UNSAFE (arg_iter, remaining, n);

    /* get all other fields */
    (void)get_field_arguements (&settings, remaining, arg_iter);
    
    /* verify valid input */
    validate_required_settings (settings);
    
    return settings;
}


static insert_settings_t
get_default_settings (void)
{
    insert_settings_t settings;

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


static int
get_sequence_arguements (insert_settings_t *settings, int remaining, 
                         char **arg_iter)
{   
    int initial_count = remaining;
    char *name    = NULL;
    char *version = NULL;

    /* insert [NAME [VERSION]] */

    /* name (optional) */
    name = conarg_get_param (arg_iter, remaining);
    if ((NULL == name) || (conarg_is_flag (name))) 
    {
        name = NULL;
        goto sequence_exit;
    }
    CONARG_STEP (arg_iter, remaining);

    /* version (optional) */
    version = conarg_get_param (arg_iter, remaining);
    if ((NULL == version) || (conarg_is_flag (version))) 
    {
        version = NULL;
        goto sequence_exit;
    }
    CONARG_STEP (arg_iter, remaining);

sequence_exit:
    settings->name    = name;
    settings->version = version;

    return (initial_count - remaining);
}


static int
get_field_arguements (insert_settings_t *settings, int remaining, 
                     char **arg_iter)
{
    int initial_count = remaining;

    const enum 
    {
        INSERT_NAME = CONARG_ID_CUSTOM,
        INSERT_VERSION,
        INSERT_HOMEPAGE,
        INSERT_MAINTAINER,
        INSERT_EMAIL,
        INSERT_REQUIRE,
        INSERT_FILES,
        INSERT_DEPENDENCY,
        INSERT_STANDALONE,
        INSERT_INSTALLED,
        INSERT_UNINSTALLED,
        INSERT_DRY,
        INSERT_DATABASE,
        INSERT_DEBUG,
        INSERT_VERBOSE,
        INSERT_TERSE,
        INSERT_HELP,
    };

    const conarg_t ARG_LIST[] = 
    {
        { INSERT_NAME,        "-n", "--name",        CONARG_PARAM_REQUIRED },
        { INSERT_VERSION,     "-V", "--version",     CONARG_PARAM_REQUIRED },
        { INSERT_HOMEPAGE,    "-p", "--homepage",    CONARG_PARAM_REQUIRED },
        { INSERT_MAINTAINER,  "-m", "--maintainer",  CONARG_PARAM_REQUIRED },
        { INSERT_EMAIL,       "-e", "--email",       CONARG_PARAM_REQUIRED },
        { INSERT_REQUIRE,     "-r", "--require",     CONARG_PARAM_REQUIRED },
        { INSERT_FILES,       "-f", "--files",       CONARG_PARAM_REQUIRED },
        { INSERT_DEPENDENCY,  "-d", "--dependency",  CONARG_PARAM_NONE },
        { INSERT_STANDALONE,  "-D", "--standalone",  CONARG_PARAM_NONE },
        { INSERT_INSTALLED,   "-i", "--installed",   CONARG_PARAM_NONE },
        { INSERT_UNINSTALLED, "-I", "--uninstalled", CONARG_PARAM_NONE },

        { INSERT_DRY,      NULL, "--dryrun",   CONARG_PARAM_NONE },
        { INSERT_DATABASE, NULL, "--database", CONARG_PARAM_REQUIRED },

        { INSERT_DEBUG,   NULL, "--debug",   CONARG_PARAM_NONE },
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
            settings->name = conarg_get_param (arg_iter, remaining);
            break;

        case INSERT_VERSION:
            CONARG_STEP (arg_iter, remaining);
            settings->version = conarg_get_param (arg_iter, remaining);
            break;

        case INSERT_HOMEPAGE:
            CONARG_STEP (arg_iter, remaining);
            settings->homepage = conarg_get_param (arg_iter, remaining);
            break;

        case INSERT_MAINTAINER:
            CONARG_STEP (arg_iter, remaining);
            settings->maintainer = conarg_get_param (arg_iter, remaining);
            break;

        case INSERT_EMAIL:
            CONARG_STEP (arg_iter, remaining);
            settings->email = conarg_get_param (arg_iter, remaining);
            break;

        case INSERT_FILES:
            CONARG_STEP (arg_iter, remaining);
            settings->file_list = conarg_get_param (arg_iter, remaining);
            break;

        case INSERT_REQUIRE:
            CONARG_STEP (arg_iter, remaining);
            settings->require_list = conarg_get_param (arg_iter, remaining);
            break;

        case INSERT_DEPENDENCY:
            settings->as_dependency = true;
            break;

        case INSERT_STANDALONE:
            settings->as_dependency = false;
            break;

        case INSERT_INSTALLED:
            settings->is_installed = true;
            break;

        case INSERT_UNINSTALLED:
            settings->is_installed = false;
            break;

        case INSERT_DATABASE:
            CONARG_STEP (arg_iter, remaining);
            settings->database = conarg_get_param (arg_iter, remaining);
            break;

        case INSERT_DRY:
            settings->dry_run = true;
            break;

        case INSERT_DEBUG:
            settings->debug   = true;
            /* fall through, 
             * enable all verbose flags too */
        case INSERT_VERBOSE:
            settings->verbose = true;
            break;

        case INSERT_TERSE:
            settings->verbose = false;
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

    return (initial_count - remaining);
}


static void
validate_required_settings (insert_settings_t settings)
{    
    if (NULL == settings.database)
    {
        fprintf (stderr, "error: database cannot be NULL\n");
        log_insert_help (stderr);
        exit (EXIT_FAILURE);
    }

    if ((NULL == settings.name) ||
        (NULL == settings.version))
    {
        fprintf (stderr, "error: name and version fields are required.\n");
        log_insert_help (stderr);
        exit (EXIT_FAILURE);
    }

    return;
}


static void
add_to_database (insert_settings_t settings)
{
    int retcode = 0;
    sqlite3 *db = NULL;
    db_package_t package;
    db_package_t *package_list = NULL;
    size_t list_count = 0;

    package.package_id = 0;
    package.name       = settings.name;
    package.version    = settings.version;
    package.homepage   = settings.homepage;
    package.maintainer = settings.maintainer;
    package.email      = settings.email;
    package.as_dependency = settings.as_dependency;
    package.is_installed  = settings.is_installed;


    db = db_open (settings.database);
    if (NULL == db)
    {
        fprintf (stderr, "error: cannot open database at '%s'\n", 
                 settings.database);
    }

    retcode = db_create_tables (db, NULL);
    if (0 != retcode)
    {
        fprintf (stderr, "error: cannot create database tables\n");
        goto add_to_db_exit;
    }
    
    package_list = db_search_packages (db, package.name, package.version,
                                       &list_count, NULL); 
    if (0 != list_count)
    {
        fprintf (stderr, "error: package exists\n");
        goto add_to_db_exit;
    }

    if (settings.dry_run)
    {
        fprintf (stderr, "dry run detected\n"); 
    }
    else
    {
        retcode = db_insert_package (db, &package, NULL);
        if (0 != retcode)
        {
            fprintf (stderr, "error: cannot insert package\n");
            goto add_to_db_exit;
        }
    }

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
log_insert_help (FILE *out)
{
    const char *HELP_MESSAGE = {
        "Usage: " PROJECT_NAME " insert [NAME [VERSION]] [OPTION]...\n"
        "Insert a package into the package database.\n"
        "Egless otherwise specified assume -i -D -t flags,\n"
        "\n"
        "The insert will fail if the package is not given a NAME and a VERSION; either\n"
        "may be included as 1st+2nd arguements or through their respective flags\n"
        "\n"
        "Mandatory arguements to long options are mandatory for short options too.\n"
        "  -n, --name NAME             define the NAME for the package\n"
        "  -V, --version VERSION       define the VERSION\n"
        "  -p, --homepage HOMEPAGE     define the HOMEPAGE (optional)\n"
        "  -m, --maintainer MAINTAINER define the MAINTAINER (optional)\n"
        "  -e, --email EMAIL           define the EMAIL (optional)\n"
        "  -r, --require PACKAGE_LIST  define a PACKAGE_LIST containing the package\n"
        "                                dependencies for the program\n"
        "  -F, --files FILE_LIST       define a FILE_LIST containing the program's\n"
        "                                files\n" 
        "  -d, --dependency            mark the package as nothing but a dependency\n"
        "                                for another package\n"
        "  -D, --standalone            mark the package as it's own program\n" 
        "  -i, --installed             mark the package as installed\n"
        "  -I, --uninstalled           mark the package as not yet installed\n"
        "      --database DBFILE       override the package database file, use DBFILE\n"
        "      --dryrun                preform a dry-run. dont preform any writes\n"
        "      --debug                 log all (often unnecessary) information\n"
        "  -v, --verbose               log extra information\n"
        "  -t, --terse                 only log errors\n"
        "  -h, --help                  show this message\n"
        "\n"
        "The NAME and VERSION arguements are required for a package insert to complete\n"
        "successfully.\n"
        "\n"
        "The PACKAGE_LIST arguement expects a comma seperated list of package name's\n"
        "to list the dependencies of the given program. All such dependencies MUST\n"
        "exist within the database prior to listing as a dependency.\n"
        "\n"
        "The FILE_LIST arguement is a comma seperated list of files related to the\n"
        "package. The provided files are not required to exist in the current\n"
        "file-system; however, it is iladvisable to not install such files.\n"
        "\n"
        "The DBFILE arguement is expected to be a SQLite3 database, and is expected to\n"
        "exist, if it does not, it will be created.\n"
        "\n"
        "Exit status:\n"
        " 0  if OK,\n"
        " 1  if error.\n"
        "\n"
        "SoftFauna hemlock: <https://github.com/SoftFauna/hemlock/>\n"
        "\n"
    };

    fprintf (out, HELP_MESSAGE);
    fflush (out);
}


/* end of file */