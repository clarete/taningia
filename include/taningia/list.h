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

#ifndef _TANINGIA_LIST_H_
#define _TANINGIA_LIST_H_

typedef struct _list_t list_t;

struct _list_t
{
  list_t *prev;
  list_t *next;
  void *data;
};

/* This prototype defines a function that will be used by t_list_count
 * and to say that values are equal, it should return `0'. Like
 * strcmp.
 */
typedef int *(*list_cmp_func_t) (void *, void *);

/**
 * @name: list_new
 * @type: constructor list
 */
list_t *list_new (void);

/**
 * @name: list_free
 * @type: destructor list
 */
void list_free (list_t *list);

/**
 * @name: list_first
 * @type: method list
 *
 * Returns the first element of a list.
 */
list_t *list_first (list_t *list);

/**
 * @name: list_last
 * @type: method list
 *
 * Returns the last element of a list.
 */
list_t *list_last (list_t *list);

/**
 * @name: list_len
 * @type: method list
 *
 * Returns the length of a list.
 */
int list_len (list_t *list);

/**
 * @name: list_append
 * @type: method list
 * @param data: The element to be appended.
 *
 * Appends a new element to a list instance.
 */
list_t *list_append (list_t *list, void *data);

/**
 * @name: list_count
 * @type: method list
 * @param data: The element to be counted.
 * @param cmpfunc: Function used to compare elements and decide which
 * is equal to `data'.
 *
 * Count the number of an specific element in the list.
 */
int list_count (list_t *list, void *data, list_cmp_func_t cmpfunc);

/**
 * @name: list_extend
 * @type: method list
 * @param otherlist: The list used to extend the current list
 * instance.
 *
 * Bind the last element of a list instance to the first element of
 * the `otherlist'.
 */
list_t *list_extend (list_t *list, list_t *otherlist);

/**
 * @name: list_index
 * @type: method list
 * @param data: The element to be found
 *
 * Finds an element in the list and returns its position. If the
 * element is not found -1 is returned.
 */
int list_index (list_t *list, void *data);

/**
 * @name: list_item
 * @type: method list
 * @param index: The position to look for the item.
 *
 * Returns the element in the `index' position.
 */
void *list_item (list_t *list, int index);

/**
 * @name: list_insert
 * @type: method list
 * @param data: Data to be inserted in the list.
 * @param position: Position to insert the new element.
 *
 * Inserts the `data' element in such a `position'.
 */
list_t *list_insert (list_t *list, void *data, int position);

/**
 * @name: list_remove
 * @type: method list
 * @param data: The element to be removed from the list.
 *
 * Removes the element that holds the `data' value from the list, but
 * it only frees that found. Not `data', it is up to the caller.
 */
list_t *list_remove (list_t *list, void *data);

/**
 * @name: list_reverse
 * @type: method list
 *
 * Reverses a list by simply switch the next/prev items of each
 * element.
 */
list_t *list_reverse (list_t *list);

/* list_t *list_sort (list_t *list, list_cmp_func_t cmpfunc); */

#endif /* _TANINGIA_LIST_H_ */
