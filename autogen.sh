#!/bin/sh
#
# Copyright (C) 2009  Lincoln de Sousa <lincoln@minaslivre.org>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public
# License along with this program; if not, write to the
# Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.

(test -f configure.ac) || {
    echo "** ERROR: You should run this script from the top-level "
    echo "dir of the source tree."
    exit 1
}

which autoreconf || {
    echo "You don't have the autoreconf program installed. You should "
    echo "install the autotools package (aptitude install autotools in "
    echo "Debian systems)"
    exit 1
}

# Cleaning up everything that is not in git
if [ -d .git ]; then
    > .gitignore
    git clean -df
    git checkout .gitignore
fi

# Finally, running autoreconf
autoreconf -vi

echo "Running ./configure $@"

./configure $@ && echo "Now type \`make' to compile taningia"
