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
#include <errno.h>

#include <sys/stat.h>

#include "configmake.h"
#include "common.h"
#include "log.h"
#include "cfgtree.h"
#include "cfgfile.h"

#define DEFAULT_CONFIG_FILE SYSCONFDIR "/gastoold.conf"

static int open_config_file(const char *filename, FILE **stream)
{
    FILE *result;
    int status, saved_errno;
    struct stat statbuf;

    result = fopen(filename, "r");
    if (result == NULL) {
        log_print(LOG_ERR, errno, "cannot open configuration file '%s'",
                  filename);
        return -GAS_FAILURE;
    }

    status = stat(filename, &statbuf);
    if (status < 0) {
        saved_errno = errno;

        fclose(result);

        errno = saved_errno;
        log_print(LOG_ERR, errno, "cannot stat configuration file '%s'",
                  filename);
        return -GAS_FAILURE;
    }

    if (!S_ISREG(statbuf.st_mode)) {
        fclose(result);

        log_print(LOG_ERR, 0, "access to file '%s' denied: not a regular file",
                  filename);
        return -GAS_FAILURE;
    }

    *stream = result;

    return GAS_SUCCESS;
}

static int read_config_file(const char *filename, directive_t **conftree)
{
    int result;
    FILE *fp;

    result = open_config_file(filename, &fp);
    if (result < 0)
        return -GAS_FAILURE;

    fclose(fp);

    return GAS_SUCCESS;
}

void read_config(const char *configfile)
{
    int result;
    directive_t *conftree = NULL;

    if (!configfile)
        configfile = DEFAULT_CONFIG_FILE;

    result = read_config_file(configfile, &conftree);
    if (result < 0) {
        /* Failed to parse the configuration file.
           The cause should have already been logged. */
        exit(EXIT_FAILURE);
    }
}
