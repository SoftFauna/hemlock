/* HEMLOCK - a system independent package manager. */
/* <https://github.com/SoftFauna/HEMLOCK.git> */
/* Copyright (c) 2024 The SoftFauna Team */

#include "string_utils.h"

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


char *
string_clone (const char *src)
{
    char *dest;
    size_t n;

    if (NULL == src) 
    {
        errno = EINVAL;
        return NULL;
    }

    n = strlen (src);
    dest = malloc (n + 1);
    if (NULL == dest) 
    {
        errno = ENOMEM;
        return NULL;
    }

    strcpy (dest, src);

    return dest;
}


char *
substring_clone (const char *src, size_t n)
{
    char *dest;
    
    if ((NULL == src) || (0 == n)) return NULL;

    dest = malloc (n + 1);
    if (NULL == dest) return NULL;

    (void)strncpy (dest, src, n);
    dest[n] = '\0';

    return dest;
}


char *
string_join (char **array, size_t n, char *seperator)
{
    char *dest = NULL;
    size_t dest_len = 0;

    for (size_t i = 0; i < n; i++)
    {
        dest_len += strlen (array[i]);
    }

    dest_len += (strlen (seperator) * (n - 1));

    dest = calloc (dest_len + 1, sizeof (char));
    if (NULL == dest)
    {
        return NULL;
    }

    for (size_t i = 0; i < n; i++)
    {
        (void)strcat (dest, array[i]);

        if ((i + 1) < n) (void)strcat (dest, seperator);
    }

    return dest;
}


char **
string_split (char *src, char *find, size_t *length_out)
{
    char *substr_start = src;
    char *substr_end = NULL;
    size_t substr_len = 0;
    char *end = strchr (src, '\0');
    bool last_run = false;

    size_t find_len = NULL;

    char **dest = NULL;
    size_t alloc = 3;
    size_t count = 0;

    if ((NULL == src) || (NULL == find)) 
    {
        errno = EINVAL;
        goto string_split_exit;
    }

    find_len = strlen (find);

    dest = malloc (alloc * sizeof (char *));
    if (NULL == dest) 
    {
        errno = ENOMEM;
        goto string_split_exit;
    }

    while (false == last_run)
    {
        substr_end = strstr (substr_start, find);
        if (NULL == substr_end)
        {
            last_run = true;
            substr_end = end;
        }

        if (count == alloc)
        {
            void *temp = realloc (dest, (alloc * 2) * sizeof (char *));
            if (NULL == temp)
            {   /* handle out of memeory */
                for (int i = 0; i < count; i++) 
                { 
                    free (dest[i]); dest[i] = NULL; 
                }
                free (dest); dest = NULL;
                count = 0;
                errno = ENOMEM;
                goto string_split_exit;
            }

            dest = temp; temp = NULL;
            alloc = alloc * 2;
        }

        substr_len = substr_end - substr_start;
        dest[count] = substring_clone (substr_start, substr_len);
        count++;

        substr_start = substr_end + find_len;
    }

string_split_exit:
    if (NULL != length_out) *length_out = count;
    return dest;
}


char *
string_replace (char *src, char *find, char *replace)
{
    char *dest, **split_array;
    size_t n;

    if (NULL == src) return NULL;
    if (NULL == find) return NULL;

    split_array = string_split (src, find, &n);
    dest = string_join (split_array, n, replace);

    for (int i = 0; i < n; i++) free (split_array[i]);
    free (split_array);

    return dest;
}


char *
string_quote (char *base, char *quote)
{
    if ((NULL == base) || (NULL == quote)) 
    {
        errno = EINVAL;
        return NULL;
    }

    char *dest;
    size_t quote_size, base_size, dest_size;

    base_size  = strlen (base);
    quote_size = strlen (quote);
    dest_size  = quote_size + base_size + quote_size;

    dest = calloc (dest_size + 1, sizeof (char));
    if (NULL == dest)
    {
        errno = ENOMEM;
        return NULL;
    }

    (void)strcat (dest, quote);
    (void)strcat (dest, base);
    (void)strcat (dest, quote);

    return dest;
}


char *
int_to_string (int n)
{
    size_t int_length;
    char *dest;

    if (0 == n)     int_length = 1;
    else if (n > 0) int_length = (int)(log10 (fabs (n)) + 1);
    else if (n < 0) int_length = ((int)(log10 (fabs (n)) + 1) + 1);

    dest = malloc (int_length + 1);
    if (NULL == dest)
    {
        errno = ENOMEM;
        return NULL;
    }

    (void)itoa (n, dest, 10);

    return dest;
}


/* end of file */