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
#include <ctype.h>
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

static int read_config_line(char **buf, size_t *bufsize, FILE *fp)
{
    char *linep, *cp;
    ssize_t len;

    if (feof(fp))
        return 0;

    len = getdelim(buf, bufsize, '\n', fp);
    if (len < 0)
        return -errno;

    linep = *buf;

    /* Strip trailing newline characters. */
    while (len > 0
           && (linep[len - 1] == '\n' || linep[len - 1] == '\r'))
        len--;
    linep[len] = '\0';

    /* Strip trailing whitespace. */
    while (len > 0 && isblank((unsigned char)linep[len - 1]))
        len--;
    linep[len] = '\0';

    /* Strip leading whitespace. */
    for (cp = linep; isblank((unsigned char)*cp); cp++)
        len--;
    memmove(linep, cp, len);
    linep[len] = '\0';

    return 1;
}

static int parse_config_line(const char *filename, char *line, int linenum,
                             directive_t **current, directive_t **curr_parent)
{
    /* Skip comments and empty lines. */
    if (*line == '#' || *line == '\0')
        return GAS_SUCCESS;

    log_print(LOG_DEBUG, 0, "%d:'%s'", linenum, line);

    return GAS_SUCCESS;
}

static int parse_config_file(const char *filename, FILE *fp,
                             directive_t **conftree)
{
    char *line = NULL;
    size_t linesize = 0;
    int linenum = 0;
    directive_t *current = *conftree;
    directive_t *curr_parent = NULL;
    int result_read = 0, result_parser = 0;

    while ((result_read = read_config_line(&line, &linesize, fp)) > 0) {
        /* Increment line number. */
        linenum++;

        result_parser = parse_config_line(filename, line, linenum, &current,
                                          &curr_parent);
        if (result_parser < 0)
            break;
    }

    free(line);

    if (result_read < 0) {
        log_print(LOG_ERR, result_read, "error reading '%s' at line %d",
                  filename, linenum);
    }

    if (result_parser < 0) {
        log_print(LOG_ERR, 0, "syntax error in file '%s' at line %d",
                  filename, linenum);
    }

    return (result_read < 0 || result_parser < 0) ? -GAS_FAILURE : GAS_SUCCESS;
}

static int read_config_file(const char *filename, directive_t **conftree)
{
    int result;
    FILE *fp;

    result = open_config_file(filename, &fp);
    if (result < 0)
        return -GAS_FAILURE;

    /* Parse the configuration file and build the tree. */
    result = parse_config_file(filename, fp, conftree);

    fclose(fp);

    if (result < 0)
        return -GAS_FAILURE;

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
