/* Copyright (C) 2020 Guilherme de Almeida Suckevicz.
   This file is part of Gastool.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>. */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "common.h"

static void gas_alloc_die(void)
{
    log_print(LOG_CRIT, 0, "memory exhausted");
    exit(EXIT_FAILURE);
}

void *gas_malloc(size_t n)
{
    void *p = malloc(n);
    if (!p)
        gas_alloc_die();
    return p;
}

void *gas_realloc(void *p, size_t n)
{
    void *r = realloc(p, n);
    if (!r && n)
        gas_alloc_die();
    return r;
}

char *gas_strdup(const char *string)
{
    char *s = strdup(string);
    if (!s)
        gas_alloc_die();
    return s;
}

char *gas_strerror(int errnum, char *buf, size_t buflen)
{
    int result;

    *buf = '\0';

    if (errnum < 0)
        errnum = abs(errnum);

    result = strerror_r(errnum, buf, buflen);

    if (result != 0)
        snprintf(buf, buflen, "Unknown error %d", errnum);

    return buf;
}
