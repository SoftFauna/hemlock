/* HEMLOCK - a system independent package manager. */
/* <https://github.com/SoftFauna/HEMLOCK.git> */
/* Copyright (c) 2024 The SoftFauna Team */

#include "insert.h"

#include "arguement.h"
#include "config.h"
#include "database.h"
#include "database_core.h"
#include "mode_template.h"
#include "settings.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


static int get_sequenced_args (settings_t *settings, int argc, char **argv);
static int get_field_args (settings_t *settings, int argc, char **argv);
static void log_insert_help (FILE *fp);
static void add_to_database (settings_t settings);


void _Noreturn
insert_wrapper (int argc, char **argv)
{
    const required_t required = REQUIRE_NAME | REQUIRE_VERSION;
    
    settings_t settings = mode_template_proccess_args (argc, argv, required, 
            get_sequenced_args, get_field_args, log_insert_help);

    add_to_database (settings);

    exit (EXIT_SUCCESS);
}


static void
add_to_database (settings_t settings)
{
    int retcode = 0;
    sqlite3 *db = NULL;
    db_package_t package;
    db_package_t *package_list = NULL;
    size_t list_count = 0;

    package.package_id    = 0;
    package.name          = settings.name;
    package.version       = settings.version;
    package.homepage      = settings.homepage;
    package.maintainer    = settings.maintainer;
    package.email         = settings.email;
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


static int
get_sequenced_args (settings_t *settings, int argc, char **argv)
{   
    int initial_count = argc;
    char *name    = NULL;
    char *version = NULL;

    /* insert [NAME [VERSION]] */

    /* name (optional) */
    name = conarg_get_param (argc, argv);
    if ((NULL == name) || (conarg_is_flag (name))) 
    {
        name = NULL;
        goto sequence_exit;
    }
    CONARG_STEP (argc, argv);

    /* version (optional) */
    version = conarg_get_param (argc, argv);
    if ((NULL == version) || (conarg_is_flag (version))) 
    {
        version = NULL;
        goto sequence_exit;
    }
    CONARG_STEP (argc, argv);

sequence_exit:
    settings->name    = name;
    settings->version = version;

    return (initial_count - argc);
}


static int
get_field_args (settings_t *settings, int argc, char **argv)
{
    int initial_count = argc;

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

    while (argc > 0)
    {
        param_stat = CONARG_STATUS_NA;
        id = conarg_check (ARG_LIST, ARG_COUNT, argc, argv, &param_stat);

        switch (id)
        {
        case INSERT_NAME:
            CONARG_STEP (argc, argv);
            settings->name = conarg_get_param (argc, argv);
            break;

        case INSERT_VERSION:
            CONARG_STEP (argc, argv);
            settings->version = conarg_get_param (argc, argv);
            break;

        case INSERT_HOMEPAGE:
            CONARG_STEP (argc, argv);
            settings->homepage = conarg_get_param (argc, argv);
            break;

        case INSERT_MAINTAINER:
            CONARG_STEP (argc, argv);
            settings->maintainer = conarg_get_param (argc, argv);
            break;

        case INSERT_EMAIL:
            CONARG_STEP (argc, argv);
            settings->email = conarg_get_param (argc, argv);
            break;

        case INSERT_FILES:
            CONARG_STEP (argc, argv);
            settings->file_list = conarg_get_param (argc, argv);
            break;

        case INSERT_REQUIRE:
            CONARG_STEP (argc, argv);
            settings->require_list = conarg_get_param (argc, argv);
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
            CONARG_STEP (argc, argv);
            settings->database = conarg_get_param (argc, argv);
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

        CONARG_STEP (argc, argv);
    }

    return (initial_count - argc);
}


static void
log_insert_help (FILE *fp)
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

    fprintf (fp, HELP_MESSAGE);
    fflush (fp);
}


/* end of file */