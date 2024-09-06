/* HEMLOCK - a system independent package manager. */
/* <https://github.com/SoftFauna/HEMLOCK.git> */
/* Copyright (c) 2024 The SoftFauna Team */

#ifndef HEMLOCK_MODE_HEADER
#define HEMLOCK_MODE_HEADER
#ifdef __cplusplus  /* C++ compatibility */
extern "C" {
#endif
/* code start */

/* use: hemlock [mode] [args...] 
 *
 * modes:
 *   update [args...]     update an existing package
 *   insert [args...]     insert a new package
 *   remove [args...]     remove a package
 *   search [args...]     search for a specific package
 * special modes:
 *   -h, --help           show this screen
 *       --version        show copyright and version
 * 
 * universal args:
 *   -h, --help           show help page for the given mode
 * 
 * 
 * */

typedef enum
{
    MODE_ERROR,
    MODE_UNRECOGNIZED,
    MODE_NONE,
    MODE_UPDATE,
    MODE_INSERT,
    MODE_REMOVE,
    MODE_SEARCH,
    MODE_HELP,
    MODE_VERSION,
} mode_t;

mode_t mode_select (int argc, char **argv);
void _Noreturn mode_execute (mode_t mode, int argc, char **argv);


/* code end */
#ifdef __cplusplus  /* C++ compatibility */
}
#endif
#endif /* header guard */
/* end of file */