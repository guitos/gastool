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
#include <stdarg.h>
#include <errno.h>

#include "log.h"

/* Default log level. */
static int default_log_level = LOG_INFO;

static void log_write(int level, const char *format, va_list args)
{
    if (level > default_log_level)
        return;

    /* Write only to stderr for now. */
    vfprintf(stderr, format, args);
    fputc('\n', stderr);

    fflush(stderr);
}

void log_print(int level, const char *format, ...)
{
    va_list args;

    va_start(args, format);
    log_write(level, format, args);
    va_end(args);
}

int log_set_default_level(int level)
{
    if (0 > level || level > LOG_PRIMASK) {
        errno = EINVAL;
        return -1;
    }

    default_log_level = level;
    return 0;
}
