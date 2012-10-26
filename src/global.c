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

#include <taningia/global.h>

/* Dummy implementation of a thread-safe global context
 *
 * It's inspired in the libgit2 global error reporting api. Currently,
 * this file is nothing but a good place to implement the real
 * protection of the global state.
 */


static ta_global_state_t __ta_state;


ta_global_state_t *
ta_global_state_get (void)
{
  return &__ta_state;
}
