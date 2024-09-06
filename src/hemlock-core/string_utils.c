/* HEMLOCK - a system independent package manager. */
/* <https://github.com/SoftFauna/HEMLOCK.git> */
/* Copyright (c) 2024 The SoftFauna Team */

#include "string_utils.h"

#include <errno.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
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
    void *temp = NULL;
    char *substr_start = NULL;
    char *substr_end = NULL;
    size_t substr_len = 0;
    char *end = NULL; 
    bool last_run = false;

    size_t find_len = 0;

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

    end = strchr (src, '\0');
    substr_start = src;
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
            temp = realloc (dest, (alloc * 2) * sizeof (char *));
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
    char *dest = NULL, **split_array = NULL;
    size_t n = 0;

    if ((NULL == src) || (NULL == find))
    {
        errno = EINVAL;
        return NULL;
    }

    split_array = string_split (src, find, &n);
    if (NULL == split_array) goto string_replace_exit; 
    
    dest = string_join (split_array, n, replace);

    for (int i = 0; i < n; i++) free (split_array[i]);
    free (split_array);

string_replace_exit:
    return dest;
}


char *
string_quote (char *base, char *quote)
{

    char *dest = NULL;
    size_t quote_size = 0, base_size = 0, dest_size = 0;

    if ((NULL == base) || (NULL == quote)) 
    {
        errno = EINVAL;
        return NULL;
    }

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
    size_t int_length = 0;
    char *dest = NULL;

    if (0 == n)     int_length = 1;
    else if (n > 0) int_length = (int)(log10 (fabs (n)) + 1);
    else if (n < 0) int_length = ((int)(log10 (fabs (n)) + 1) + 1);

    dest = malloc (int_length + 1);
    if (NULL == dest)
    {
        errno = ENOMEM;
        return NULL;
    }

    (void)snprintf (dest, int_length + 1, "%d", n);

    return dest;
}


/* end of file */