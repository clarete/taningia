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

#include <stdlib.h>
#include <taningia/list.h>

list_t *
list_new (void)
{
  list_t *list = malloc (sizeof (list_t));
  list->prev = NULL;
  list->next = NULL;
  return list;
}

void
list_free (list_t *list)
{
  list_t *node;
  for (node = list->next; node; node = node->next)
    free (node);
}

list_t *
list_first (list_t *list)
{
  if (list)
    while (list->prev)
      list = list->prev;
  return list;
}

list_t *
list_last (list_t *list)
{
  if (list)
    while (list->next)
      list = list->next;
  return list;
}

int
list_len (list_t *list)
{
  list_t *node;
  int len = 0;
  for (node = list; node; node = node->next)
    len++;
  return len;
}

list_t *
list_append (list_t *list, void *data)
{
  list_t *node;
  node = list_new ();

  /* This is going to be the last one, so we have no next entry */
  node->next = NULL;
  node->data = data;

  if (list)
    {
      /* If there something in the list, let's just set our newly
       * allocated node as the last one. */
      list_t *last;
      last = list_last (list);
      last->next = node;
      node->prev = last;
      return list;
    }
  else
    {
      /* Our list is empty, so let's set our node as its first
       * child */
      node->prev = NULL;
      return node;
    }
  return list;
}

int
list_count (list_t *list, void *data, list_cmp_func_t cmpfunc)
{
  list_t *node;
  int count = 0;
  for (node = list; node; node = node->next)
    if ((*cmpfunc) (node->data, data) == 0)
      count++;
  return count;
}

list_t *
list_extend (list_t *list, list_t *otherlist)
{
  if (otherlist)
    {
      list_t *last;
      last = list_last (list);
      if (last)
        last->next = otherlist;
      else
        list = otherlist;
      otherlist->prev = last;
    }
  return list;
}

int
list_index (list_t *list, void *data)
{
  list_t *node;
  int counter;
  for (node = list, counter = 0; node; node = node->next, counter++)
    if (node->data == data)
      return counter;
  return -1;
}

void *
list_item (list_t *list, int index)
{
  list_t *node;
  int counter;
  for (node = list, counter = 0; node; node = node->next, counter++)
    if (index == counter)
      return node->data;
  return NULL;
}

list_t *
list_insert (list_t *list, void *data, int position)
{
  list_t *node;
  list_t *newnode;
  int counter;
  int found = 0;

  newnode = list_new ();
  newnode->data = data;

  /* inserting an element at the first position */
  if (position == 0)
    {
      list->prev = newnode;
      newnode->next = list;
      list = newnode;
      return list;
    }

  /* handling the rest of the elements */
  for (node = list, counter = 0; node; node = node->next, counter++)
    {
      if (counter == position)
        {
          list_t *prev;
          prev = node->prev;
          prev->next = newnode;
          newnode->next = node;
          newnode->prev = prev;
          node->prev = newnode;
          found = 1;
          break;
        }
    }

  /* inserting the new element in the end of the list. */
  if (!node && !found)
    list = list_append (list, data);

  return list;
}

list_t *
list_remove (list_t *list, void *data)
{
  list_t *node = list;
  while (node)
    {
      if (node->data != data)
        node = node->next;
      else
        {
          /* Removing the chains of the found element */
          if (node->prev)
            node->prev->next = node->next;
          if (node->next)
            node->next->prev = node->prev;

          /* Handling deletions of the first element */
          if (node == list)
            {
              list = list->next;
              list->prev = NULL;
            }

          /* Freeing the element, not its data, so be carefull with
           * memory leaks */
          free (node);
          break;
        }
    }
  return list;
}

list_t *
list_reverse (list_t *list)
{
  list_t *node = NULL;
  while (list)
    {
      node = list;
      list = node->next;
      node->next = node->prev;
      node->prev = list;
    }
  return node;
}