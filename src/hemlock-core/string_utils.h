/* HEMLOCK - a system independent package manager. */
/* <https://github.com/SoftFauna/HEMLOCK.git> */
/* Copyright (c) 2024 The SoftFauna Team */

#ifndef HEMLOCK_STRING_UTILS_HEADER
#define HEMLOCK_STRING_UTILS_HEADER
#ifdef __cplusplus  /* C++ compatibility */
extern "C" {
#endif
/* code start */

#include <stddef.h>


char *string_clone (const char *src);
char *substring_clone (const char *src, size_t n);
char *string_join (char **array, size_t n, char *seperator);
char **string_split (char *src, char *find, size_t *length_out);
char *string_replace (char *src, char *find, char *replace);
char *string_quote (char *base, char *quote);
char *int_to_string (int n);


/* code end */
#ifdef __cplusplus  /* C++ compatibility */
}
#endif
#endif /* header guard */
/* end of file */