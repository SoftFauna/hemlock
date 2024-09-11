/* HEMLOCK - a system independent package manager. */
/* <https://github.com/SoftFauna/HEMLOCK.git> */
/* Copyright (c) 2024 The SoftFauna Team */

#ifndef HEMLOCK_SETTINGS_HEADER
#define HEMLOCK_SETTINGS_HEADER
#ifdef __cplusplus  /* C++ compatibility */
extern "C" {
#endif
/* code start */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>


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
} settings_t;

const enum
{
    REQUIRE_NONE         = 0x0000,    /* 0000 0000 */
    REQUIRE_DATABASE     = 0x0001,    /* 0000 0001 */
    REQUIRE_NAME         = 0x0002,    /* 0000 0010 */
    REQUIRE_VERSION      = 0x0004,    /* 0000 0100 */
    REQUIRE_HOMEPAGE     = 0x0008,    /* 0000 1000 */
    REQUIRE_MAINTAINER   = 0x0010,    /* 0001 0000 */
    REQUIRE_EMAIL        = 0x0020,    /* 0010 0000 */
    REQUIRE_REQUIRE_LIST = 0x0040,    /* 0100 0000 */
    REQUIRE_FILE_LIST    = 0x0080,    /* 1000 0000 */
};

typedef uint32_t required_t;


settings_t settings_default (void);
void settings_print (FILE *fp, settings_t settings);
required_t settings_validate (settings_t settings, required_t require);
void settings_log_required (FILE *fp, required_t missing);


/* code end */
#ifdef __cplusplus  /* C++ compatibility */
}
#endif
#endif /* header guard */
/* end of file */