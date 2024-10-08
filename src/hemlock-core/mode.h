/* HEMLOCK - a system independent package manager. */
/* <https://github.com/SoftFauna/HEMLOCK.git> */
/* Copyright (c) 2024 The SoftFauna Team */

#ifndef HEMLOCK_MODE_HEADER
#define HEMLOCK_MODE_HEADER
#ifdef __cplusplus  /* C++ compatibility */
extern "C" {
#endif
/* code start */

/* usage: hemlock <mode> [modeargs...] 
 * or     hemlock <mode> --help
 * or     hemlock --help 
 * */

void _Noreturn mode_exec (int argc, char **argv);

/* code end */
#ifdef __cplusplus  /* C++ compatibility */
}
#endif
#endif /* header guard */
/* end of file */