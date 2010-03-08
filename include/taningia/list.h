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

#include <taningia/mem.h>

typedef struct _ta_list_t ta_list_t;

struct _ta_list_t
{
  ta_list_t *prev;
  ta_list_t *next;
  void *data;
};

/* This prototype defines a function that will be used by t_ta_list_count
 * and to say that values are equal, it should return `0'. Like
 * strcmp.
 */
typedef int *(*ta_list_cmp_func_t) (void *, void *);

/**
 * @name: ta_list_new
 * @type: constructor ta_list
 */
ta_list_t *ta_list_new (void);

/**
 * @name: ta_list_free
 * @type: destructor ta_list
 */
void ta_list_free (ta_list_t *list);

/**
 * @name: ta_list_first
 * @type: method ta_list
 *
 * Returns the first element of a list.
 */
ta_list_t *ta_list_first (ta_list_t *list);

/**
 * @name: ta_list_last
 * @type: method ta_list
 *
 * Returns the last element of a list.
 */
ta_list_t *ta_list_last (ta_list_t *list);

/**
 * @name: ta_list_len
 * @type: method ta_list
 *
 * Returns the length of a list.
 */
int ta_list_len (ta_list_t *list);

/**
 * @name: ta_list_append
 * @type: method ta_list
 * @param data: The element to be appended.
 *
 * Appends a new element to a list instance.
 */
ta_list_t *ta_list_append (ta_list_t *list, void *data);

/**
 * @name: ta_list_count
 * @type: method ta_list
 * @param data: The element to be counted.
 * @param cmpfunc: Function used to compare elements and decide which
 * is equal to `data'.
 *
 * Count the number of an specific element in the list.
 */
int ta_list_count (ta_list_t *list, void *data, ta_list_cmp_func_t cmpfunc);

/**
 * @name: ta_list_extend
 * @type: method ta_list
 * @param otherlist: The list used to extend the current list
 * instance.
 *
 * Bind the last element of a list instance to the first element of
 * the `otherlist'.
 */
ta_list_t *ta_list_extend (ta_list_t *list, ta_list_t *otherlist);

/**
 * @name: ta_list_index
 * @type: method ta_list
 * @param data: The element to be found
 *
 * Finds an element in the list and returns its position. If the
 * element is not found -1 is returned.
 */
int ta_list_index (ta_list_t *list, void *data);

/**
 * @name: ta_list_item
 * @type: method ta_list
 * @param index: The position to look for the item.
 *
 * Returns the element in the `index' position.
 */
void *ta_list_item (ta_list_t *list, int index);

/**
 * @name: ta_list_insert
 * @type: method ta_list
 * @param data: Data to be inserted in the list.
 * @param position: Position to insert the new element.
 *
 * Inserts the `data' element in such a `position'.
 */
ta_list_t *ta_list_insert (ta_list_t *list, void *data, int position);

/**
 * @name: ta_list_remove
 * @type: method ta_list
 * @param data: The element data to be removed from the list.
 * @param data_free_cb: Callback to free data.
 *
 * Removes the element that holds the `data' value from the list, but
 * it only frees the found element. To free data, you will have to
 * pass the `data_free_cb' parameter too.
 */
ta_list_t *ta_list_remove (ta_list_t *list, void *data,
                           ta_free_func_t data_free_cb);

/**
 * @name: ta_list_reverse
 * @type: method ta_list
 *
 * Reverses a list by simply switch the next/prev items of each
 * element.
 */
ta_list_t *ta_list_reverse (ta_list_t *list);

/* ta_list_t *ta_list_sort (ta_list_t *list, ta_list_cmp_func_t cmpfunc); */

#endif /* _TANINGIA_LIST_H_ */
