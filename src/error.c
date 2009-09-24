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

struct _error_t
{
  char *name;
  char *message;
  int code;
};

error_t *
error_new (void)
{
  error_t *error;
  error = malloc (sizeof (error_t));
  error->name = NULL;
  error->message = NULL;
  error->code = 0;
  return error;
}

void
error_free (error_t *error)
{
  if (error->name)
    free (error->name);
  if (error->message)
    free (error->message);
  free (error);
}

int
error_get_code (error_t *error)
{
  return error->code;
}

void
error_set_code (error_t *error, int code)
{
  error->code = code;
}

const char *
error_get_name (error_t *error)
{
  return error->name;
}

void
error_set_name (error_t *error, const char *name)
{
  if (error->name)
    free (error->name);
  error->name = strdup (name);
}

const char *
error_get_message (error_t *error)
{
  return error->message;
}

void
error_set_message (error_t *error, const char *fmt, ...)
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
error_set_full (error_t *error, int code, const char *name,
                const char *fmt, ...)
{
  int n, size = 50;
  char *msg, *np;
  va_list argp;

  error_set_code (error, code);
  error_set_name (error, name);

  /* This part is a duplication of error_set_message because it is
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
