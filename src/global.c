/* global.c - This file is part of the taningia library
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.*
 */

#include <config.h>
#include <stdlib.h>
#include <string.h>
#include <taningia/global.h>

/* Thread Local Storage based context. Inspired in the libgit2 global
 * error reporting api.
 */


#ifdef HAVE_LIBPTHREAD

#include <pthread.h>

static pthread_key_t _tls_key;
static int _tls_init = 0;

void
ta_global_state_setup (void)
{
  pthread_key_create (&_tls_key, free);
  _tls_init = 1;
}

void
ta_global_state_teardown (void)
{
  pthread_key_delete (_tls_key);
  _tls_init = 0;
}

ta_global_state_t *
ta_global_state_get (void)
{
  void *state;
  if ((state = pthread_getspecific (_tls_key)) != NULL)
    return state;

  state = malloc (sizeof (ta_global_state_t));
  if (!state)
    return NULL;

  memset (state, 0x0, sizeof (ta_global_state_t));
  pthread_setspecific (_tls_key, state);
  return (ta_global_state_t *) state;
}

#else

static ta_global_state_t __ta_state;

void
ta_global_state_setup (void)
{
}

void
ta_global_state_teardown (void)
{
}

ta_global_state_t *
ta_global_state_get (void)
{
  return &__ta_state;
}

#endif  /* HAVE_PTHREADS */
