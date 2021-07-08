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

#ifndef _GASTOOL_PARSER_H
#define _GASTOOL_PARSER_H

#include "cfgtree.h"

int read_config_file(const char *filename, directive_t **conftree);

void free_conf_tree(directive_t *current);

#endif  /* !_GASTOOL_PARSER_H */
