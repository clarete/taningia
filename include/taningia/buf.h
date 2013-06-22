/* buf.h - This file is part of the taningia library
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef _TANINGIA_BUF_H_
#define _TANINGIA_BUF_H_

#include <stdarg.h>
#include <taningia/error.h>

#define TA_BUF_INIT { NULL, 0, 0 }

typedef struct {
  char *ptr;
  int string_length;
  int allocated_size;
} ta_buf_t;

void ta_buf_alloc (ta_buf_t *b, int initial_size);
void ta_buf_dealloc (ta_buf_t *b);
int ta_buf_cat (ta_buf_t *b, const char *s);
int ta_buf_catf (ta_buf_t *b, const char *s, ...);
int ta_buf_vcatf (ta_buf_t *b, const char *s, va_list args);
const char *ta_buf_cstr (ta_buf_t *b);
char *ta_buf_dump (ta_buf_t *b);


#endif  /* _TANINGIA_BUF_H_ */
