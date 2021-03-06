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

ta_list_t *
ta_list_new (void)
{
  ta_list_t *list = malloc (sizeof (ta_list_t));
  list->prev = NULL;
  list->next = NULL;
  return list;
}

void
ta_list_free (ta_list_t *list)
{
  ta_list_t *node = list, *tmp = NULL;
  while (node)
    {
      tmp = node;
      node = node->next;
      free (tmp);
    }
}

ta_list_t *
ta_list_first (ta_list_t *list)
{
  ta_list_t *node = list, *tmp = NULL;
  while (node)
    {
      tmp = node;
      node = node->prev;
    }
  return tmp;
}

ta_list_t *
ta_list_last (ta_list_t *list)
{
  ta_list_t *node = list, *tmp = NULL;
  while (node)
    {
      tmp = node;
      node = node->next;
    }
  return tmp;
}

int
ta_list_len (ta_list_t *list)
{
  ta_list_t *node;
  int len = 0;
  for (node = list; node; node = node->next)
    len++;
  return len;
}

ta_list_t *
ta_list_append (ta_list_t *list, void *data)
{
  ta_list_t *node;
  if (data == NULL)
    return list;

  node = ta_list_new ();

  /* This is going to be the last one, so we have no next entry */
  node->next = NULL;
  node->data = data;

  if (list)
    {
      /* If there something in the list, let's just set our newly
       * allocated node as the last one. */
      ta_list_t *last;
      last = ta_list_last (list);
      last->next = node;
      node->prev = last;
    }
  else
    {
      /* Our list is empty, so let's set our node as its first
       * child */
      node->prev = NULL;
      list = node;
    }
  return list;
}

ta_list_t *
ta_list_prepend (ta_list_t *list, void *data)
{
  return ta_list_insert (list, data, 0);
}

int
ta_list_count (ta_list_t *list, void *data, ta_list_cmp_func_t cmpfunc)
{
  ta_list_t *node;
  int count = 0;
  for (node = list; node; node = node->next)
    if ((*cmpfunc) (node->data, data) == 0)
      count++;
  return count;
}

ta_list_t *
ta_list_extend (ta_list_t *list, ta_list_t *otherlist)
{
  if (otherlist)
    {
      ta_list_t *last;
      last = ta_list_last (list);
      if (last)
        last->next = otherlist;
      else
        list = otherlist;
      otherlist->prev = last;
    }
  return list;
}

int
ta_list_index (ta_list_t *list, void *data)
{
  ta_list_t *node;
  int counter;
  for (node = list, counter = 0; node; node = node->next, counter++)
    if (node->data == data)
      return counter;
  return -1;
}

void *
ta_list_item (ta_list_t *list, int index)
{
  ta_list_t *node;
  int counter;
  for (node = list, counter = 0; node; node = node->next, counter++)
    if (index == counter)
      return node->data;
  return NULL;
}

ta_list_t *
ta_list_insert (ta_list_t *list, void *data, int position)
{
  ta_list_t *node;
  ta_list_t *newnode;
  int counter;
  int found = 0;

  newnode = ta_list_new ();
  newnode->data = data;

  /* The list is empty, let's make it the first element */
  if (list == NULL)
    return newnode;

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
          ta_list_t *prev;
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
    {
      free (newnode);
      list = ta_list_append (list, data);
    }

  return list;
}

ta_list_t *
ta_list_remove (ta_list_t *list, void *data, ta_list_t **removed)
{
  ta_list_t *node = list;
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
              if (list)
                list->prev = NULL;
            }

          /* Setting the removed element to the `removed' output
           * param. This way, the user can free its data and the node
           * itself. */
          if (removed)
            *removed = node;
          break;
        }
    }
  return list;
}

ta_list_t *
ta_list_pop (ta_list_t *list, ta_list_t **popped_out)
{
  ta_list_t *node = NULL, *prev = NULL;

  /* Popping from an empty list should return NULL */
  if ((node = list) == NULL)
    return NULL;

  /* Getting the last element (the first one which ->next is NULL) */
  node = ta_list_last (list);

  /* If the list has more than one element */
  if (node->prev)
    {
      /* Unbind the prev element from the last one */
      prev = node->prev;
      prev->next = NULL;

      /* Unbind the removed one from the previous one */
      node->prev = NULL;
    }

  /* The list must be set to NULL if there are no more elements */
  if (node == list)
    list = NULL;

  *popped_out = node;
  return list;
}

ta_list_t *
ta_list_reverse (ta_list_t *list)
{
  ta_list_t *node = NULL;
  while (list)
    {
      node = list;
      list = node->next;
      node->next = node->prev;
      node->prev = list;
    }
  return node;
}

/* Linked list merge sorting. Thanks to anakin@pobox.com for his time
 * writing a great guide and for all wikipedia contributors for the
 * very nice Mergesorte article.
 *
 * http://www.chiark.greenend.org.uk/~sgtatham/algorithms/listsort.html
 * http://en.wikipedia.org/wiki/Mergesort
 */
ta_list_t *
ta_list_sort (ta_list_t *list, ta_list_cmp_func_t cmpfunc)
{
  ta_list_t *p, *q, *e, *tail;
  int split_point, nmerges, psize, qsize, i;

  /* If the list is empty or has only one element, it is already
   * sorted. */
  if (list == NULL || list->next == NULL)
    return list;

  split_point = 1;
  while (1)
    {
      p = list;
      list = NULL;
      tail = NULL;
      nmerges = 0;

      while (p)
        {
          /* Says that we have merges to do */
          nmerges++;

          /* This `aparentely' complicated loop just initialize the q
           * var and increment size counters, IOW it splits the first
           * list in two. */
          for (i = 0, psize = 0, q = p; i < split_point; i++)
            {
              psize++;
              q = q->next;
              if (q == NULL)
                break;
            }

          /* The q part of the list starts where the p ends */
          qsize = split_point;

          while (psize > 0 || (qsize > 0 && q))
            {
              if (psize == 0)
                {
                  /* p is empty, so, let's use q as the next
                   * element. */
                  e = q;
                  q = q->next;
                  qsize--;
                }
              else if (qsize == 0 || !q)
                {
                  /* Here, q is empty. Time to use p. */
                  e = p;
                  p = p->next;
                  psize--;
                }
              else if (cmpfunc (p, q) <= 0)
                {
                  /* p is lower (or equals to) q */
                  e = p;
                  p = p->next;
                  psize--;
                }
              else
                {
                  /* q is lower than p */
                  e = q;
                  q = q->next;
                  qsize--;
                }

              if (tail)
                tail->next = e;
              else
                list = e;

              /* Preserving the chain of the double linked list */
              e->prev = tail;
              tail = e;
            }
          p = q;
        }
      tail->next = NULL;

      if (nmerges <= 1)
        /* No more merges, time to get out */
        return list;

      split_point *= 2;
    }
}
