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

#include "gasconfig.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include <sys/stat.h>

#include "common.h"
#include "log.h"
#include "cfgtree.h"
#include "parser.h"

/* For debugging: define GASTOOL_DEBUG_PARSER to trace every line read
   from the configuration files. */

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

static char *parse_config_substring(const char *string, size_t length,
                                    char quote)
{
    char *result = gas_malloc(length + 1);
    char *resp = result;
    size_t i;

    for (i = 0; i < length; i++) {
        if (string[i] == '\\' && (string[i + 1] == '\\'
                                  || (quote && string[i + 1] == quote)))
            *resp++ = string[++i];
        else
            *resp++ = string[i];
    }

    *resp = '\0';

    return result;
}

static int parse_config_string(char **line, char **retval)
{
    char *string = *line, *strend, *result;
    char quote;

    *retval = NULL;

    while (isblank((unsigned char)*string))
        string++;

    if (!*string) {
        *line = string;
        return 0;
    }

    quote = *string;
    if (quote == '"' || quote == '\'') {
        strend = string + 1;

        while (*strend && *strend != quote) {
            if (*strend == '\\' && strend[1]
                && (strend[1] == '\\' || strend[1] == quote))
                strend += 2;
            else
                strend++;
        }

        /* Unclosed quote. */
        if (*strend != quote)
            return -GAS_FAILURE;
        /* Empty string. */
        if (strend - string <= 1)
            return -GAS_FAILURE;

        result = parse_config_substring(string + 1, strend - string - 1,
                                        quote);
        strend++;
    } else {
        strend = string;

        while (*strend && !isblank((unsigned char)*strend))
            strend++;

        result = parse_config_substring(string, strend - string, 0);
    }

    *retval = result;

    while (isblank((unsigned char)*strend))
        strend++;
    *line = strend;

    return 1;
}

static void parse_insert_argv(char ***argv, int *argc, char *string)
{
    *argv = gas_realloc(*argv, (*argc + 1) * sizeof(**argv));
    (*argv)[*argc] = string;
    (*argc)++;
}

static void parse_free_argv(char **argv)
{
    if (argv) {
        size_t i;

        for (i = 0; argv[i] != NULL; i++)
            free(argv[i]);

        free(argv);
    }
}

#define ARGV_MAX 16

static int parse_config_splitline(char **line, int *argc, char ***argv)
{
    char *string;
    int result;

    *argc = 0;
    *argv = NULL;

    do {
        result = parse_config_string(line, &string);
        if (result <= 0)
            break;

        parse_insert_argv(argv, argc, string);
    } while (*argc < ARGV_MAX);

    /* Too many arguments. */
    if (*argc == ARGV_MAX)
        result = -GAS_FAILURE;

    /* Insert NULL in the last position. */
    parse_insert_argv(argv, argc, NULL);

    /* Decrement argc, so argv[argc] is NULL. */
    (*argc)--;

    return result;
}

static directive_t *parse_add_node(directive_t **parent, directive_t *current,
                                   directive_t *newdir, bool child)
{
    if (current == NULL) {
        if (*parent != NULL) {
            (*parent)->child = newdir;
            newdir->parent = *parent;
        }

        if (child) {
            *parent = newdir;
            return NULL;
        }

        return newdir;
    }

    current->next = newdir;
    newdir->parent = *parent;

    if (child) {
        *parent = newdir;
        return NULL;
    }

    return newdir;
}

static int parse_config_line(const char *filename, char *line, int linenum,
                             directive_t **current, directive_t **curr_parent)
{
    char *linep = line, *cmdname, **argv;
    int result, argc;
    directive_t *newdir;

    /* Skip comments and empty lines. */
    if (*line == '#' || *line == '\0')
        return GAS_SUCCESS;

#ifdef GASTOOL_DEBUG_PARSER
    log_print(LOG_DEBUG, 0, "%d:'%s'", linenum, line);
#endif

    /* Open/close block syntax check. Remove the close block character now
       so it will not be converted to a token later. */
    if (*line == '<') {
        char *lastc = line + strlen(line) - 1;
        if (*lastc != '>')
            return -GAS_FAILURE;
        *lastc = '\0';
    }

#ifdef GASTOOL_DEBUG_PARSER
    log_print(LOG_DEBUG, 0, "%d:'%s'", linenum, line);
#endif

    /* Get first token. This will be the directive name. */
    result = parse_config_string(&line, &cmdname);
    if (result <= 0)
        return result;

#ifdef GASTOOL_DEBUG_PARSER
    log_print(LOG_DEBUG, 0, "cmdname='%s'", cmdname);
#endif

    /* Get all tokens. This will be the directive arguments. */
    result = parse_config_splitline(&line, &argc, &argv);
    if (result < 0)
        goto parse_free_memory;

#ifdef GASTOOL_DEBUG_PARSER
    int i;

    log_print(LOG_DEBUG, 0, "argc=%d", argc);
    for (i = 0; i < argc; i++)
        log_print(LOG_DEBUG, 0, "argv[%d]='%s'", i, argv[i]);
#endif

    /* Build the directive and insert it into the tree. Note: close block
       entries are not added and their memory must be freed. */
    result = GAS_SUCCESS;

    if (linep[0] == '<' && linep[1] == '/') {
        if (argc != 0) {
            result = -GAS_FAILURE;
            goto parse_free_memory;
        }

        if (*curr_parent == NULL) {
            result = -GAS_FAILURE;
            goto parse_free_memory;
        }

        if (strcmp(cmdname + 2, (*curr_parent)->directive + 1) != 0) {
            result = -GAS_FAILURE;
            goto parse_free_memory;
        }

        *current = *curr_parent;
        *curr_parent = (*current)->parent;

        goto parse_free_memory;
    }

    newdir = gas_malloc(sizeof(directive_t));
    memset(newdir, 0, sizeof(directive_t));

    newdir->directive = cmdname;
    newdir->argc = argc;
    newdir->argv = argv;
    newdir->filename = gas_strdup(filename);
    newdir->linenum = linenum;

    if (linep[0] == '<')
        *current = parse_add_node(curr_parent, *current, newdir, true);
    else
        *current = parse_add_node(curr_parent, *current, newdir, false);

    goto parse_line_return;

parse_free_memory:
    free(cmdname);
    parse_free_argv(argv);

parse_line_return:
    return result;
}

void free_conf_tree(directive_t *current)
{
    int i;

    if (current == NULL)
        return;

    free(current->directive);

    for (i = 0; i < current->argc; i++)
        free(current->argv[i]);
    free(current->argv);

    free(current->filename);

    if (current->child)
        free_conf_tree(current->child);
    if (current->next)
        free_conf_tree(current->next);

    free(current);
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

        /* Parse the configuration line and insert the node into the tree. */
        result_parser = parse_config_line(filename, line, linenum, &current,
                                          &curr_parent);
        if (result_parser < 0)
            break;

        /* Update conftree reference. */
        if (*conftree == NULL && current != NULL)
            *conftree = current;

        if (*conftree == NULL && curr_parent != NULL)
            *conftree = curr_parent;
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

    if (result_read < 0 || result_parser < 0) {
        free_conf_tree(*conftree);
        *conftree = NULL;
    }

    return (result_read < 0 || result_parser < 0) ? -GAS_FAILURE : GAS_SUCCESS;
}

int read_config_file(const char *filename, directive_t **conftree)
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
