dnl Autoconf script for Gastool.
dnl Copyright (C) 2020 Guilherme de Almeida Suckevicz.

dnl This program is free software: you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation, either version 3 of the License, or
dnl (at your option) any later version.

dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.

dnl You should have received a copy of the GNU General Public License
dnl along with this program.  If not, see <https://www.gnu.org/licenses/>.

AC_PREREQ([2.64])

AC_INIT([Gastool], [0.1], [https://github.com/guitos/gastool/issues/],
  [gastool], [https://github.com/guitos/gastool/])

AC_CONFIG_SRCDIR([src/gastoold.c])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([foreign subdir-objects])

dnl Default to silent rules.
AM_SILENT_RULES([yes])

dnl Check for programs.
AC_PROG_CC

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
