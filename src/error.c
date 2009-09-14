/*
 * Copyright (C) 2009  Lincoln de Sousa <lincoln@minaslivre.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <taningia/error.h>

struct _TError
{
  char *name;
  char *message;
  int code;
};

TError *
t_error_new (void)
{
  TError *error;
  error = malloc (sizeof (TError));
  error->name = NULL;
  error->message = NULL;
  error->code = 0;
  return error;
}

void
t_error_free (TError *error)
{
  if (error->name)
    free (error->name);
  if (error->message)
    free (error->message);
  free (error);
}

int
t_error_get_code (TError *error)
{
  return error->code;
}

void
t_error_set_code (TError *error, int code)
{
  error->code = code;
}

const char *
t_error_get_name (TError *error)
{
  return error->name;
}

void
t_error_set_name (TError *error, const char *name)
{
  if (error->name)
    free (error->name);
  error->name = strdup (name);
}

const char *
t_error_get_message (TError *error)
{
  return error->message;
}

void
t_error_set_message (TError *error, const char *fmt, ...)
{
  int n, size = 50;
  char *msg, *np;
  va_list argp;

  if (error->message)
    free (error->message);
  if ((msg = malloc (size)) == NULL)
    msg = NULL;
  else
    while (1)
      {
        va_start (argp, fmt);
        n = vsnprintf (msg, size, fmt, argp);
        va_end (argp);
        if (n > -1 && n < size)
          break;
        if (n > -1)
          size = n+1;
        else
          size *= 2;
        if ((np = realloc (msg, size)) == NULL)
          {
            free (msg);
            msg = NULL;
            break;
          }
        else
          msg = np;
      }
  error->message = msg;
}

void
t_error_set_full (TError *error, int code, const char *name,
                  const char *fmt, ...)
{
  int n, size = 50;
  char *msg, *np;
  va_list argp;

  t_error_set_code (error, code);
  t_error_set_name (error, name);

  /* This part is a duplication of t_error_set_message because it is
   * hard to pass ... (varargs) arguments from a function to another
   * that is already doing this (we use vsnprintf in vsnprintf) */

  if (error->message)
    free (error->message);
  if ((msg = malloc (size)) == NULL)
    msg = NULL;
  else
    while (1)
      {
        va_start (argp, fmt);
        n = vsnprintf (msg, size, fmt, argp);
        va_end (argp);
        if (n > -1 && n < size)
          break;
        if (n > -1)
          size = n+1;
        else
          size *= 2;
        if ((np = realloc (msg, size)) == NULL)
          {
            free (msg);
            msg = NULL;
            break;
          }
        else
          msg = np;
      }
  error->message = msg;
}
