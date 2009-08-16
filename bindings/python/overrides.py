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

def j_atom_content_new():
    return '''
static int
AtomContent_init (PyAtomContentObject *self,
                  PyObject            *args,
                  PyObject            *kwargs)
{
  const char * _type;
  const char * _content;
  int _len;
  if (!PyArg_ParseTuple (args, "ss", &_type, &_content))
    return -1;
  _len = strlen (_content);
  self->inner = j_atom_content_new (_type, _content, _len);
  return 0;
}
'''

def j_atom_entry_set_updated():
    return '''
static PyObject *
AtomEntry_set_updated (PyAtomEntryObject *self,
                       PyObject          *args)
{
  PyObject *date;
  time_t cdate;
  int sec, min, hour, day, month, year;
  PyDateTime_IMPORT;

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
AtomEntry_get_updated (PyAtomEntryObject *self,
                       PyObject          *args)
{
  PyObject *date;
  time_t cdate;
  struct tm *ctm;
  PyDateTime_IMPORT;

  cdate = j_atom_entry_get_updated (self->inner);
  ctm = gmtime (&cdate);
  date = PyDateTime_FromDateAndTime (1900 + ctm->tm_year, ctm->tm_mon + 1,
                                     ctm->tm_mday, ctm->tm_hour,
                                     ctm->tm_min, ctm->tm_sec, 0);
  Py_INCREF (date);
  return date;
}
'''

def j_atom_entry_add_author():
    return '''
static PyObject *
AtomEntry_add_author (PyAtomEntryObject *self,
                      PyObject          *args)
{
  PyAtomAuthorObject * _author;
  if (!PyArg_ParseTuple (args, "O", &_author))
    return NULL;
  Py_INCREF (_author);
  j_atom_entry_add_author (self->inner, _author->inner);
  return Py_BuildValue ("");
}
'''

OVERRIDES = {
    'j_atom_content_new': j_atom_content_new,
    'j_atom_entry_set_updated': j_atom_entry_set_updated,
    'j_atom_entry_get_updated': j_atom_entry_get_updated,
    'j_atom_entry_add_author': j_atom_entry_add_author,
}
