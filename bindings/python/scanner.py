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

_ = lambda x: x

NAMESPACE = 'J'

def usage():
    sys.stderr.write(_('%s: [options | headerfile]\n' % sys.argv[0]))
    sys.stderr.write(_('Scan C header files to collect info to generate'
                       'python bindings\n\n'))
    sys.stderr.write(_('Options can be\n'))
    sys.stderr.write(_('  -h, --help\t\tShows this help'))
    sys.stderr.write(_('\n\n'))
    return 1

def camel_to_underscore(content):
    newstr = ''
    for index, item in enumerate(content):
        if item.isupper() and not index == 0:
            newstr += '_'
            newstr += item.lower()
        else:
            newstr += item.lower()
    return newstr

def clear_type(type_):
    while '  ' in type_:
        type_ = type_.replace('  ', ' ')
    while type_[-1] == ' ':
        type_ = type_[:-1]
    return type_

def clear_params(params):
    find_type = re.compile('((?:const\s+)?\w+\s*\**)\s*([\w\d]+)')
    new_params = []
    for type_, name in find_type.findall(params):
        new_params.append((clear_type(type_), name))
    return new_params

def scan(files):
    find_typedefs = re.compile('typedef struct _.+ ([^;]+)')
    ns = NAMESPACE.lower()
    ret = []

    for hfile in files:
        content = open(hfile).read()
        new_types = find_typedefs.findall(content)
        for type_ in new_types:
            type_name = type_[1:]
            type_prefix = camel_to_underscore(type_name)
            method_prefix = '%s_%s_' % (ns, type_prefix)
            find_methods = \
                re.compile('([\w][\w\d]+\s*\*?)(%s[\w\d]+)\s*\(([^\)]+)' % (
                    method_prefix))
            ptype = {'name': type_name}
            ptype['cname'] = type_
            ptype['doc'] = 'Class %s binded from c type %s' % (type_name, type_)
            ptype['lib'] = os.path.basename(hfile).replace('.h', '')
            ptype['methods'] = []
            for type_, name, params in find_methods.findall(content):
                mname = name.replace(method_prefix, '')
                method = {}
                method['name'] = mname
                method['cname'] = name
                method['rtype'] = clear_type(type_)
                method['params'] = clear_params(params)
                method['doc'] = 'Method %s from the object %s' % (
                    mname, type_name)
                if mname == 'new':
                    ptype['constructor'] = method
                    continue
                if mname == 'free':
                    ptype['destructor'] = method
                    continue
                ptype['methods'].append(method)
            ret.append(ptype)
    return ret

def main(files):
    scan(files)
    return 0

if __name__ == '__main__':
    if len(sys.argv) < 2:
        exit(usage())
    if sys.argv[1] in ('-h', '--help'):
        exit(usage())
    exit(main(sys.argv[1:]))
