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

OVERRIDES = {
    'j_atom_entry_set_updated': j_atom_entry_set_updated,
    'j_atom_entry_get_updated': j_atom_entry_get_updated,
    'j_atom_entry_get_authors': j_atom_entry_get_authors,
    'j_atom_entry_get_categories': j_atom_entry_get_categories,
}
