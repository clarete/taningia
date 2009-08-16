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

def j_atom_entry_set_updated():
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
      PyErr_SetString(PyExc_ValueError,
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
  
  j_atom_entry_set_updated (self->inner, cdate);
  return Py_BuildValue ("");
}
'''

def j_atom_entry_get_updated():
    return '''
static PyObject *
PyAtomEntryObject_get_updated (PyAtomEntryObject *self,
                               PyObject          *args)
{
  PyObject *date;
  time_t cdate;
  struct tm *ctm;

  cdate = j_atom_entry_get_updated (self->inner);
  ctm = gmtime (&cdate);
  date = PyDateTime_FromDateAndTime (1900 + ctm->tm_year, ctm->tm_mon + 1,
                                     ctm->tm_mday, ctm->tm_hour,
                                     ctm->tm_min, ctm->tm_sec, 0);
  Py_INCREF (date);
  return date;
}
'''

def j_atom_entry_get_authors():
    return '''
static PyObject *
PyAtomEntryObject_get_authors (PyAtomEntryObject *self,
                               PyObject          *args)
{
  PyObject *alist;
  JAtomAuthor **authors;
  int len, i;
  j_atom_entry_get_authors (self->inner, &authors, &len);

  alist = PyList_New (len);
  if (!alist)
    return NULL;

  for (i = 0; i < len; i++)
    {
      PyObject *author;
      author = Py_AtomAuthor_FromAtomAuthor (authors[i]);
      Py_INCREF (author);
      PyList_SetItem (alist, i, author);
    }

  Py_INCREF (alist);
  return alist;
}
'''

def j_atom_entry_get_categories():
    return '''
static PyObject *
PyAtomEntryObject_get_categories (PyAtomEntryObject *self,
                                  PyObject          *args)
{
  PyObject *alist;
  JAtomCategory **categories;
  int len, i;
  j_atom_entry_get_categories (self->inner, &categories, &len);

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
    return '''typedef struct {
  PyObject_HEAD
  JFilter *inner;
  PyObject *callback;
  PyObject *param;
} PyFilterObject;
'''

def j_filter_add():
    return '''
static int
call_filter_callback (void *data1, void *data2, void *data3)
{
  PyObject *retval;
  PyObject *ptuple;
  PyObject *p1 = NULL, *p2 = NULL, *p3 = NULL;
  PyFilterObject *filter;

  /* Getting params to pass to the callback */
  filter = (PyFilterObject *) data2;
  p1 = (PyObject *) j_filter_get_data (filter->inner);
  p2 = (PyObject *) filter->param;
  p3 = (PyObject *) data3;

  if (!p1)
    {
      Py_INCREF (Py_None);
      p1 = Py_None;
    }
  if (!p2)
    {
      Py_INCREF (Py_None);
      p2 = Py_None;
    }
  if (!p3)
    {
      Py_INCREF (Py_None);
      p3 = Py_None;
    }

  /* Packing all params in a tuple */
  ptuple = PyTuple_New (3);
  PyTuple_SetItem (ptuple, 0, p1);
  PyTuple_SetItem (ptuple, 1, p2);
  PyTuple_SetItem (ptuple, 2, p3);

  retval = PyObject_CallObject (filter->callback, ptuple);
  if (!retval)
    PyErr_Print ();
  else
    Py_DECREF (retval);

  return 1;
}

static PyObject *
PyFilterObject_add (PyFilterObject *self,
                    PyObject       *args)
{
  const char *name = NULL;
  PyObject *callback = NULL;
  PyObject *extra = NULL;
  if (!PyArg_ParseTuple (args, "sO|O", &name, &callback, &extra))
    return NULL;

  if (!PyCallable_Check (callback))
    {
      PyErr_SetString (PyExc_ValueError,
                       "Param 2 must be callable.");
      return NULL;
    }
  Py_INCREF (callback);
  self->callback = callback;
  if (extra)
    {
      Py_INCREF (extra);
      self->param = extra;
    }
  j_filter_add (self->inner,
                name,
                (JFilterCallback) call_filter_callback,
                self);

  Py_INCREF (Py_None);
  return Py_None;
}
'''

def j_filter_call():
    return '''
static PyObject *
PyFilterObject_call (PyFilterObject *self,
                     PyObject       *args)
{
  const char *name = NULL;
  PyObject *extra = NULL;
  if (!PyArg_ParseTuple (args, "s|O", &name, &extra))
    return NULL;

  j_filter_call (self->inner, name, extra);

  Py_INCREF (Py_None);
  return Py_None;
}
'''

OVERRIDES = {
    'PyFilterObject': pyfilterobject,
    'j_atom_entry_set_updated': j_atom_entry_set_updated,
    'j_atom_entry_get_updated': j_atom_entry_get_updated,
    'j_atom_entry_get_authors': j_atom_entry_get_authors,
    'j_atom_entry_get_categories': j_atom_entry_get_categories,
    'j_filter_add': j_filter_add,
    'j_filter_call': j_filter_call,
}
