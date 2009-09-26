# -*- coding: utf-8; -*-
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

import simplejson
import sys
import warnings
import scanner
from overrides import OVERRIDES

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
    'void *':        'Py_BuildValue ("O", ret ? (PyObject *) ret : Py_None)',
    'char *':        'Py_BuildValue ("s", ret)',
    'const char *':  'Py_BuildValue ("s", ret)',
    'int':           'Py_BuildValue ("i", ret)',
    'long':          'Py_BuildValue ("i", ret)',
    'iks *':         'Py_BuildValue ("O", ret ? PyIks_FromIks (ret) : Py_None)',
}

LOCALDEP_MAP = {
    'atom': ['error', 'iri'],
    'error': [],
    'iri': ['error'],
    'log': ['error'],
    'pubsub': ['error', 'log'],
    'xmpp': ['error', 'log'],
}

# List that holds modules that should not be built. The first module
# that we don't want to build is the `list' module, we'll prefer to
# use python list type, that is more robust than anything we'll
# implement here.
BLACKLISTEDMODULES = {
    'list': 'pylist_from_tlist',
}

# ---- .h file templates ----

TEMPLATE_H_FILE_HEADER = '''
#ifndef _PY_%(mnameupper)s_H_
#define _PY_%(mnameupper)s_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <Python.h>
%(includes)s

#ifndef Py%(mnamecamel)s_BUILD_CORE

#define Py%(mnamecamel)s_IMPORT \\
  Py%(mnamecamel)sAPI = \\
    (Py%(mnamecamel)s_CAPI*) PyCObject_Import ("%(lib)s.%(mname)s", "_C_API")

#endif /* Py%(mnamecamel)s_BUILD_CORE */
'''

TEMPLATE_H_MACROS = '''/* Useful macros for %(type)s python type */

#define Py%(type)s_AS_%(type_upper)s(op) \\
  (((Py%(type)sObject *) op)->inner)

#ifndef Py%(mnamecamel)s_BUILD_CORE

#define Py_%(type)s_From%(type)s(o) \\
  Py%(mnamecamel)sAPI->Py%(type)s_From%(type)s(o, Py%(mnamecamel)sAPI->Py%(type)sType)

#define Py%(type)sObject_Check(op) \\
  PyObject_TypeCheck(op, Py%(mnamecamel)sAPI->Py%(type)sType)

#else

#define Py%(type)s_From%(type)s(o) \\
  Py%(mnamecamel)sAPI->Py%(type)s_From%(type)s(o, &Py%(type)sType)

#define Py%(type)sObject_Check(op) \\
  PyObject_TypeCheck(op, &Py%(type)sType)

#endif /* Py%(mnamecamel)s_BUILD_CORE */
'''

TEMPLATE_H_TYPES = '''typedef struct {
  PyObject_HEAD
  %(cname)s *inner;%(extra)s
} %(pytype)s;
'''

TEMPLATE_H_CAPI = '''/* Define struct for the C API. */
typedef struct {
%(types)s
} Py%(mnamecamel)s_CAPI;
'''

TEMPLATE_H_FILE_FOOTER = '''#ifndef Py%(mnamecamel)s_BUILD_CORE
static Py%(mnamecamel)s_CAPI *Py%(mnamecamel)sAPI;
#endif /* Py%(mnamecamel)s_BUILD_CORE*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _PY_%(mnameupper)s_H_ */
'''

# ---- .c file templates ----

TEMPLATE_C_MACROS = '''
/* Macros used to make it easy to generate code. */

#define INIT_MEMBER(x)         \\
  Py_INCREF (Py_None);         \\
  x = Py_None;
'''

TEMPLATE_C_CAPI = '''/* C API definition */
static Py%(mnamecamel)s_CAPI CAPI = {
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
  Py_XINCREF (self);
  return (PyObject *) self;
}
'''

TEMPLATE_C_CONSTRUCTOR = '''static int
%(pyname)s_init (%(pyname)s *self,
                 PyObject   *args,
                 PyObject   *kwargs)
{%(params)s
  self->inner = %(name)s (%(param_names)s);
  if (self->inner == NULL)
    {
      PyErr_SetString (PyExc_ValueError,
                       "Probable error in parameters");
      return -1;
    }
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
  %(rtype)s ret;
%(params)s
  ret = %(cname)s (%(param_names)s);
  return %(ret)s;
}
'''

TEMPLATE_C_METHOD_RET_DATE = '''static PyObject *
%(pyname)s_%(name)s (%(pyname)s *self, PyObject *args)
{
  struct tm *ctm;
  PyObject *date;
  %(rtype)s ret;%(params)s

  ret = %(cname)s (%(param_names)s);
  if (ret)
    {
      ctm = localtime (&ret);
      date = PyDateTime_FromDateAndTime (1900 + ctm->tm_year, ctm->tm_mon + 1,
                                         ctm->tm_mday, ctm->tm_hour,
                                         ctm->tm_min, ctm->tm_sec, 0);
      Py_INCREF (date);
      return date;
    }
  Py_INCREF (Py_None);
  return Py_None;
}
'''

TEMPLATE_C_METHOD_NORET = '''static PyObject *
%(pyname)s_%(name)s (%(pyname)s *self, PyObject *args)
{%(params)s
  %(cname)s (%(param_names)s);
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

TEMPLATE_C_CHEKC_DATE = '''
  if (!PyDateTime_Check (%s))
    {
      PyErr_SetString (PyExc_TypeError,
                       "param 1 must be a datetime.datetime instance.");
      return NULL;
    }
'''

TEMPLATE_C_PARAM_DATE = '''
  PyObject *%(name)s;
  time_t _time_%(name)s;

  if (!PyArg_ParseTuple (args, "O", &%(name)s))
    return NULL;%(check)s
  if (%(name)s == Py_None)
    _time_%(name)s = 0;
  else
    {
      struct tm mtm;
      mtm.tm_sec = PyDateTime_DATE_GET_SECOND (%(name)s);
      mtm.tm_min = PyDateTime_DATE_GET_MINUTE (%(name)s);
      mtm.tm_hour = PyDateTime_DATE_GET_HOUR (%(name)s);
      mtm.tm_mday = PyDateTime_GET_DAY (%(name)s);
      mtm.tm_mon = PyDateTime_GET_MONTH (%(name)s) - 1;      /* Month starts from 0 */
      mtm.tm_year = PyDateTime_GET_YEAR (%(name)s) - 1900;   /* see `man mktime'    */
      mtm.tm_isdst = -1;
      mtm.tm_gmtoff = 1;
      if ((_time_%(name)s = mktime (&mtm)) == -1)
        {
          PyErr_SetString (PyExc_ValueError, "Invalid date");
          return NULL;
        }
    }
'''

# This part of code was writen to generate objects that support
# "cyclic garbage collection". I've followed the example available in
# the 2.1.3 section of the "Extending and Embeding the Python
# Interpreter" tutorial.

TEMPLATE_C_CYCLIC_GC = '''
static int
%(pyname)s_traverse (%(pyname)s *self, visitproc visit, void *arg)
{
%(traverse_members)s
  return 0;
}

static int
%(pyname)s_clear (%(pyname)s *self)
{
%(clear_members)s
  return 0;
}

static void
%(pyname)s_dealloc (%(pyname)s *self)
{
  %(pyname)s_clear (self);
  self->ob_type->tp_free ((PyObject *) self);
}

static PyObject *
%(pyname)s_new (PyTypeObject *type, PyObject *args, PyObject *kwargs)
{
  %(pyname)s *self;

  self = (%(pyname)s *) type->tp_alloc (type, 0);
  if (self != NULL)
    {
%(new_members)s
    }
  return (PyObject *) self;
}

static struct PyMemberDef %(pyname)s_members[] = {
%(members_def)s
  { NULL }
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
  %(tp_flags)s,                             /* tp_flags */
  "%(name)s Objects",                       /* tp_doc */
  %(tp_traverse)s,                          /* tp_traverse */
  %(tp_clear)s,                             /* tp_clear */
  0,                                        /* tp_richcompare */
  0,                                        /* tp_weaklistoffset */
  0,                                        /* tp_iter */
  0,                                        /* tp_iternext */
  %(pyname)s_methods,                       /* tp_methods */
  %(tp_members)s,                           /* tp_members */
  0,                                        /* tp_getset */
  0,                                        /* tp_base */
  0,                                        /* tp_dict */
  0,                                        /* tp_descr_get */
  0,                                        /* tp_descr_set */
  0,                                        /* tp_dictoffset */
  %(tp_init)s,                              /* tp_init */
  0,                                        /* tp_alloc */
  %(tp_new)s,                               /* tp_new */
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

  PyEval_InitThreads ();

  PyIks_IMPORT;                 /* To use iksemel bindings */
  PyDateTime_IMPORT;

  /* Library imports */

%(imports)s

  m = Py_InitModule3 ("%(mname)s", NULL, "%(doc)s");

  capi = PyCObject_FromVoidPtr (&CAPI, NULL);
  if (capi == NULL)
    return;
  PyModule_AddObject (m, "_C_API", capi);

  /* Enums */

%(enums)s

  /* Module declaration */

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

def underscore_to_camel(name):
    nname = ''
    nsize = len(name)
    i = 0
    while i < nsize:
        if name[i] == '_':
            i += 1
            nname += name[i].upper()
        else:
            nname += name[i]
        i += 1
    return nname

class Helper(object):
    cyclegctypes = []

    def __init__(self, defs, module):
        self.defs = defs
        self.module = module
        self.lib = self.defs['name']
        self.libcamel = self.defs['name'].capitalize()
        self.libupper = self.defs['name'].upper()
        self.alltypes = []
        self.allenums = []

        if module:
            self.mname = module['name']
            self.mnameupper = self.mname.upper()
            self.mnamecamel = self.mname.capitalize()

        for i in self.defs['modules']:
            for e in i['enums']:
                self.allenums.append(e['name'])
            for j in i['types']:
                self.alltypes.append(j['cname'])

        self.dctx = {
            'lib': self.lib,
            'libcamel': self.libcamel,
            'libupper': self.libupper,
            'mname': self.mname,
            'mnamecamel': self.mnamecamel,
            'mnameupper': self.mnameupper,
            'allenums': self.allenums,
            'alltypes': self.alltypes,
            }

    def iscgctype(self, tname):
        for i in self.cyclegctypes:
            if i['type'] == tname:
                return True

    def uctx(self, mydict):
        ctx = self.dctx.copy()
        ctx.update(mydict)
        return ctx

    def name_from_cname(self, cname):
        nopointer = cname.replace('*', '').strip()
        for module in self.defs['modules']:
            for ctype in module['types']:
                if ctype['cname'] == nopointer:
                    return ctype['name']
        raise ValueError('CName %s not found.' % cname)

# ---- file generators ----

class Header(Helper):

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
        for ctype in self.module['types']:
            app(TEMPLATE_H_MACROS % self.uctx({
                        'type': self.name_from_cname(ctype['cname']),
                        'type_upper': ctype['name'].upper(),
                        }))
        return '\n'.join(ret)

    def types(self):
        ret = []
        app = ret.append
        for ctype in self.module['types']:
            extra = []
            pname = pyname(ctype['name'])

            # Overriding a type means add more items to its
            # struct. Currently, only PyObjects can be added. Because
            # the overriding function right now returns a list of item
            # names to be added to the struct.
            #
            # The idea here is simple. Since we have a PyObject* field
            # in the type struct, this type should support "Cyclic
            # GC", so a new entry in `self.cyclegctypes' that will be
            # readed in the type generation function. This new entry
            # contains the customized items. It is importanto to
            # generate _new and _clean functions.
            if pname in OVERRIDES:
                subitems = OVERRIDES[pname]()
                if subitems:
                    self.cyclegctypes.append({
                            'type': pname,
                            'childs': subitems,
                            })
                for sitem in subitems:
                    extra.append('  PyObject *%s;' % sitem)

            # Just to make generated code better
            if extra:
                extra.insert(0, '\n')

            # Just filling the type template. All bizarre cyclic gc
            # stuff ends right above here!
            app(TEMPLATE_H_TYPES % self.uctx({
                        'cname': ctype['cname'],
                        'pytype': pname,
                        'extra': '\n'.join(extra),
                        }))
        return '\n'.join(ret)

    def capi(self):
        ret = []
        app = ret.append
        pytypes = []
        for ctype in self.module['types']:
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

class Module(Helper):

    def parse_args(self, obj, method):
        args = []
        app = args.append
        params = method['params']
        svars = []
        tests = []

        # We don't need to parse the `self' parameter
        if params and params[0]['type'] in \
                (obj['cname'], '%s *' % obj['cname']):
            params = params[1:]

        if params:
            app('')

        # Here we're declaring variables that actually holds values
        # that will be passed to the method.
        types = ''
        increfs = []
        for param in params:
            name = param['name']
            ptype = param['type']
            modifiers = param['modifiers']
            nopointer = ptype.replace('*', '').strip()
            if ptype == 'varargs':
                continue

            if nopointer in self.alltypes:
                pname = pyname(self.name_from_cname(ptype))
                app('  %s * %s = NULL;' % (pname, name))
                increfs.append(name)
                tests.append((pname, name))

            # FIXME: hardcoded things should be avoided..
            elif nopointer == 'iks':
                app('  PyIksObject * %s = NULL;' % name)
                increfs.append(name)
            elif nopointer == 'time_t':
                check = ''
                if not 'nullable' in modifiers:
                    check = TEMPLATE_C_CHEKC_DATE % name
                app(TEMPLATE_C_PARAM_DATE % {
                        'name': name,
                        'check': check
                        })
                return '\n'.join(args)
            else:
                # it is a pointer... so it should be initialized
                # before used.
                if ptype.endswith('*'):
                    app('  %s %s = NULL;' % (ptype, name))
                elif ptype == 'int':
                    app('  %s %s = 0;' % (ptype, name))
                else:
                    app('  %s %s;' % (ptype, name))
            svars.append('&%s' % name)

            # Here we're discovering which params should be passed to
            # the function and building a string with needed arguments
            # with its types.
            pytype = TYPE_MAP.get(ptype, 'O') # Fallingback to `object'

            # If the current type is found in our enum registry, we
            # know that our param will receive an int.
            if ptype in self.allenums:
                pytype = 'i'

            if 'optional' in modifiers:
                # Python optional flag is not per parameter, if a
                # parameter is optional in a function, all of the next
                # params are going to be too. So, only one `|'
                # (optional marker) should be added.
                if '|' in types:
                    types += pytype
                else:
                    types += '|' + pytype
            elif 'len' in modifiers:
                types += '#'
            elif 'out' in modifiers:
                # We're not handling this kind of modifier yet, we're
                # writting overrides for them. we actually have just a
                # few placess with it, so don't care about it now...
                continue
            else:
                types += pytype

        # Calling python api function that parse arguments from the
        # args list.
        pnames = ''
        if svars:
            pnames = ', ' + ', '.join(svars)

        app('  if (!PyArg_ParseTuple (args, "%s"%s))' % (
                types, pnames))
        if method['name'] == 'new': # Handling constructor
            app('    return -1;')
        else:
            app('    return NULL;')

        # Testing for known type parameters
        for pname, name in tests:
            app('  if (%s && !%s_Check (%s))' % (name, pname, name))
            app('    {')
            app('      PyErr_SetString(PyExc_TypeError,')
            app('        "param 1 must be a %s instance.");' % pname)
            if method['name'] == 'new':
                app('      return -1;')
            else:
                app('      return NULL;')
            app('    }')

        for i in increfs:
            app('  Py_XINCREF (%s);' % i)

        return '\n'.join(args)

    def parse_arg_names(self, obj, method):
        params = method['params']
        newparams = []
        if params and params[0]['type'] in \
                (obj['cname'], '%s *' % obj['cname']):
            newparams.append('self->inner')
            params = params[1:]

        # Shortcuts that are going to be used in the loop bellow.
        optional = lambda x: '%s ? %s->inner : NULL' % (x, x)
        required = lambda x: '%s->inner' % x

        for param in params:
            name = param['name']
            ptype = param['type']
            modifiers = param['modifiers']
            nopointer = ptype.replace('*', '').strip()
            if ptype == 'varargs':
                varargs = 1
                continue
            if nopointer in self.alltypes:
                if 'optional' in modifiers:
                    newparams.append(optional(name))
                else:
                    newparams.append(required(name))
            elif nopointer == 'time_t':
                newparams.append('_time_%s' % name)
            elif nopointer == 'iks':
                if 'optional' in modifiers:
                    newparams.append(optional(name))
                else:
                    newparams.append(required(name))
            else:
                newparams.append('%s' % name)
        return ', '.join(newparams)

    def build_return(self, obj, method):
        rtype = method['rtype'];
        bval = RETURN_MAP.get(rtype)
        if bval:
            return bval

        # Looking for our special list type
        if rtype == 'ta_list_t *':
            fname = prototypename(obj['name'])
            subtype = method['return']['subtype'].replace('ta_', '')
            typename = pytypename(subtype)
            return 'Py_BuildValue ("O", t_list_to_py_list (ret, ' \
                '(TListConvertFunc) %s, &%s))' % (fname, typename)

        # Looking in our registered types for an already known one
        for i in self.alltypes:
            known = False
            if rtype in (i + ' *', 'const ' + i + '*'):
                bval = 'ret ? Py_BuildValue ("O", %s ((%s *) ret)) : ' \
                       'Py_BuildValue ("O", Py_None)' % \
                    (macroname(self.name_from_cname(i)), i)

        # Now is the time to look in our enum registry.
        for i in self.allenums:
            if rtype == i:
                bval = 'Py_BuildValue ("i", ret)'
        if bval:
            return bval
        return 'Py_BuildValue ("")'

    # ---- writting functions ----

    def header(self):
        parts = []
        app = parts.append
        app('#include <Python.h>')
        app('#include <structmember.h>')
        app('#include <time.h>')
        app('#include "listconverter.h"')
        for dep in self.defs['dependencies']['public']:
            app.append('#include <%s>' % dep)
        for dep in self.defs['dependencies']['priv']:
            app('#include <%s>' % dep)

        app('#include "%smodule.h"' % self.module['name'])
        for dep in LOCALDEP_MAP.get(self.module['name'], []):
            app('#include "%smodule.h"' % dep)
        app(TEMPLATE_C_MACROS)
        return '\n'.join(parts)

    def macros(self):
        parts = []
        app = parts.append
        app('/* Useful macros for creating new pyobjects from our binded\n'
            ' * types. */')
        for ctype in self.module['types']:
            app(macro_py2c(ctype['name']))
        app('')
        return '\n'.join(parts)

    def prototypes(self):
        parts = []
        app = parts.append
        app('/* Prototypes for python type creation */')
        for ctype in self.module['types']:
            app('static PyObject *%s (%s *ctype, PyTypeObject *type);' % (
                    prototypename(ctype['name']),
                    ctype['cname']))
        app('')
        return '\n'.join(parts)

    def newfuncs(self):
        parts = []
        app = parts.append
        for ctype in self.module['types']:
            app(TEMPLATE_C_NEWFUNC % {
                    'proto': prototypename(ctype['name']),
                    'pyname': pyname(ctype['name']),
                    'cname': ctype['cname'],
                    })
        return '\n'.join(parts)

    def types(self):
        parts = []
        app = parts.append
        app('/* Defining types to forward using. */')
        for ctype in self.module['types']:
            app('static PyTypeObject %s;' % pytypename(ctype['name']))
        app('')
        return '\n'.join(parts)

    def constructor(self, ctype):
        try:
            cname = ctype['constructor']['cname']
        except KeyError:
            warnings.warn('Type %s has no constructor' % ctype['name'])
            return ''
        if cname in OVERRIDES:
            return OVERRIDES[cname]()
        ctx = {'pyname': pyname(ctype['name']),
               'name': ctype['constructor']['cname'],
               'params': self.parse_args(ctype, ctype['constructor']),
               'param_names': self.parse_arg_names(ctype, ctype['constructor']),
               }
        return TEMPLATE_C_CONSTRUCTOR % ctx

    def destructor(self, ctype):
        # We should not build the constructor of an object that
        # implements cgc, so aborting.
        if self.iscgctype(pyname(ctype['name'])):
            return ''
        try:
            cname = ctype['constructor']['cname']
        except KeyError:
            warnings.warn('Type %s has no destructor' % ctype['name'])
            return ''
        if cname in OVERRIDES:
            return OVERRIDES[cname]()
        ctx = {'pyname': pyname(ctype['name']),
               'name': ctype['destructor']['cname'],
               }
        return TEMPLATE_C_DESTRUCTOR % ctx

    def method(self, ctype, method):
        cname = method['cname']
        if cname in OVERRIDES:
            return OVERRIDES[cname]()
        ctx = {'pyname': pyname(ctype['name']),
               'name': method['name'],
               'cname': method['cname'],
               'rtype': method['rtype'],
               'ret': self.build_return(ctype, method),
               'params': self.parse_args(ctype, method),
               'param_names': self.parse_arg_names(ctype, method),
               }
        if method['rtype'] == 'void':
            return TEMPLATE_C_METHOD_NORET % ctx
        elif method['rtype'] == 'time_t':
            return TEMPLATE_C_METHOD_RET_DATE % ctx
        else:
            return TEMPLATE_C_METHOD % ctx

    def method_defs(self, ctype):
        methods = []
        for method in ctype['methods']:
            # If a false value (actually a blank string) is returned
            # from the OVERRIDE dict, the method will not be added to
            # the method list.
            cname = method['cname']
            if cname in OVERRIDES and not OVERRIDES[cname]():
                continue
            if method['params']:
                arg = 'METH_VARARGS'
            else:
                arg = 'METH_NOARGS'
            methods.append(TEMPLATE_C_METHOD_DEF % {
                    'name': method['name'],
                    'pyname': pyname(ctype['name']),
                    'arg': arg,
                    'doc': method.get('doc', '').replace('\n', '\\n'),
                    })
        return TEMPLATE_C_METHOD_DEFS % {
            'pyname': pyname(ctype['name']),
            'methods': '\n'.join(methods),
            }

    def cgc(self, ctype):
        cpyname = pyname(ctype['name'])

        # We should not generate cgc code for types that doesn't
        # actually need it.
        if not self.iscgctype(cpyname):
            return ''

        # Since we reached this point, we are sure that there *is* an
        # entry called `pyname(ctype['name'])' in the OVERRIDES dict.
        members = OVERRIDES[cpyname]()

        # Building entries to be put in _clear method.
        new_members = []
        traverse_members = []
        clear_members = []
        members_def = []
        for i in members:
            new_members.append('      INIT_MEMBER (self->%s);' % i)
            traverse_members.append('  Py_VISIT (self->%s);' % i)
            clear_members.append('  Py_CLEAR (self->%s);' % i)
            members_def.append('  {"%s", T_OBJECT_EX, offsetof (%s, %s), 0, ""},'
                               % (i, cpyname, i))

        return TEMPLATE_C_CYCLIC_GC % {
            'pyname': pyname(ctype['name']),
            'new_members': '\n'.join(new_members),
            'traverse_members': '\n'.join(traverse_members),
            'clear_members': '\n'.join(clear_members),
            'members_def': '\n'.join(members_def),
            }

    def pytype(self, ctype):
        tp_flags = ['Py_TPFLAGS_DEFAULT', 'Py_TPFLAGS_BASETYPE']
        tp_traverse = '0'
        tp_clear = '0'
        tp_new = 'PyType_GenericNew'
        tp_members = '0'
        tp_init = '(initproc) %s_init' % pyname(ctype['name'])

        # Here we're looking for types that need to implement cyclic
        # garbage collection. To do it, some slots in the type
        # definition must be implemented, like traverse, clear and new
        # methods.
        if self.iscgctype(pyname(ctype['name'])):
            cpyname = pyname(ctype['name'])
            tp_flags.append('Py_TPFLAGS_HAVE_GC')
            tp_traverse = '(traverseproc) %s_traverse' % cpyname
            tp_clear = '(inquiry) %s_clear' % cpyname
            tp_new = '%s_new' % cpyname
            tp_members = '%s_members' % cpyname

        mname = self.module['name']
        name = ctype['name']
        msize = len(mname)
        if len(name) > msize:
            pymodname = name[msize:]
        else:
            pymodname = name

        return TEMPLATE_C_TYPE % {
            'name': '%s.%s.%s' % (self.lib, mname,
                                  underscore_to_camel(pymodname)),
            'pyname': pyname(ctype['name']),
            'type': pytypename(ctype['name']),
            'tp_flags': ' | ' .join(tp_flags),
            'tp_traverse': tp_traverse,
            'tp_clear': tp_clear,
            'tp_new': tp_new,
            'tp_init': tp_init,
            'tp_members': tp_members,
            }

    def methods(self):
        parts = []
        app = parts.append
        for ctype in self.module['types']:
            app(self.constructor(ctype))
            for method in ctype['methods']:
                app(self.method(ctype, method))
            app(self.destructor(ctype))
            app(self.method_defs(ctype))
            app(self.cgc(ctype))
            app(self.pytype(ctype))
        return '\n'.join(parts)

    def enums(self, module):
        enums = []
        for enum in module['enums']:
            for entry in enum['entries']:
                enums.append('  PyModule_AddIntConstant (m, "%s", %s);' % (
                        entry, entry))
        return '\n'.join(enums)

    def capi(self):
        parts = []
        defs = []
        app = parts.append
        for ctype in self.module['types']:
                defs.append('  &%s' % pytypename(ctype['name']))
                defs.append('  %s' % prototypename(ctype['name']))
        app(TEMPLATE_C_CAPI % self.uctx({
                    'definitions': ',\n'.join(defs),
                    }))
        return '\n'.join(parts)

    def modinit(self):
        ctypes = []
        module = self.module
        mname  = module['name']
        msize = len(mname)
        args = (mname, mname)

        for ctype in module['types']:
            name = ctype['name']
            if len(name) > msize:
                pymodname = name[msize:]
            else:
                pymodname = name
            ctypes.append(TEMPLATE_C_MODULE_INIT % {
                    'mname': module['name'].lower(),
                    'name': underscore_to_camel(pymodname),
                    'pytype': pytypename(ctype['name'])
                    })

        imports = []
        for dep in LOCALDEP_MAP.get(module['name'], []):
            imports.append('  Py%s_IMPORT;' % dep.capitalize())
        return TEMPLATE_C_MODULE_INITFUNC % self.uctx({
                'imports': '\n'.join(imports),
                'enums': self.enums(module),
                'modules': '\n'.join(ctypes),
                'doc': module.get('doc', '').replace('\n', '\\n'),
                })

    def __str__(self):
        parts = []
        parts.append(self.header())
        parts.append(self.macros())
        parts.append(self.prototypes())
        parts.append(self.newfuncs())
        parts.append(self.types())
        parts.append(self.methods())
        parts.append(self.capi())
        parts.append(self.modinit())
        return '\n'.join(parts)

class HFile(Helper):

    def __str__(self):
        parts = []
        #parts.append(self.header())
        #parts.append(self.capi())
        for module in self.defs['modules']:
            parts.append(str(Header(self.defs, module)))
        #parts.append(self.footer())
        return '\n'.join(parts)

class CFile(Helper):

    def __str__(self):
        parts = []
        for module in self.defs['modules']:
            parts.append(str(Module(self.defs, module)))
        return '\n'.join(parts)

def main(args):
    defs = simplejson.loads(open(args[1]).read())
    for module in defs['modules']:
        if module['name'] not in BLACKLISTEDMODULES:
            open('%smodule.h' % module['name'], 'w').write(str(Header(defs, module)))
            open('%smodule.c' % module['name'], 'w').write(str(Module(defs, module)))

if __name__ == '__main__':
    main(sys.argv)
