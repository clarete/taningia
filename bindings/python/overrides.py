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

def t_atom_entry_get_authors():
    return '''
static PyObject *
PyAtomEntryObject_get_authors (PyAtomEntryObject *self,
                               PyObject          *args)
{
  PyObject *alist;
  TAtomPerson **authors;
  int len, i;
  t_atom_entry_get_authors (self->inner, &authors, &len);

  alist = PyList_New (len);
  if (!alist)
    return NULL;

  for (i = 0; i < len; i++)
    {
      PyObject *author;
      author = Py_AtomPerson_FromAtomPerson (authors[i]);
      Py_INCREF (author);
      PyList_SetItem (alist, i, author);
    }

  Py_INCREF (alist);
  return alist;
}
'''

def t_atom_entry_get_categories():
    return '''
static PyObject *
PyAtomEntryObject_get_categories (PyAtomEntryObject *self,
                                  PyObject          *args)
{
  PyObject *alist;
  TAtomCategory **categories;
  int len, i;
  t_atom_entry_get_categories (self->inner, &categories, &len);

  alist = PyList_New (len);
  if (!alist)
    return NULL;

  for (i = 0; i < len; i++)
    {
      PyObject *category;
      category = Py_AtomCategory_FromAtomCategory (categories[i]);
      Py_INCREF (category);
      PyList_SetItem (alist, i, category);
    }

  Py_INCREF (alist);
  return alist;
}
'''

def pyfilterobject():
    return ['callback', 'param']

def t_filter_add():
    return '''
static void
_call_py_filter_callback (void *data1, void *data2, void *data3)
{
  PyObject *args, *result;
  PyFilterObject *self;
  PyObject *p1, *p2, *p3;
  PyGILState_STATE state;
  void *tmp;

  state = PyGILState_Ensure ();

  /* The filter object is coming as a parameter to our callback */
  self = (PyFilterObject *) data2;

  /* Packing parameters in a tuple to pass to our callback */
  tmp = t_filter_get_data (self->inner);
  if (tmp)
    {
      p1 = Py_Xmpp_FromXmpp (tmp);
      Py_INCREF (p1);
    }
  else
    {
      Py_INCREF (Py_None);
      p1 = Py_None;
    }
  p2 = self->param;
  if (p2)
    Py_INCREF (p2);
  else
    {
      Py_INCREF (Py_None);
      p2 = Py_None;
    }
  if (data3)
    {
      p3 = PyIks_FromIks ((iks *)data3);
      Py_XINCREF (p3);
    }
  else
    {
      Py_INCREF (Py_None);
      p3 = Py_None;
    }

  args = Py_BuildValue ("(OOO)", p1, p2, p3);
  result = PyObject_CallObject (self->callback, args);
  Py_DECREF (args);

  if (result == NULL)
    {
      PyErr_Print ();
      PyErr_Clear ();
    }
  else
    Py_DECREF (result);

  PyGILState_Release (state);
}

static PyObject *
PyFilterObject_add (PyFilterObject *self,
                    PyObject       *args)
{
  const char *name = NULL;
  PyObject *callback = NULL;
  PyObject *param = NULL;
  if (!PyArg_ParseTuple (args, "sO|O:add", &name, &callback, &param))
    return NULL;
  if (!PyCallable_Check (callback))
    {
      PyErr_SetString (PyExc_TypeError, "Param 1 must be callable.");
      return NULL;
    }
  else
    {
      Py_INCREF (callback);
      self->callback = callback;
    }
  if (param != NULL)
    {
      Py_INCREF (param);
      self->param = param;
    }
  else
    {
      Py_INCREF (Py_None);
      self->param = Py_None;
    }
  Py_INCREF (self);
  t_filter_add (self->inner, name, (TFilterCallback) _call_py_filter_callback, self);

  Py_INCREF (Py_None);
  return Py_None;
}
'''

def t_filter_call():
    return '''
static PyObject *
PyFilterObject_call (PyFilterObject *self,
                     PyObject       *args)
{
  const char *name = NULL;
  PyObject *extra = NULL;

  if (!PyArg_ParseTuple (args, "s|O", &name, &extra))
    return NULL;

  Py_INCREF (extra);

  Py_BEGIN_ALLOW_THREADS

  t_filter_call (self->inner, name, extra);

  Py_END_ALLOW_THREADS

  Py_INCREF (Py_None);
  return Py_None;
}
'''

def pylogobject():
    return ['handler_callback', 'handler_data']

def t_log_set_handler():
    return '''
static void
_call_py_log_handler (TLog *log, TLogLevel level, const char *msg, void *user_data)
{
  PyLogObject *self;
  PyObject *args;
  PyObject *result;
  self = (PyLogObject *) user_data;
  args = Py_BuildValue ("(OisO)", self, level, msg, self->handler_data);
  result = PyObject_CallObject (self->handler_callback, args);
  Py_DECREF (args);
  if (result == NULL)
    PyErr_Print ();
  else
    Py_DECREF (result);
}

static PyObject *
PyLogObject_set_handler (PyLogObject *self,
                         PyObject    *args)
{
  PyObject *callback = NULL;
  PyObject *param = NULL;
  if (!PyArg_ParseTuple (args, "O|O", &callback, &param))
    return NULL;
  if (!PyCallable_Check (callback))
    {
      PyErr_SetString (PyExc_TypeError, "Param 1 must be callable.");
      return NULL;
    }
  else
    {
      Py_INCREF (callback);
      self->handler_callback = callback;
    }
  if (param != NULL)
    {
      Py_INCREF (param);
      self->handler_data = param;
    }
  else
    {
      Py_INCREF (Py_None);
      self->handler_data = Py_None;
    }
  Py_INCREF (self);
  t_log_set_handler (self->inner, (TLogHandlerFunc) _call_py_log_handler, self);

  Py_INCREF (Py_None);
  return Py_None;
}
'''

def t_atom_feed_get_authors():
    return '''
static PyObject *
PyAtomFeedObject_get_authors (PyAtomFeedObject *self,
                              PyObject         *args)
{
  PyObject *alist;
  TAtomPerson **authors;
  int len = 0, i;
  t_atom_feed_get_authors (self->inner, &authors, &len);

  alist = PyList_New (len);
  if (!alist)
    return NULL;

  for (i = 0; i < len; i++)
    {
      PyObject *author;
      author = Py_AtomPerson_FromAtomPerson (authors[i]);
      Py_INCREF (author);
      PyList_SetItem (alist, i, author);
    }

  Py_INCREF (alist);
  return alist;
}
'''

def t_atom_feed_get_categories():
    return '''
static PyObject *
PyAtomFeedObject_get_categories (PyAtomFeedObject *self,
                                 PyObject         *args)
{
  PyObject *alist;
  TAtomCategory **categories;
  int len = 0, i;
  t_atom_feed_get_categories (self->inner, &categories, &len);

  alist = PyList_New (len);
  if (!alist)
    return NULL;

  for (i = 0; i < len; i++)
    {
      PyObject *category;
      category = Py_AtomCategory_FromAtomCategory (categories[i]);
      Py_INCREF (category);
      PyList_SetItem (alist, i, category);
    }

  Py_INCREF (alist);
  return alist;
}
'''

def t_atom_feed_get_entries():
    return '''
static PyObject *
PyAtomFeedObject_get_entries (PyAtomFeedObject *self,
                              PyObject         *args)
{
  PyObject *alist;
  TAtomEntry **entries;
  int len = 0, i;
  t_atom_feed_get_entries (self->inner, &entries, &len);

  alist = PyList_New (len);
  if (!alist)
    return NULL;

  for (i = 0; i < len; i++)
    {
      PyObject *entry;
      entry = Py_AtomEntry_FromAtomEntry (entries[i]);
      Py_INCREF (entry);
      PyList_SetItem (alist, i, entry);
    }

  Py_INCREF (alist);
  return alist;
}
'''

OVERRIDES = {
    'PyFilterObject': pyfilterobject,
    'PyLogObject': pylogobject,
    't_atom_entry_get_authors': t_atom_entry_get_authors,
    't_atom_entry_get_categories': t_atom_entry_get_categories,
    't_atom_feed_get_authors': t_atom_feed_get_authors,
    't_atom_feed_get_categories': t_atom_feed_get_categories,
    't_atom_feed_get_entries': t_atom_feed_get_entries,
    't_filter_add': t_filter_add,
    't_filter_call': t_filter_call,
    't_log_set_handler': t_log_set_handler,
}
