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

def t_atom_entry_set_updated():
    return '''
static PyObject *
PyAtomEntryObject_set_updated (PyAtomEntryObject *self,
                               PyObject          *args)
{
  PyObject *date;
  time_t cdate;
  int sec, min, hour, day, month, year;

  if (!PyArg_ParseTuple (args, "O", &date))
    return NULL;

  if (!PyDateTime_Check (date))
    {
      PyErr_SetString(PyExc_TypeError,
		      "param 1 must be a datetime.datetime instance.");
      return NULL;
    }

  sec = PyDateTime_DATE_GET_SECOND (date);
  min = PyDateTime_DATE_GET_MINUTE (date);
  hour = PyDateTime_DATE_GET_HOUR (date);
  day = PyDateTime_GET_DAY (date);
  month = PyDateTime_GET_MONTH (date) - 1;  /* Month starts from 0 */
  year = PyDateTime_GET_YEAR (date) - 1900; /* read man mktime */
  struct tm mytime = { sec, min, hour, day, month, year };
  cdate = mktime (&mytime);
  
  t_atom_entry_set_updated (self->inner, cdate);
  return Py_BuildValue ("");
}
'''

def t_atom_entry_get_updated():
    return '''
static PyObject *
PyAtomEntryObject_get_updated (PyAtomEntryObject *self,
                               PyObject          *args)
{
  PyObject *date;
  time_t cdate;
  struct tm *ctm;

  cdate = t_atom_entry_get_updated (self->inner);
  ctm = gmtime (&cdate);
  date = PyDateTime_FromDateAndTime (1900 + ctm->tm_year, ctm->tm_mon + 1,
                                     ctm->tm_mday, ctm->tm_hour,
                                     ctm->tm_min, ctm->tm_sec, 0);
  Py_INCREF (date);
  return date;
}
'''

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

OVERRIDES = {
    'PyFilterObject': pyfilterobject,
    'PyLogObject': pylogobject,
    't_atom_entry_set_updated': t_atom_entry_set_updated,
    't_atom_entry_get_updated': t_atom_entry_get_updated,
    't_atom_entry_get_authors': t_atom_entry_get_authors,
    't_atom_entry_get_categories': t_atom_entry_get_categories,
    't_filter_add': t_filter_add,
    't_filter_call': t_filter_call,
    't_log_set_handler': t_log_set_handler,
}
