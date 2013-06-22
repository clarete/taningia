/* buf.c - This file is part of the taningia library
 *
 * Copyright (C) 2012  Lincoln de Sousa <lincoln@comum.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <taningia/error.h>
#include <taningia/buf.h>

/* http://gcc.gnu.org/bugzilla/show_bug.cgi?id=40880 */
#ifndef va_copy
#define va_copy __va_copy
#endif

/* Forward declarations */


int _buf_realloc (ta_buf_t *b, int requested_size);


/* Public API */


void
ta_buf_alloc (ta_buf_t *b, int initial_size)
{
  b->ptr = NULL;
  b->string_length = 0;
  b->allocated_size = 0;
  _buf_realloc (b, initial_size);
}

void
ta_buf_dealloc (ta_buf_t *b)
{
  if (b->ptr)
    {
      free (b->ptr);
      b->ptr = NULL;
    }
  b->string_length = 0;
  b->allocated_size = 0;
}


int
ta_buf_cat (ta_buf_t *b, const char *s)
{
  int s_size = strlen (s);

  /* Allocating enough room to the new data */
  if (_buf_realloc (b, b->string_length + s_size + 1) != TA_OK)
    return TA_ERROR;

  /* Copying stuff */
  memmove (b->ptr + b->string_length, s, s_size);
  b->string_length += s_size;
  b->ptr[b->string_length] = '\0';

  return TA_OK;
}


int
ta_buf_catf (ta_buf_t *b, const char *s, ...)
{
  va_list args;
  int status;
  va_start (args, s);
  status = ta_buf_vcatf (b, s, args);
  va_end (args);
  return status;
}


int
ta_buf_vcatf (ta_buf_t *b, const char *s, va_list args)
{
  int n;
  int allocated_size = b->allocated_size + strlen (s);

  if (_buf_realloc (b, allocated_size) != TA_OK)
    return TA_ERROR;

  while (1)
    {
      va_list copied_args;
      va_copy (copied_args, args);

      n = vsnprintf (b->ptr + b->string_length,
                     b->allocated_size - b->string_length, 
                     s,
                     copied_args);

      /* Updating the string length */
      b->string_length += n;

      /* Ok, we're done! gtfo */
      if (n > -1 && n <= b->string_length)
        break;

      /* Need more memory */
      allocated_size += n;
      if (_buf_realloc (b, allocated_size) != TA_OK)
        return TA_ERROR;
    }
  return TA_OK;
}


const char *
ta_buf_cstr (ta_buf_t *b)
{
  return b->ptr;
}

char *
ta_buf_dump (ta_buf_t *b)
{
  char *tmp = strdup (b->ptr);
  tmp[b->string_length] = '\0';
  ta_buf_dealloc (b);
  return tmp;
}

/* Private API */

int
_buf_realloc (ta_buf_t *b, int requested_size)
{
  int size;
  void *tmp = NULL;

  size = b->allocated_size;

  /* Nothing to do here */
  if (requested_size == 0 || requested_size <= size)
    return TA_OK;

  /* Taking care of the first time we allocate things */
  if (size == 0)
    size = requested_size;

  /* Multiply by 1.5 untill we reach the requested size */
  while (size < requested_size)
    size = (size << 1) - (size >> 1);

  /* Align the memory that will be allocated to a multiple of 8 */
  size = (size + 7) & ~7;

  /* Time to realloc our internal pointer. If it does not work, there's
   * nothing actually to do. It's up to the caller to decide if he will
   * do anything with the already allocated memory and the data on
   * it. */
  if ((tmp = realloc (b->ptr, size)) == NULL)
    return TA_ERROR;
  b->ptr = tmp;

  /* Just updating the buffer counter after making sure things worked */
  b->allocated_size = size;

  return TA_OK;
}
