/* global.h - This file is part of the taningia library
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

#ifndef _TANINGIA_GLOBAL_H_
#define _TANINGIA_GLOBAL_H_

#include <taningia/error.h>


typedef struct {
  ta_error_t *last_error;
  ta_error_t error_t;
} ta_global_state_t;


void ta_global_state_setup (void);
void ta_global_state_teardown (void);
ta_global_state_t *ta_global_state_get (void);

#define TA_GLOBAL (ta_global_state_get ())

#endif  /* _TANINGIA_GLOBAL_H_ */
