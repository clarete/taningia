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
import warnings
import simplejson

# This is a standard of our library. For example, our main library is
# called `taningia', so our namespace starts with `ta'. Sure it
# doesn't fit other libraries, so be carefull.
LIBPREFIX = 'ta_'

def camel_to_underscore(content):
    """Convert names like TAtomEntry to t_atom_entry.
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
    `TAtomContent          *' to `TAtomContent *'.
    """
    while buf.find('  ') != -1:
        buf = buf.replace('  ', ' ')
    return buf.strip()

def clear_params(params, dparams):
    """Scan the `params' string and return a dict with parsed
    information.
    """
    new_params = []

    # Cleaning params string and splitting params with comma
    params = params.replace('\n', '')
    eparams = params.split(',')

    # Handling unique void param
    if len(eparams) == 1 and eparams[0] == 'void':
        return []

    # Params dict has the following fields: name, type and
    # modifiers. These `modifiers' give some more info about the
    # parameter, they are declared in the method docstring. The
    # current possible modifiers are `out', `optional' and `len'.
    pat = re.compile('((?:const\s+)?\w+\s*\**)(\w+)')
    for i in eparams:
        param = i.strip()
        if param == '...':
            ptype = 'varargs'
            name = ''
            modifiers = ['optional']
        else:
            found = pat.findall(i)
            ptype, name = found[0]
            modifiers = []
            # Looking for parameter modifiers declared
            for dparam in dparams.get('params', []):
                if dparam['name'] == name:
                    modifiers = dparam['modifiers']
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

def docstrings(buf):
    result = []
    lastidx = 0
    while True:
        start = buf.find('/**', lastidx)
        stop = buf.find('*/', start)
        if start >= 0:
            result.append(buf[start+3:stop])
            lastidx = stop
        else:
            break
    return result

def parse_docstring(dstring):
    # Result will be put here
    ret = {}

    # Clearing `*' chars from the docstring
    newbuf = []
    for i in dstring.split('\n'):
        line = re.sub('^\s*\*\s*', '', i).strip()
        newbuf.append(line)

    # Splitting parameters from the description
    broken = '\n'.join(newbuf).split('\n\n', 1)

    # Handling parameters starting with @
    params = broken[0]
    dvars = re.findall('@([^:]+):\s*([^@]+)', params)
    params = {}
    for key, val in dvars:
        skey = key.strip()
        if skey == 'name':
            ret['cname'] = val.strip()
        elif skey == 'raise':
            ret['raise'] = [x.strip() for x in val.strip().split(',')]
        elif skey == 'return':
            if '(' in val:
                # The user is trying to say that the object being
                # returned is a container and the objects inside the
                # container are described between parenthesis, like
                # this:
                #
                # /* @name: ...
                #  * @return: ta_list (ta_atom_simple_element)
                #  */
                #
                # So, we need to grab this info and pass to the
                # binding writing programs.
                pval = val.replace(')', '')
                pval, subtype = pval.split('(')
                pval = pval.strip()
                subtype = subtype.strip()
            else:
                pval = val
                subtype = ''
            ret['return'] = {'type': pval, 'subtype': subtype}
        elif skey.startswith('param'):
            nil, pname = skey.split(' ', 1)
            modifiers = []
            if '(' in pname:
                # We found modifiers! let's handle it.
                pname = pname.replace(')', '')
                pname, modifiers = pname.split('(')
                pname = pname.strip()
                modifiers = [x.strip() for x in modifiers.split(',')]

            # This will be the entry in the return list.
            pfull = {'name': pname, 'modifiers': modifiers}

            # Now we'll test if any other param was already added. If
            # not, we will need to start a new list.
            if ret.get('params'):
                ret['params'].append(pfull)
            else:
                ret['params'] = [pfull]
        elif skey == 'type':
            try:
                typen, classn = val.strip().rsplit(' ', 1)
            except ValueError:
                raise Exception('Docstring parsing error')
            ret['type'] = typen
            if typen in ('getter', 'setter'):
                sval = classn.split(':')
                ret['class'] = sval[0]
                ret['prop'] = sval[1]
            else:
                ret['class'] = classn            
        params[key.strip()] = val.strip()

    if 'class' in ret:
        ret['name'] = ret['cname'] \
            .replace('%s_' % ret['class'], '') \
            .replace(LIBPREFIX, '')

    # Handling description if it is found
    if len(broken) > 1:
        description = broken[1]
        ret['doc'] = description.strip()
    return ret

def scan_file(fname):
    """Opens the file `fname' and looks for types, functions methods
    and other kinds of structures in a header file.

    Returns a dict with found info.
    """

    # Cleaning content
    content = open(fname).read()
    allds = []
    dstrings = {}
    for i in docstrings(content):
        allds.append(parse_docstring(i))
        parsed = parse_docstring(i)
        dstrings[parsed['cname']] = parsed
    ccont = clear_spaces(strip_comments(content))

    # Building header
    module = {}
    module['file'] = fname
    module['name'] = os.path.basename(fname).replace('.h', '')

    # Looking for types
    pat = re.compile('typedef struct _.+ ([^;]+)_t')
    type_names = pat.findall(ccont)

    # Looking for enums
    pat = re.compile('(?:typedef\s)?enum {([^}]*)} ([^;]+)')
    module['enums'] = []
    for i in pat.findall(ccont):
        econt, name = i
        econt = clear_spaces(econt.replace('\n', ''))
        vals = [x for x in econt.split(',') if x]
        isflags = '<<' in econt
        if isflags:
            entries = []
            for i in vals:
                entry = i.split('=')[0].strip()
                entries.append(entry)
        else:
            entries = vals
        module['enums'].append({
                'name': name,
                'isflags': isflags,
                'entries': entries
        })

    # Looking for functions / methods in the whole file
    pat = re.compile('((?:const\s+)?\w+\s*\**)\s*([a-z0-9\_]+)\s*\(([^\)]+)\)')

    methods = []
    for i in pat.findall(ccont):
        rtype, name, params = i
        try:
            pparams = clear_params(params, dstrings[name])
        except KeyError:
            warnings.warn('C identifier %s has no docstring or it is wrong' %
                          name)
            continue
        methods.append({
                'name': name,
                'rtype': rtype.strip(),
                'rinfo': dstrings[name].get('return'),
                'params': pparams,
        })

    # Associating a method to its `class'.
    types = []
    for i in type_names:
        fmethods = []

        # The type dict has the following fields: name, constructor,
        # destructor and methods (another dict build bellow). We don't
        # have anything like `fields', since we use getters and
        # setters in our C library.
        thetype = {}

        # Finding module name
        name = i.replace(LIBPREFIX, '', 1)
        thetype['name'] = name
        thetype['cname'] = '%s_t' % i

        for method in methods:
            name = method['name']
            dsmethod = None
            cclass = thetype['cname'].replace('_t', '')
            for i in allds:
                if i['cname'] == name and i['class'] == cclass:
                    dsmethod = i
                    break
            if not dsmethod:
                continue

            # method dict has the following fields: name, cname, rtype
            # and params (another dict)
            nmethod = {}
            nmethod.update(dsmethod)
            nmethod['rtype'] = method['rtype']
            nmethod['params'] = method['params']

            # Handling constructor
            if dsmethod['name'] == 'new':
                thetype['constructor'] = nmethod

            # Handling destructor
            elif dsmethod['name'] == 'free':
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

    # Scaning file by file
    modules = []
    for i in files:
        absname = os.path.abspath(i)
        modules.append(scan_file(absname))

    # Dependencies. Today they are hardcoded, maybe some day we could
    # do something more generic.
    public_deps = []
    priv_deps = ['glib.h', 'iksemel.h', 'iksemelmodule.h', 'datetime.h']

    # This is our final answer. A dict containing all collected and
    # (un)useful info about our library.
    library = {'name': libname, 'prefix': LIBPREFIX, 'modules': modules,
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
