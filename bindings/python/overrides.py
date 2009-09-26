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

def t_pubsub_node_createv():
    return ''

def t_pubsub_node_create():
    return '''
static PyObject *
PyPubsubNodeObject_create (PyPubsubNodeObject *self,
                           PyObject           *args)
{
  PyObject *dict = NULL;
  char **params = NULL;
  Py_ssize_t counter = 0;

  if (!PyArg_ParseTuple (args, "|O", &dict))
    return NULL;
  if (dict && !PyDict_Check(dict))
    {
      PyErr_SetString (PyExc_TypeError, "Param 1 must be a dictionary");
      return NULL;
    }
  else if (dict)
    {
      PyObject *items, *key, *val;
      Py_ssize_t len, i;

      items = PyDict_Items (dict);
      len = PyList_Size (items);
      params = malloc (sizeof (char *) * (len*2) + 1);

      for (i = 0; i < len; i++)
        {
          PyObject *item;
          item = PyList_GetItem (items, i);

          key = PyTuple_GetItem (item, 0);
          val = PyTuple_GetItem (item, 1);

          if (!PyString_Check (key) || !PyString_Check (val))
            {
              PyErr_SetString (PyExc_TypeError,
                    "All keys and values in the config_param dict "
                    "must be strings.");
              Py_DECREF (items);
              return NULL;
            }
          else
            {
              char *ckey, *cval;
              ckey = PyString_AS_STRING (key);
              cval = PyString_AS_STRING (val);
              params[counter++] = strdup (ckey);
              params[counter++] = strdup (cval);
            }
        }
    }
  if (params)
    {
      iks *riq;
      char *arg;
      int c = 0;

      /* Adding the sentinel required by the next step */
      params[counter++] = NULL;

      /* Yep, we must use createv() here to pass a char ** to it.
       * the create() one uses va_lists in (...) form. */
      riq = t_pubsub_node_createv (self->inner, (const char **) params);

      /* Freeing param container */
      while ((arg = params[c++]) != NULL)
        free (arg);
      free (params);

      return PyIks_FromIks (riq);
    }
  else
    {
      iks *riq = t_pubsub_node_create (self->inner, NULL);
      return PyIks_FromIks (riq);
    }
}
'''

OVERRIDES = {
    'PyLogObject': pylogobject,
    't_log_set_handler': t_log_set_handler,
    't_pubsub_node_createv': t_pubsub_node_createv,
    't_pubsub_node_create': t_pubsub_node_create,
}
