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
import scanner
from overrides import OVERRIDES

HEADER_DIR = 'jarvis'

TYPE_MAP = {
    'void': 'z',                # None
    'void *': 'O',              # object
    'const char *': 's',        # str
    'char *': 's',              # str
    'int': 'i',                 # int
    'long': 'L',                # long
    'double': 'd',              # double
    'float': 'd',               # double
}

TEMPLATE_HEAD = '''#include <%(header_dir)s/%(lib)s.h>'''

TEMPLATE_TYPES = '''typedef struct {
  PyObject_HEAD
  %(cname)s *inner;
} %(type_name)s;
'''

TEMPLATE_METHOD_HEAD = '''static PyObject *
%(obj_name)s_%(method_name)s (%(obj_name)s *self, PyObject *args)
{
%(params)s
  %(ret_var)s%(cname)s (self->inner%(param_names)s);
  %(ret)s
}
'''

TEMPLATE_MODULE_INIT = '''  if (PyType_Ready (&%(type_name)sType) < 0)
    return;
  Py_INCREF (&%(type_name)sType);
  PyModule_AddObject (m, "%(type_name)s", (PyObject *) &%(type_name)sType);
'''

TEMPLATE_METHOD = '''  { "%(method_name)s", (PyCFunction) %(type_name)s_%(method_name)s,
    %(arg_type)s, "%(doc)s" },
'''

TEMPLATE_METHODS = '''
static PyMethodDef %(type_name)s_methods[] = {
%(methods)s
  { NULL }                        /* sentinel */
};
'''

TEMPLATE_CONSTRUCTOR = '''static int
%(type_name)s_init (%(type_name)s *self,
                    PyObject      *args,
                    PyObject      *kwargs)
{
%(params)s
  self->inner = %(constructor_name)s (%(param_names)s);
  return 0;
}
'''

TEMPLATE_DESTRUCTOR = '''static void
%(type_name)s_dealloc (%(type_name)s *self)
{
  if (self->inner)
    %(destructor_name)s (self->inner);
  self->ob_type->tp_free ((PyObject *) self);
}'''

TEMPLATE_TYPE = '''
static PyTypeObject %(type_name)sType = {
  PyObject_HEAD_INIT(NULL)
  0,                                        /* ob_size */
  "%(module_name)s.%(type_name)s",          /* tp_name */
  sizeof (%(type_name)s),                   /* tp_basicsize */
  0,                                        /* tp_itemsize */
  (destructor) %(type_name)s_dealloc,       /* tp_dealloc */
  0,                                        /* tp_print */
  0,                                        /* tp_getattr */
  0,                                        /* tp_setattr */
  0,                                        /* tp_compare */
  0,                                        /* tp_repr */
  0,                                        /* tp_as_number */
  0,                                        /* tp_as_sequence */
  0,                                        /* tp_as_mapping */
  0,                                        /* tp_hash */
  0,                                        /* tp_call */
  0,                                        /* tp_str */
  0,                                        /* tp_getattro */
  0,                                        /* tp_setattro */
  0,                                        /* tp_as_buffer */
  Py_TPFLAGS_DEFAULT,                       /* tp_flags */
  "%(type_name)s Objects",                  /* tp_doc */
  0,                                        /* tp_traverse */
  0,                                        /* tp_clear */
  0,                                        /* tp_richcompare */
  0,                                        /* tp_weaklistoffset */
  0,                                        /* tp_iter */
  0,                                        /* tp_iternext */
  %(type_name)s_methods,                    /* tp_methods */
  0,                                        /* tp_members */
  0,                                        /* tp_getset */
  0,                                        /* tp_base */
  0,                                        /* tp_dict */
  0,                                        /* tp_descr_get */
  0,                                        /* tp_descr_set */
  0,                                        /* tp_dictoffset */
  (initproc) %(type_name)s_init,            /* tp_init */
  0,                                        /* tp_alloc */
  PyType_GenericNew,                        /* tp_new */
};
'''

TEMPLATE_MODULE = '''
#ifndef PyMODINIT_FUNC
#define PyMODINIT_FUNC void
#endif

PyMODINIT_FUNC
init%(mname)s (void)
{
  PyObject *m;
  m = Py_InitModule3 ("%(mname)s", NULL, "%(doc)s");

%(modules)s
}
'''
def parse_return(obj, method):
    if method['rtype'] != 'void':
        return '%(rtype)s _ret = (%(rtype)s) ' % method
    return ''

def build_return(obj, method):
    rtype = method['rtype'];
    if rtype in ('char *', 'const char *'):
        return 'return Py_BuildValue ("s", _ret);'
    if rtype == 'iks *':
        return 'PyObject *nret = PyIks_FromIks (_ret);\n' + \
            '  Py_INCREF (nret)\n;' + \
            '  return nret;'
    else:
        return 'return Py_BuildValue ("");'

def parse_arg_names(obj, method):
    params = method['params']
    if params[0][0] in (obj['cname'], '%s *' % obj['cname']):
        params = params[1:]

    newparams = []
    for ptype, name in params:
        newparams.append('_%s' % name)
    return ', '.join(newparams)

def parse_args(obj, method):
    args = []
    app = args.append
    params = method['params']
    svars = []

    # We don't need to parse the `self' parameter
    if params[0][0] in (obj['cname'], '%s *' % obj['cname']):
        params = params[1:]

    # Here we're declaring variables that actually holds values that
    # will be passed to the method.
    for ptype, name in params:
        app('  %s _%s;' % (ptype, name))
        svars.append('&_%s' % name)

    # Here we're discovering which params should be passed to the
    # function and building a string with needed arguments with its
    # types.
    types = ''
    for ptype, name in params:
        pytype = TYPE_MAP.get(ptype, 'O') # Fallingback to `object'
        types += pytype

    # Calling python api function that parse arguments from the args
    # list.
    if params:
        app('  if (!PyArg_ParseTuple (args, "%s", %s))' % (
                types, ', '.join(svars)))
        if method['name'] == 'new': # Handling constructor
            app('    return -1;')
        else:
            app('    return NULL;')
    return '\n'.join(args)

def write_header(objs):
    ret = []
    app = ret.append
    # FIXME: Hardcoded includes
    app('#include <Python.h>')
    app('#include <datetime.h>')
    app('#include <glib.h>')
    app('#include <iksemel.h>')
    app('#include <iksemelmodule.h>')
    libs = set()
    for obj in objs:
        libs.add(obj['lib'])
    for i in libs:
        app(TEMPLATE_HEAD % {'header_dir': HEADER_DIR, 'lib': i})
    app('')
    for obj in objs:
        app(TEMPLATE_TYPES % {
                'type_name': obj['name'],
                'cname': obj['cname']})
    return '\n'.join(ret)

def write_module_init(mname, objs):
    modules = []
    app = modules.append
    for i in objs:
        tpl = TEMPLATE_MODULE_INIT % {
            'type_name': i['name'],
            'doc': i['doc']}
        app(tpl)
    return TEMPLATE_MODULE % {
        'mname': mname,
        'doc': 'Jarvis library bindings',
        'modules': '\n'.join(modules)}

def write_constructor(obj):
    if 'constructor' in obj.keys():
        return TEMPLATE_CONSTRUCTOR % {
            'type_name': obj['name'],
            'constructor_name': obj['constructor']['cname'],
            'params': parse_args(obj, obj['constructor']),
            'param_names': parse_arg_names(obj, obj['constructor']),
            }
    return ''

def write_destructor(obj):
    if 'destructor' in obj.keys():
        return TEMPLATE_DESTRUCTOR % {
            'type_name': obj['name'],
            'destructor_name': obj['destructor']['cname'],
            }
    return''

def write_type(obj):
    return TEMPLATE_TYPE % {'type_name': obj['name'],
                            'module_name': obj['lib']}

def write_methods(obj):
    methods = []
    app = methods.append
    for i in obj['methods']:
        ctx = {}
        ctx['method_name'] = i['name']
        ctx['type_name'] = obj['name']
        ctx['doc'] = i['doc']
        if len(i['params']) == 0:
            ctx['arg_type'] = 'METH_NOARGS'
        else:
            ctx['arg_type'] = 'METH_VARARGS'
        app(TEMPLATE_METHOD % ctx)
    return TEMPLATE_METHODS % {
        'type_name': obj['name'],
        'methods': '\n'.join(methods)}

def write_module(obj):
    ret = []
    app = ret.append
    for i in obj['methods']:
        if i['cname'] in OVERRIDES:
            app(OVERRIDES[i['cname']]())
            continue
        mapping = dict(method_name=i['name'], obj_name=obj['name'])
        mapping['cname'] = i['cname']
        mapping['params'] = parse_args(obj, i)
        mapping['ret_var'] = parse_return(obj, i)
        mapping['ret'] = build_return(obj, i)
        arg_names = parse_arg_names(obj, i)
        if arg_names:
            arg_names = ', ' + arg_names
        mapping['param_names'] = arg_names
        app(TEMPLATE_METHOD_HEAD % mapping)
    return '\n'.join(ret)

def main(files):
    types = scanner.scan(files)
    print write_header(types)
    for i in types:
        print write_module(i)
        print write_constructor(i)
        print write_destructor(i)
        print write_methods(i)
        print write_type(i)
    print write_module_init('jarvis', types)

if __name__ == '__main__':
    if len(sys.argv) < 2:
        exit(usage())
    if sys.argv[1] in ('-h', '--help'):
        exit(usage())
    exit(main(sys.argv[1:]))
