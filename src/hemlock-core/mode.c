/* HEMLOCK - a system independent package manager. */
/* <https://github.com/SoftFauna/HEMLOCK.git> */
/* Copyright (c) 2024 The SoftFauna Team */

#include "mode.h"

#include "arguement.h"
#include "insert.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


const enum
{
    INDEX_PROG_NAME,
    INDEX_MODE,
    INDEX_ARG_START,
};


static void log_hemlock_help (FILE *out);
static void log_hemlock_version (FILE *out);


void _Noreturn
mode_exec (int argc, char **argv)
{
    const enum
    {
        MODE_UPDATE = CONARG_ID_CUSTOM,
        MODE_INSERT,
        MODE_SEARCH,
        MODE_REMOVE,
        MODE_HELP,
        MODE_VERSION,
    };

    const conarg_t ARG_LIST[] =
    {
        { MODE_UPDATE,  NULL, "update",    CONARG_PARAM_REQUIRED },
        { MODE_INSERT,  NULL, "insert",    CONARG_PARAM_OPTIONAL },
        { MODE_SEARCH,  NULL, "search",    CONARG_PARAM_REQUIRED },
        { MODE_REMOVE,  NULL, "remove",    CONARG_PARAM_REQUIRED },
        { MODE_HELP,    "-h", "--help",    CONARG_PARAM_NONE },
        { MODE_VERSION, NULL, "--version", CONARG_PARAM_NONE },
    };
    const size_t ARG_COUNT = sizeof (ARG_LIST) / sizeof (*ARG_LIST);

    /* skip the executable name */
    char **arg_iter = argv + 1;
    int remaining = argc - 1;

    char *parameter = NULL;
    conarg_status_t param_stat = CONARG_STATUS_NA;
    int id = conarg_check (ARG_LIST, ARG_COUNT, arg_iter, remaining, 
                           &param_stat);

    switch (id)    
    {
    case MODE_UPDATE:   /* update mode, pass only args after mode */
        printf ("update mode unimplemented\n");
        break;

    case MODE_INSERT:   /* insert mode, pass only args after mode */
        if (CONARG_STATUS_VALID_PARAM == param_stat)
        {
            CONARG_STEP (arg_iter, remaining);
            parameter = conarg_get_param (arg_iter, remaining);
        }
        CONARG_STEP (arg_iter, remaining);
        insert_wrapper (parameter, remaining, arg_iter);
        break;

    case MODE_SEARCH:   /* search mode, pass only args after mode */
        printf ("search mode unimplemented\n");
        break;

    case MODE_REMOVE:   /* remove mode, pass only args after mode */
        printf ("remove mode unimplemented\n");
        break;

    case MODE_HELP:     /* hemlock help mode */
        log_hemlock_help (stdout);
        exit (EXIT_SUCCESS);
        break;

    case MODE_VERSION:  /* hemlock version mode */
        log_hemlock_version (stdout);
        exit (EXIT_SUCCESS);
        break;

    /* error states */
    case CONARG_ID_PARAM_ERROR:
        fprintf (stderr, "error: mode '%s' requires additional parameter\n", *arg_iter);
        log_hemlock_help (stderr);
        exit (EXIT_FAILURE);

    case CONARG_ID_UNKNOWN:
        fprintf (stderr, "error: unknown mode: '%s'\n", *arg_iter);
        log_hemlock_help (stderr);
        exit (EXIT_FAILURE);

    default:
        log_hemlock_help (stderr);
        exit (EXIT_FAILURE);
    }

    exit (EXIT_SUCCESS);
}


static void
log_hemlock_help (FILE *out)
{
    const char HELP_MESSAGE[] = 
    {
        "usage: hemlock [mode] [args...]\n" 
        "\n"
        "modes:\n"
        "  update <pkgname> [args...]   update an existing package\n"
        "  insert [pkgname] [args...]   insert a new package\n"
        "  remove <pkgname> [args...]   remove a package\n"
        "  search <query> [args...]     search for a specific package\n"
        "special modes:\n"
        "  -h, --help                   show this screen\n"
        "      --version                show copyright and version\n"
        "\n"
        "universal args:\n"
        "  -h, --help                   show help page for the given mode\n"
        "\n"
        "key:\n"
        "  <>   required input\n"
        "  []   optional input\n"
        "  ...  multiple inputs\n"
        "\n"
    };

    fprintf (out, "%s", HELP_MESSAGE);
    fflush (out);

    return;
}

static void
log_hemlock_version (FILE *out)
{
    const char VERSION_MESSAGE[] = 
    {
        "[version message template]"
    };

    fprintf (out, "%s\n", VERSION_MESSAGE);
    fflush (out);

    return;
}

/* end of file */