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

#ifndef _GASTOOL_COMMON_H
#define _GASTOOL_COMMON_H

/* Function return values that can be used to indicate success or failure.
   Note that GAS_FAILURE is not negative. */

#define GAS_SUCCESS 0           /* Successful return status. */
#define GAS_FAILURE 1           /* Failing return status. */

#define ERRBUF_LEN_MAX 256

char *gas_strerror(int errnum, char *buf, size_t buflen);

#endif
