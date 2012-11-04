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

#include <taningia/global.h>
#include <taningia/error.h>


const ta_error_t *
ta_error_last (void)
{
  return TA_GLOBAL->last_error;
}

void
ta_error_clear (void)
{
  if (TA_GLOBAL->last_error)
    free (TA_GLOBAL->last_error->message);
  TA_GLOBAL->last_error = NULL;
}

static void
set_err (int code, char *msg)
{
  ta_error_t *error = &TA_GLOBAL->error_t;
  free (error->message);
  error->code = code;
  error->message = msg;
  TA_GLOBAL->last_error = error;
}

void
ta_error_set (int errcode, const char *fmt, ...)
{
  int n, size = 32;
  char *msg, *tmp;
  va_list args;

  /* Well, we don't know how many params the user will send. So, let's
   * figure it out */
  while (1)
    {
      va_start (args, fmt);
      n = vsnprintf (msg, size, fmt, args);
      va_end (args);

      /* Ok, we're done! gtfo */
      if (n > -1 && n < size)
        break;

      /* Need more memory */
      if (n > -1)
        size = n + 1;           /* glibc 1.2 */
      else
        size *= 2;              /* glibc 2.0 */
      if ((tmp = realloc (msg, size)) == NULL)
        {
          free (msg);
          return;
        }
      else
        msg = tmp;
    }

  /* Setting the error */
  set_err (errcode, msg);
}
