/* HEMLOCK - a system independent package manager. */
/* <https://github.com/SoftFauna/HEMLOCK.git> */
/* Copyright (c) 2024 The SoftFauna Team */

#include "mode_template.h"

#include "arguement.h"
#include "settings.h"
#include <stdio.h>
#include <stdlib.h>


settings_t
mode_template_proccess_args (int argc, char **argv, required_t required,
                             int (*sequence_cb)(settings_t*, int, char**),
                             int (*field_cb)(settings_t*, int, char**),
                             void (*help_cb)(FILE*))
{    
    required_t missing  = REQUIRE_NONE;
    settings_t settings = settings_default ();

    /* get sequential arguements */
    if (NULL != sequence_cb)
    {
        int n = sequence_cb (&settings, argc, argv);
        CONARG_STEP_N_UNSAFE (argc, argv, n);
    }

    /* get flag arguements */
    if (NULL != field_cb)
    {
        (void)field_cb (&settings, argc, argv);
    }

    /* validate that required settings are present */
    missing = settings_validate (settings, required);
    if (REQUIRE_NONE != missing) 
    {
        settings_log_required (stderr, missing);
        if (NULL != help_cb)
        {
            help_cb (stderr);
        }
        exit (EXIT_FAILURE);
    }
    
    return settings;

}


/* end of file */