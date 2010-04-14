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

#include <taningia/object.h>

void
ta_object_init (ta_object_t *obj, ta_free_func_t destructor)
{
  obj->refcount = 1;
  obj->destructor = destructor;
}

void *
ta_object_ref (void *obj)
{
  ((ta_object_t *) obj)->refcount++;
  return obj;
}

void
ta_object_unref (void *obj)
{
  ta_object_t *object = (ta_object_t *) obj;
  if (--object->refcount == 0)
    if (object->destructor)
      object->destructor (obj);
}
