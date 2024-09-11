/* HEMLOCK - a system independent package manager. */
/* <https://github.com/SoftFauna/HEMLOCK.git> */
/* Copyright (c) 2024 The SoftFauna Team */

#include "remove.h"

#include "arguement.h"
#include "config.h"
#include "mode_template.h"
#include "settings.h"
#include <stdio.h>
#include <stdlib.h>


static int get_sequenced_args (settings_t *settings, int argc, char **argv);
static int get_field_args (settings_t *settings, int argc, char **argv);
static void log_remove_help (FILE *fp);
static void remove_package (settings_t settings);


void _Noreturn
remove_wrapper (int argc, char **argv)
{
    const required_t required = REQUIRE_NAME | REQUIRE_VERSION;
    
    settings_t settings = mode_template_proccess_args (argc, argv, required, 
            get_sequenced_args, get_field_args, log_remove_help);

    remove_package (settings);

    exit (EXIT_SUCCESS);
}


static void
remove_package (settings_t settings)
{
    printf ("removeing package\n");
    settings_print (stdout, settings);
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
        INSERT_DRY = CONARG_ID_CUSTOM,
        INSERT_DATABASE,
        INSERT_DEBUG,
        INSERT_VERBOSE,
        INSERT_TERSE,
        INSERT_HELP,
    };

    const conarg_t ARG_LIST[] = 
    {
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
            log_remove_help (stdout);
            exit (EXIT_SUCCESS);

        /* error states */
        case CONARG_ID_UNKNOWN:
        case CONARG_ID_PARAM_ERROR:
        default:
            log_remove_help (stderr);
            exit (EXIT_FAILURE);
        }

        CONARG_STEP (argc, argv);
    }

    return (initial_count - argc);
}


static void
log_remove_help (FILE *fp)
{
    const char *HELP_MESSAGE = {
        "Usage: " PROJECT_NAME " remove NAME VERSION [OPTION]...\n"
        "Remove a package from the package database.\n"
        "Egless otherwise specified assume -t flag,\n"
        "\n"
        "Mandatory arguements to long options are mandatory for short options too.\n"
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