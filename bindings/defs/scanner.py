# -*- coding: utf-8; -*-
#
# Copyright (C) 2009 Lincoln de Sousa <lincoln@minaslivre.org>
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

import sys
import re
import os
import simplejson

def camel_to_underscore(content):
    """Convert names like JAtomEntry to j_atom_entry.
    """
    newstr = ''
    for index, item in enumerate(content):
        if item.isupper() and not index == 0:
            newstr += '_'
            newstr += item.lower()
        else:
            newstr += item.lower()
    return newstr

def clear_spaces(buf):
    """Replace all whitespaces to a single whitespace, like:
    `JAtomContent          *' to `JAtomContent *'.
    """
    while buf.find('  ') != -1:
        buf = buf.replace('  ', ' ')
    return buf

def clear_params(params):
    """Scan the `params' string and return a dict with parsed
    information.
    """
    new_params = []

    # Cleaning params string and splitting params with comma
    params = params.replace('\n', '')
    eparams = params.split(',')

    # Params dict has the following fields: name, type and
    # modifiers. These `modifiers' are empty macros declared after the
    # param name to give some more info about the parameter. The
    # current possible modifiers are `out' and `optional'.
    pat = re.compile('((?:const\s+)?\w+\s*\**)(\w+)(?:\s+([\w\s]+))?')
    for i in eparams:
        param = i.strip()
        if param == '...':
            ptype = 'varargs'
            name = ''
            modifiers = ['optional']
        else:
            found = pat.findall(i)
            ptype, name, modifiers = found[0]
            modifiers = modifiers.split()
        new_params.append({
                'type': ptype.strip(),
                'name': name,
                'modifiers': modifiers,
        })
    return new_params

def strip_comments(buf):
    """Strip C multiline comments from `buf'.
    """
    newbuf = []
    lastidx = 0
    while 1:
        idx = buf.find('/*', lastidx)
        if idx >= 0:
            newbuf.append(buf[lastidx:idx])
            idx = buf.find('*/', idx)
            if idx >= 0:
                lastidx = idx + 2
            else:
                break
        else:
            newbuf.append(buf[lastidx:])
            break
    return ''.join(newbuf)

def scan_file(libprefix, fname):
    """Opens the file `fname' and looks for types, functions methods
    and other kinds of structures in a header file.

    Returns a dict with found info.
    """

    # Cleaning content
    content = open(fname).read()
    ccont = clear_spaces(strip_comments(content))

    # Building header
    module = {}
    module['file'] = fname
    module['name'] = os.path.basename(fname).replace('.h', '')

    # Looking for types
    pat = re.compile('typedef struct _.+ ([^;]+)')
    type_names = pat.findall(ccont)

    # Looking for functions / methods in the whole file
    pat = re.compile('((?:const\s+)?\w+\s*\**)\s*([a-z0-9\_]+)\s*\(([^\)]+)\)')

    methods = []
    for i in pat.findall(ccont):
        rtype, name, params = i
        methods.append({
                'name': name,
                'rtype': rtype.strip(),
                'params': clear_params(params),
        })

    # Associating a method to its `class'.
    types = []
    for i in type_names:
        # All methods of a `class' starts with its underscored name,
        # like this: JAtomEntry methods starts with the j_atom_entry_
        # prefix.
        prefix = '%s_' % camel_to_underscore(i)
        fmethods = []

        # The type dict has the following fields: name, constructor,
        # destructor and methods (another dict build bellow). We don't
        # have anything like `fields', since we use getters and
        # setters in our C library.
        thetype = {'cname': i}

        # Finding module name
        name = i.replace(libprefix.upper(), '', 1)
        thetype['name'] = name

        for method in methods:
            name = method['name']
            # FIXME: This line tries to match things like
            # "j_atom_entry_set_title".startswith("j_atom_entry"). This
            # actually is not right, since we can have another type
            # called "j_atom_entry_content". So, this will need to be
            # rewriten. This works now because there is no clash in
            # our type names today.
            if name.startswith(prefix):
                # method dict has the following fields: name, cname,
                # rtype and params (another dict)
                nmethod = {}
                newname = name.replace(prefix, '')
                nmethod['name'] = newname
                nmethod['cname'] = name
                nmethod['rtype'] = method['rtype']
                nmethod['params'] = method['params']

                # Handling constructor
                if newname == 'new':
                    thetype['constructor'] = nmethod
                # Handling destructor
                elif newname == 'free':
                    thetype['destructor'] = nmethod
                # Normal method
                else:
                    fmethods.append(nmethod)
        thetype['methods'] = fmethods
        types.append(thetype)
    module['types'] = types
    return module

def get_defs(libname, files):
    """Function that build the binding info collecting information
    from files declared in `files'.
    """

    # This is a standard of our library. For example, our main library
    # is called `jarvis', so our namespace starts with `j'. Sure it
    # doesn't fit other libraries, so be carefull.
    libprefix = libname[0]

    # Scaning file by file
    modules = []
    for i in files:
        absname = os.path.abspath(i)
        modules.append(scan_file(libprefix, absname))

    # Dependencies. Today they are hardcoded, maybe some day we could
    # do something more generic.
    public_deps = []
    priv_deps = ['glib.h', 'iksemel.h', 'iksemelmodule.h', 'datetime.h']

    # This is our final answer. A dict containing all collected and
    # (un)useful info about our library.
    library = {'name': libname, 'prefix': libprefix, 'modules': modules,
               'dependencies': {'priv': priv_deps, 'public': public_deps}}

    return library

def main(libname, files):
    """Generates the .json file with `libname'.json with the generated
    info from `files'.
    """

    # Generating our .json file that will be used to feed binding
    # writers.
    json = simplejson.dumps(get_defs(libname, files), indent=2)
    open('%s.json' % libname, 'w').write(json)
    return 0

def usage():
    """Show program usage.
    """
    sys.stderr.write('''%s: [options | headerfiles]
Scan C header files to collect info to generate language bindings

Options can be
  -h, --help        Shows this help
''' % sys.argv[0])
    return 1

if __name__ == '__main__':
    if len(sys.argv) < 2:
        exit(usage())
    if sys.argv[1] in ('-h', '--help'):
        exit(usage())
    if len(sys.argv) < 3:
        exit(usage())
    exit(main(sys.argv[1], sys.argv[2:]))
