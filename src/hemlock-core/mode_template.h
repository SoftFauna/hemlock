/* HEMLOCK - a system independent package manager. */
/* <https://github.com/SoftFauna/HEMLOCK.git> */
/* Copyright (c) 2024 The SoftFauna Team */

#ifndef HEMLOCK_MODE_TEMPLATE_HEADER
#define HEMLOCK_MODE_TEMPLATE_HEADER
#ifdef __cplusplus  /* C++ compatibility */
extern "C" {
#endif
/* code start */

#include "settings.h"
#include <stdio.h>

settings_t
mode_template_proccess_args (int argc, char **argv, required_t required, 
                             int (*sequence_cb)(settings_t*, int, char**), 
                             int (*field_cb)(settings_t*, int, char **),
                             void (*help_cb)(FILE*));


/* code end */
#ifdef __cplusplus  /* C++ compatibility */
}
#endif
#endif /* header guard */
/* end of file */