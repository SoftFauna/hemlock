/* HEMLOCK - a system independent package manager. */
/* <https://github.com/SoftFauna/HEMLOCK.git> */
/* Copyright (c) 2024 The SoftFauna Team */

#include "mode.h"

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


mode_t
mode_select (int argc, char **argv)
{
    char *mode_iter = NULL;

    if (NULL == argv) return MODE_NONE;

    if (argc < (INDEX_MODE + 1)) return MODE_NONE;

    mode_iter = argv[INDEX_MODE];
    if (NULL == mode_iter) return MODE_NONE;

    if (0 == strcmp (mode_iter, "update"))
    {
        return MODE_UPDATE;
    }
    else if (0 == strcmp (mode_iter, "insert"))
    {
        return MODE_INSERT;
    }
    else if (0 == strcmp (mode_iter, "remove"))
    {
        return MODE_REMOVE;
    }
    else if (0 == strcmp (mode_iter, "search"))
    {
        return MODE_SEARCH;
    }
    else if ((0 == strcmp (mode_iter, "--help")) ||
             (0 == strcmp (mode_iter, "-h")))
    {
        return MODE_HELP;
    }
    else if (0 == strcmp (mode_iter, "--version"))
    {
        return MODE_VERSION;
    }

    return MODE_UNRECOGNIZED;
}


void _Noreturn
mode_execute (mode_t mode, int argc, char **argv)
{
    switch (mode)
    {
    case MODE_INSERT:
        printf ("insert mode unimplemented\n");
        fflush (stdout);
        break;

    case MODE_UPDATE:
        printf ("insert mode unimplemented\n");
        fflush (stdout);
        break;

    case MODE_SEARCH:
        printf ("insert mode unimplemented\n");
        fflush (stdout);
        break;

    case MODE_VERSION:
        log_hemlock_version (stdout);
        exit (EXIT_SUCCESS);
        break;

    case MODE_HELP:
        log_hemlock_help (stdout);
        exit (EXIT_SUCCESS);
        break;

    case MODE_UNRECOGNIZED:
        fprintf (stderr, "Error: Unrecognized mode '%s'\n", argv[INDEX_MODE]);  
        log_hemlock_help (stderr);
        exit (EXIT_FAILURE);
        break;

    case MODE_NONE:
        fprintf (stderr, "Error: No mode specified\n");
        log_hemlock_help (stderr);
        exit (EXIT_FAILURE);
        break;

    case MODE_ERROR:
    default:
        log_hemlock_help (stderr);
        exit (EXIT_FAILURE);
        break;
    }    

    exit (EXIT_FAILURE);
}

static void
log_hemlock_help (FILE *out)
{
    const char HELP_MESSAGE[] = 
    {
        "usage: hemlock [mode] [args...]\n" 
        "\n"
        "modes:\n"
        "  update [args...]     update an existing package\n"
        "  insert [args...]     insert a new package\n"
        "  remove [args...]     remove a package\n"
        "  search [args...]     search for a specific package\n"
        "special modes:\n"
        "  -h, --help           show this screen\n"
        "      --version        show copyright and version\n"
        "\n"
        "universal args:\n"
        "  -h, --help           show help page for the given mode\n"
    };

    fprintf (out, "%s\n", HELP_MESSAGE);
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