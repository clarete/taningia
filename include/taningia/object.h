/*
 * Copyright (C) 2010  Lincoln de Sousa <lincoln@comum.org>
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

#ifndef _TANINGIA_OBJECT_H_
#define _TANINGIA_OBJECT_H_

#include <taningia/mem.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TA_CAST_OBJECT(o) ((ta_object_t *) (o))

typedef struct
{
  int refcount;
  ta_free_func_t destructor;
} ta_object_t;

void ta_object_init (ta_object_t *obj, ta_free_func_t destructor);

void *ta_object_ref (void *obj);

void ta_object_unref (void *obj);

#ifdef __cplusplus
}
#endif

#endif /* _TANINGIA_TA_OBJECT_H_ */
