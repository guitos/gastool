# Generate configmake header file.
# Copyright (C) 2020 Guilherme de Almeida Suckevicz.

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

include/configmake.h: Makefile
	$(AM_V_GEN)rm -f $@-t && \
	{ echo '#define PREFIX "$(prefix)"'; \
	  echo '#define SYSCONFDIR "$(sysconfdir)"'; \
	} | sed '/""/d' > $@-t && \
	mv -f $@-t $@

BUILT_SOURCES += include/configmake.h
CLEANFILES += include/configmake.h include/configmake.h-t
