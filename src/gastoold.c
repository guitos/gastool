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
#include <getopt.h>
#include <limits.h>

#include "log.h"
#include "cfgfile.h"

#define PROGRAM_AUTHOR \
    "Guilherme de A. Suckevicz"

/* String containing name the program is called with.
   To be initialized by main(). */
static const char *program_name = NULL;

/* Configuration file pathname. */
static const char *configfile = NULL;

/* For long options that have no equivalent short option, use a
   non-character as a pseudo short option, starting with CHAR_MAX + 1. */
enum {
    HELP_OPTION = CHAR_MAX + 1,
    VERSION_OPTION
};

static struct option const long_options[] = {
    {"config", required_argument, NULL, 'c'},
    {"debug", no_argument, NULL, 'd'},
    {"help", no_argument, NULL, HELP_OPTION},
    {"version", no_argument, NULL, VERSION_OPTION},
    {NULL, 0, NULL, 0}
};

static void usage(int status)
{
    if (status != EXIT_SUCCESS) {
        fprintf(stderr, "Try '%s --help' for more information.\n",
                program_name);
    } else {
        printf("Usage: %s [OPTION]...\n", program_name);

        fputs("\n\
  -c, --config=FILE  specify config file to use\n\
  -d, --debug        enable debug mode\n\
      --help     display this help and exit\n\
      --version  output version information and exit\n", stdout);

        fputc('\n', stdout);

        printf("Report bugs to: <%s>.\n", PACKAGE_BUGREPORT);
        printf("%s home page: <%s>.\n", PACKAGE_NAME, PACKAGE_URL);
    }

    exit(status);
}

static void print_version(void)
{
    printf("%s %s\n", program_name, PACKAGE_VERSION);

    printf("\
License GPLv3+: GNU GPL version 3 or later <%s>.\n\
This is free software: you are free to change and redistribute it.\n\
There is NO WARRANTY, to the extent permitted by law.\n\
",
           "https://gnu.org/licenses/gpl.html");

    fputc('\n', stdout);

    printf("Written by %s.\n", PROGRAM_AUTHOR);

    exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
    int optc;

    program_name = argv[0];

    while ((optc = getopt_long(argc, argv, "c:d", long_options, NULL))
           != -1) {
        switch (optc) {
        case 'c':
            configfile = optarg;
            break;

        case 'd':
            log_set_default_level(LOG_DEBUG);
            break;

        case HELP_OPTION:
            usage(EXIT_SUCCESS);
            break;
        case VERSION_OPTION:
            print_version();
            break;

        default:
            usage(EXIT_FAILURE);
            break;
        }
    }

    log_print(LOG_DEBUG, 0, "%s version %s", program_name, PACKAGE_VERSION);

    read_config(configfile);

    exit(EXIT_SUCCESS);
}
