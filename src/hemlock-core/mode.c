/* HEMLOCK - a system independent package manager. */
/* <https://github.com/SoftFauna/HEMLOCK.git> */
/* Copyright (c) 2024 The SoftFauna Team */

#include "mode.h"

#include "arguement.h"
#include "config.h"
#include "insert.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static void log_hemlock_help (FILE *out);
static void log_hemlock_version (FILE *out);


void _Noreturn
mode_exec (int remaining, char **arg_iter)
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
        { MODE_UPDATE,  NULL, "update",    CONARG_PARAM_NONE },
        { MODE_INSERT,  NULL, "insert",    CONARG_PARAM_NONE },
        { MODE_SEARCH,  NULL, "search",    CONARG_PARAM_NONE },
        { MODE_REMOVE,  NULL, "remove",    CONARG_PARAM_NONE },
        { MODE_HELP,    "-h", "--help",    CONARG_PARAM_NONE },
        { MODE_VERSION, NULL, "--version", CONARG_PARAM_NONE },
    };
    const size_t ARG_COUNT = sizeof (ARG_LIST) / sizeof (*ARG_LIST);

    /* skip the executable name */
    CONARG_STEP (arg_iter, remaining);

    /* get the mode */
    conarg_status_t param_stat = CONARG_STATUS_NA;
    int id = conarg_check (ARG_LIST, ARG_COUNT, arg_iter, remaining, 
                           &param_stat);

    switch (id)    
    {
    case MODE_UPDATE:   /* update mode, pass only args after mode */
        printf ("update mode unimplemented\n");
        break;

    case MODE_INSERT:   /* insert mode, pass only args after mode */
        CONARG_STEP (arg_iter, remaining);
        insert_wrapper (remaining, arg_iter);
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
    const char *HELP_MESSAGE = 
    {
        "Usage: " PROJECT_NAME " MODE [OPTION]...\n"
        "Interact with the HEMLOCK package database through CLI.\n"
        "\n"
        "Mandatory arguements to long options are mandatory for short options too.\n"
        "modes:\n"
        "  update NAME [VERSION]       make updates to an existing package entry\n"
        "  insert NAME [VERSION]       create a new package entry\n"
        "  remove NAME [VERSION]       remove a package entry\n"
        "  search QUERY                search for a package entry\n"
        "special modes:\n"
        "  -h, --help                  show this message\n"
        "      --version               show extra information about the program\n"
        "\n"
        "The QUERY arguement is a SQL 'like' search query, as such, \"%\" may be used\n"
        "as a SQL equivelant of pascal regex's \".*?\" non-greedy match.\n"
        "\n"
        "Exit status:\n"
        " 0  if OK,\n"
        " 1  if error.\n"
        "\n"
        "SoftFauna hemlock: <https://github.com/SoftFauna/hemlock/>\n"
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
        PROJECT_NAME " (SoftFauna " CMAKE_PROJECT_NAME ")" PROJECT_VERSION "\n"
        "Copyright " COPYRIGHT_YEAR " The SoftFauna Team\n"
        "License MIT: The MIT license, <https://spdx.org/licenses/MIT.html>\n"
        "This is free software: you are free to change and redistribute it.\n"
        "There is NO WARRANTY, to the extent permitted by law.\n"
    };

    fprintf (out, "%s\n", VERSION_MESSAGE);
    fflush (out);

    return;
}

/* end of file */