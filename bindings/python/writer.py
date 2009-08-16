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

import simplejson
import sys
import scanner

# ---- Some definitions ----

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

RETURN_MAP = {
    'char *':        'Py_BuildValue ("s", ret)',
    'const char *':  'Py_BuildValue ("s", ret)',
    'int':           'Py_BuildValue ("i", ret)',
    'long':          'Py_BuildValue ("i", ret)',
    'iks *':         'Py_BuildValue ("O", PyIks_FromIks (ret))'
}

# ---- .h file templates ----

TEMPLATE_H_FILE_HEADER = '''
#ifndef _PY_%(libupper)s_H_
#define _PY_%(libupper)s_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <Python.h>
%(includes)s

#define Py%(libcamel)s_IMPORT \\
  Py%(libcamel)sAPI = \\
    (Py%(libcamel)s_CAPI*) PyCObject_Import ("%(lib)s", "%(lib)s_CAPI")
'''

TEMPLATE_H_MACROS = '''#define Py%(type)s_AS_%(type_upper)s(op) \\
  (((Py%(type)sObject *) op)->inner)

#define Py%(type)s_From%(type)s(o) \\
  Py%(libcamel)sAPI->Py%(type)s_From%(type)s(o, Py%(libcamel)sAPI->%(type)sType)
'''

TEMPLATE_H_TYPES = '''typedef struct {
  PyObject_HEAD
  %(cname)s *inner;
} %(pytype)s;
'''

TEMPLATE_H_CAPI = '''/* Define struct for the C API. */
typedef struct {
%(types)s
} Py%(libcamel)s_CAPI;
'''

TEMPLATE_H_FILE_FOOTER = '''static Py%(libcamel)s_CAPI *Py%(libcamel)sAPI;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _PY_%(libupper)s_H_ */
'''

# ---- .c file templates ----

TEMPLATE_C_CAPI = '''/* C API definition */
static Py%(libcamel)s_CAPI CAPI = {
%(definitions)s
};
'''

TEMPLATE_C_NEWFUNC = '''static PyObject *
%(proto)s (%(cname)s *ctype, PyTypeObject *type)
{
  %(pyname)s *self;
  self = (%(pyname)s *) (type->tp_alloc (type, 0));
  if (self != NULL)
    self->inner = ctype;
  return (PyObject *) self;
}
'''

TEMPLATE_C_CONSTRUCTOR = '''static int
%(pyname)s_init (%(pyname)s *self,
                 PyObject   *args,
                 PyObject   *kwargs)
{%(params)s
  self->inner = %(name)s (%(param_names)s);
  return 0;
}
'''

TEMPLATE_C_DESTRUCTOR = '''static void
%(pyname)s_dealloc (%(pyname)s *self)
{
  if (self->inner)
    %(name)s (self->inner);
  self->ob_type->tp_free ((PyObject *) self);
}
'''

TEMPLATE_C_METHOD = '''static PyObject *
%(pyname)s_%(name)s (%(pyname)s *self, PyObject *args)
{
  %(rtype)s ret;%(params)s
  ret = %(cname)s (self->inner%(param_names)s);
  return %(ret)s;
}
'''

TEMPLATE_C_METHOD_NORET = '''static PyObject *
%(pyname)s_%(name)s (%(pyname)s *self, PyObject *args)
{%(params)s
  %(cname)s (self->inner%(param_names)s);
  Py_INCREF (Py_None);
  return Py_None;
}
'''

TEMPLATE_C_METHOD_DEF = '''  { "%(name)s",
    (PyCFunction) %(pyname)s_%(name)s,
    %(arg)s, "%(doc)s" },
'''

TEMPLATE_C_METHOD_DEFS = '''
static PyMethodDef %(pyname)s_methods[] = {
%(methods)s
  { NULL }                        /* sentinel */
};
'''

TEMPLATE_C_TYPE = '''
static PyTypeObject %(type)s = {
  PyObject_HEAD_INIT(NULL)
  0,                                        /* ob_size */
  "%(name)s",                               /* tp_name */
  sizeof (%(pyname)s),                      /* tp_basicsize */
  0,                                        /* tp_itemsize */
  (destructor) %(pyname)s_dealloc,          /* tp_dealloc */
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
  "%(name)s Objects",                       /* tp_doc */
  0,                                        /* tp_traverse */
  0,                                        /* tp_clear */
  0,                                        /* tp_richcompare */
  0,                                        /* tp_weaklistoffset */
  0,                                        /* tp_iter */
  0,                                        /* tp_iternext */
  %(pyname)s_methods,                       /* tp_methods */
  0,                                        /* tp_members */
  0,                                        /* tp_getset */
  0,                                        /* tp_base */
  0,                                        /* tp_dict */
  0,                                        /* tp_descr_get */
  0,                                        /* tp_descr_set */
  0,                                        /* tp_dictoffset */
  (initproc) %(pyname)s_init,               /* tp_init */
  0,                                        /* tp_alloc */
  PyType_GenericNew,                        /* tp_new */
};
'''

TEMPLATE_C_MODULE_INIT = '''  if (PyType_Ready (&%(pytype)s) < 0)
    return;
  Py_INCREF (&%(pytype)s);
  PyModule_AddObject (m, "%(name)s", (PyObject *) &%(pytype)s);
'''

TEMPLATE_C_MODULE_INITFUNC = '''
#ifndef PyMODINIT_FUNC
#define PyMODINIT_FUNC void
#endif

PyMODINIT_FUNC
init%(mname)s (void)
{
  PyObject *m;
  PyObject *capi;
  PyIks_IMPORT;                 /* To use iksemel bindings */
  m = Py_InitModule3 ("%(mname)s", NULL, "%(doc)s");

  capi = PyCObject_FromVoidPtr (&CAPI, NULL);
  if (capi == NULL)
    return;
  PyModule_AddObject (m, "%(mname)s_CAPI", capi);

%(modules)s
}
'''

# ---- naming/helper stuff ----

def pyname(name):
    return 'Py%sObject' % name

def pytypename(name):
    return 'Py%sType' % name

def prototypename(name):
    return 'new_py_%s' % name.lower()

def macroname(name):
    return 'Py_%(name)s_From%(name)s' % {'name': name}

def macro_py2c(name):
    return '#define %(macro)s(o) %(proto)s (o, &%(type)s)' \
        % {'name': name.lower(),
           'macro': macroname(name),
           'proto': prototypename(name),
           'type': pytypename(name),
           }

class Helper(object):
    def __init__(self, defs):
        self.defs = defs
        self.lib = self.defs['name']
        self.libcamel = self.defs['name'].capitalize()
        self.libupper = self.defs['name'].upper()
        self.alltypes = []
        for i in self.defs['modules']:
            for j in i['types']:
                self.alltypes.append(j['cname'])

        self.dctx = {
            'lib': self.lib,
            'libcamel': self.libcamel,
            'libupper': self.libupper,
            'alltypes': self.alltypes,
            }

    def uctx(self, mydict):
        ctx = self.dctx.copy()
        ctx.update(mydict)
        return ctx

# ---- file generators ----

class HFile(Helper):
    def header(self):
        includes = []
        for module in self.defs['modules']:
            includes.append('#include <%s/%s.h>' % (
                    self.defs['name'], module['name']))
        for dep in self.defs['dependencies']['public']:
            includes.append('#include <%s>' % dep)
        return TEMPLATE_H_FILE_HEADER % self.uctx({
            'includes': '\n'.join(includes),
            })

    def macros(self):
        ret = []
        app = ret.append
        for module in self.defs['modules']:
            for ctype in module['types']:
                app(TEMPLATE_H_MACROS % self.uctx({
                            'type': ctype['name'].capitalize(),
                            'type_upper': ctype['name'].upper(),
                            }))
        return '\n'.join(ret)

    def types(self):
        ret = []
        app = ret.append
        for module in self.defs['modules']:
            for ctype in module['types']:
                app(TEMPLATE_H_TYPES % self.uctx({
                            'cname': ctype['cname'],
                            'pytype': pyname(ctype['name']),
                            }))
        return '\n'.join(ret)

    def capi(self):
        ret = []
        app = ret.append
        pytypes = []
        for module in self.defs['modules']:
            for ctype in module['types']:
                pytypes.append('  PyTypeObject *%s;' % pytypename(ctype['name']))
                pytypes.append('  PyObject *(*Py%(type)s_From%(type)s) '
                               '(%(ctype)s *, PyTypeObject *);' %
                               {'type': ctype['name'],
                                'ctype': ctype['cname']})
        app(TEMPLATE_H_CAPI % self.uctx({
                    'types': '\n'.join(pytypes),
                    }))
        return '\n'.join(ret)

    def footer(self):
        return TEMPLATE_H_FILE_FOOTER % self.dctx

    def __str__(self):
        parts = []
        parts.append(self.header())
        parts.append(self.macros())
        parts.append(self.types())
        parts.append(self.capi())
        parts.append(self.footer())
        return '\n'.join(parts)

class CFile(Helper):

    def name_from_cname(self, cname):
        nopointer = cname.replace('*', '').strip()
        for module in self.defs['modules']:
            for ctype in module['types']:
                if ctype['cname'] == nopointer:
                    return ctype['name']
        raise ValueError('CName %s not found.' % cname)

    def parse_args(self, obj, method):
        args = []
        app = args.append
        params = method['params']
        svars = []

        # We don't need to parse the `self' parameter
        if params[0]['type'] in (obj['cname'], '%s *' % obj['cname']):
            params = params[1:]

        if params:
            app('')

        # Here we're declaring variables that actually holds values
        # that will be passed to the method.
        types = ''
        for param in params:
            name = param['name']
            ptype = param['type']
            modifiers = param['modifiers']
            nopointer = ptype.replace('*', '').strip()
            if ptype == 'varargs':
                continue

            if nopointer in self.alltypes:
                app('  %s * %s;' % (pyname(self.name_from_cname(ptype)), name))
            else:
                app('  %s %s;' % (ptype, name))
            svars.append('&%s' % name)

            # Here we're discovering which params should be passed to
            # the function and building a string with needed arguments
            # with its types.
            pytype = TYPE_MAP.get(ptype, 'O') # Fallingback to `object'
            if '_optional_' in modifiers:
                types += '|' + pytype
            elif '_len_' in modifiers:
                types += '#'
            else:
                types += pytype

        # Calling python api function that parse arguments from the
        # args list.
        if params:
            app('  if (!PyArg_ParseTuple (args, "%s", %s))' % (
                    types, ', '.join(svars)))
            if method['name'] == 'new': # Handling constructor
                app('    return -1;')
            else:
                app('    return NULL;')
        return '\n'.join(args)

    def parse_arg_names(self, obj, method):
        ret = ''
        params = method['params']
        if params[0]['type'] in (obj['cname'], '%s *' % obj['cname']):
            params = params[1:]
            if params:
                ret = ', '

        newparams = []
        for param in params:
            name = param['name']
            ptype = param['type']
            modifiers = param['modifiers']
            nopointer = ptype.replace('*', '').strip()
            if ptype == 'varargs':
                continue
            if nopointer in self.alltypes:
                newparams.append('%s->inner' % name)
            else:
                newparams.append('%s' % name)
        return ret + ', '.join(newparams)

    def build_return(self, obj, method):
        rtype = method['rtype'];
        bval = RETURN_MAP.get(rtype)
        if bval:
            return bval
        for i in self.alltypes:
            if rtype == i + ' *':
                bval = 'Py_BuildValue ("O", %s (ret))' % \
                    macroname(self.name_from_cname(i))
        if bval:
            return bval
        return 'Py_BuildValue ("")'

    # ---- writting functions ----

    def header(self):
        parts = []
        app = parts.append
        app('#include <Python.h>')
        for dep in self.defs['dependencies']['public']:
            includes.append('#include <%s>' % dep)
        for dep in self.defs['dependencies']['priv']:
            app('#include <%s>' % dep)
        app('#include "%smodule.h"' % self.defs['name'])
        app('')
        return '\n'.join(parts)

    def macros(self):
        parts = []
        app = parts.append
        app('/* Useful macros for creating new pyobjects from our binded\n'
            ' * types. */')
        for module in self.defs['modules']:
            for ctype in module['types']:
                app(macro_py2c(ctype['name']))
        app('')
        return '\n'.join(parts)

    def types(self):
        parts = []
        app = parts.append
        app('/* Defining types to forward using. */')
        for module in self.defs['modules']:
            for ctype in module['types']:
                app('static PyTypeObject %s;' % pytypename(ctype['name']))
        app('')
        return '\n'.join(parts)

    def prototypes(self):
        parts = []
        app = parts.append
        app('/* Prototypes for python type creation */')
        for module in self.defs['modules']:
            for ctype in module['types']:
                app('static PyObject *%s (%s *ctype, PyTypeObject *type);' % (
                        prototypename(ctype['name']),
                        ctype['cname']))
        app('')
        return '\n'.join(parts)

    def capi(self):
        parts = []
        defs = []
        app = parts.append
        for module in self.defs['modules']:
            for ctype in module['types']:
                defs.append('  &%s' % pytypename(ctype['name']))
                defs.append('  %s' % prototypename(ctype['name']))
        app(TEMPLATE_C_CAPI % self.uctx({
                    'definitions': ',\n'.join(defs),
                    }))
        return '\n'.join(parts)

    def newfuncs(self):
        parts = []
        app = parts.append
        for module in self.defs['modules']:
            for ctype in module['types']:
                app(TEMPLATE_C_NEWFUNC % {
                        'proto': prototypename(ctype['name']),
                        'pyname': pyname(ctype['name']),
                        'cname': ctype['cname'],
                        })
        return '\n'.join(parts)

    def constructor(self, ctype):
        ctx = {'pyname': pyname(ctype['name']),
               'name': ctype['constructor']['cname'],
               'params': self.parse_args(ctype, ctype['constructor']),
               'param_names': self.parse_arg_names(ctype, ctype['constructor']),
               }
        return TEMPLATE_C_CONSTRUCTOR % ctx

    def destructor(self, ctype):
        ctx = {'pyname': pyname(ctype['name']),
               'name': ctype['destructor']['cname'],
               }
        return TEMPLATE_C_DESTRUCTOR % ctx

    def method(self, ctype, method):
        ctx = {'pyname': pyname(ctype['name']),
               'name': method['name'],
               'cname': method['cname'],
               'rtype': method['rtype'],
               'ret': self.build_return(ctype, method),
               'params': self.parse_args(ctype, method),
               'param_names': self.parse_arg_names(ctype, method),
               }
        if method['rtype'] != 'void':
            return TEMPLATE_C_METHOD % ctx
        else:
            return TEMPLATE_C_METHOD_NORET % ctx

    def method_defs(self, ctype):
        methods = []
        for method in ctype['methods']:
            if method['params']:
                arg = 'METH_VARARGS'
            else:
                arg = 'METH_NOARGS'
            methods.append(TEMPLATE_C_METHOD_DEF % {
                    'name': method['name'],
                    'pyname': pyname(ctype['name']),
                    'arg': arg,
                    'doc': method.get('doc', ''),
                    })
        return TEMPLATE_C_METHOD_DEFS % {
            'pyname': pyname(ctype['name']),
            'methods': '\n'.join(methods),
            }

    def pytype(self, ctype):
        return TEMPLATE_C_TYPE % {
            'name': ctype['name'],
            'pyname': pyname(ctype['name']),
            'type': pytypename(ctype['name']),
            }

    def modinit(self):
        ctypes = []
        for module in self.defs['modules']:
            for ctype in module['types']:
                ctypes.append(TEMPLATE_C_MODULE_INIT % {
                        'name': ctype['name'],
                        'pytype': pytypename(ctype['name'])
                        })
        return TEMPLATE_C_MODULE_INITFUNC % {
            'mname': self.lib,
            'modules': '\n'.join(ctypes),
            'doc': self.defs.get('doc', ''),
            }

    def methods(self):
        parts = []
        app = parts.append
        for module in self.defs['modules']:
            for ctype in module['types']:
                app(self.constructor(ctype))
                for method in ctype['methods']:
                    app(self.method(ctype, method))
                app(self.destructor(ctype))
                app(self.method_defs(ctype))
                app(self.pytype(ctype))
        return '\n'.join(parts)

    def __str__(self):
        parts = []
        parts.append(self.header())
        parts.append(self.macros())
        parts.append(self.types())
        parts.append(self.prototypes())
        parts.append(self.capi())
        parts.append(self.newfuncs())
        parts.append(self.methods())
        parts.append(self.modinit())
        return '\n'.join(parts)

def main(args):
    defs = simplejson.loads(open(args[1]).read())
    open('%smodule.h' % defs['name'], 'w').write(str(HFile(defs)))
    open('%smodule.c' % defs['name'], 'w').write(str(CFile(defs)))

if __name__ == '__main__':
    main(sys.argv)
