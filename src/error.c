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

struct _ta_error_t
{
  char *name;
  char *message;
  int code;
};

ta_error_t *
ta_error_new (void)
{
  ta_error_t *error;
  error = malloc (sizeof (ta_error_t));
  error->name = NULL;
  error->message = NULL;
  error->code = 0;
  return error;
}

void
ta_error_free (ta_error_t *error)
{
  if (error->name)
    free (error->name);
  if (error->message)
    free (error->message);
  free (error);
}

int
ta_error_get_code (ta_error_t *error)
{
  return error->code;
}

void
ta_error_set_code (ta_error_t *error, int code)
{
  error->code = code;
}

const char *
ta_error_get_name (ta_error_t *error)
{
  return error->name;
}

void
ta_error_set_name (ta_error_t *error, const char *name)
{
  if (error->name)
    free (error->name);
  error->name = strdup (name);
}

const char *
ta_error_get_message (ta_error_t *error)
{
  return error->message;
}

void
ta_error_set_message (ta_error_t *error, const char *fmt, ...)
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
ta_error_set_full (ta_error_t *error, int code, const char *name,
                   const char *fmt, ...)
{
  int n, size = 50;
  char *msg, *np;
  va_list argp;

  ta_error_set_code (error, code);
  ta_error_set_name (error, name);

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
