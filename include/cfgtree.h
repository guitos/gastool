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

#ifndef _GASTOOL_CFGTREE_H
#define _GASTOOL_CFGTREE_H

struct directive_t {
    /* The current directive name. */
    char *directive;

    /* The arguments of this directive. */
    int argc;
    char **argv;

    /* The filename this directive was found. */
    char *filename;
    /* The line number this directive was found. */
    int linenum;

    /* The next directive node in the tree. */
    struct directive_t *next;

    /* The parent node of this directive. */
    struct directive_t *parent;

    /* The child node of this directive. */
    struct directive_t *child;
};

typedef struct directive_t directive_t;

#endif
