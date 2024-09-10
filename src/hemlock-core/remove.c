/* HEMLOCK - a system independent package manager. */
/* <https://github.com/SoftFauna/HEMLOCK.git> */
/* Copyright (c) 2024 The SoftFauna Team */

#include "remove.h"

#include "arguement.h"
#include "config.h"
#include "database.h"
#include "database_core.h"


typedef struct
{
    char *database;
    char *name;
    char *version;
    bool dry_run;
    bool debug;
    bool verbose;
} remove_settings_t;


static void log_remove_help (FILE *out);
static void log_remove_settings (remove_settings_t settings);
static remove_settings_t proccess_arguements (int remaining, char **arg_iter);
static remove_settings_t get_default_settings (void);
static int get_sequence_arguements (remove_settings_t *settings, int remaining,
                                    char **arg_iter);
static int get_field_arguements (remove_settings_t *settings, int remaining,
                                 char **arg_iter);
static void validate_required_settings (remove_settings_t settings);
static void remove_from_database (remove_settings_t settings);


void
remove_wrapper (int remaining, char **arg_iter)
{

}


/* end of file */