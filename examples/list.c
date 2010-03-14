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

#include <stdio.h>
#include <string.h>
#include <taningia/list.h>

int
main (int argc, char **argv)
{
  ta_list_t *list = NULL;
  ta_list_t *otherlist = NULL;
  ta_list_t *node = NULL;
  ta_list_t *extended = NULL;
  ta_list_t *newlist = NULL;

  list = ta_list_append (list, "foo");
  list = ta_list_append (list, "bar");
  list = ta_list_append (list, "foo");
  list = ta_list_append (list, "baz");

  /* len, first, last and count */

  printf ("list length: %d\n\n", ta_list_len (list));
  printf ("first item value: %s\n\n", (char *) (ta_list_first (list))->data);
  printf ("last item value: %s\n\n", (char *) (ta_list_last (list))->data);
  printf ("list items:\n");
  for (node = list; node; node = node->next)
    printf (" %s\n", (char *) node->data);
  printf ("\n");
  printf ("counts of 'foo': %d\n\n",
          ta_list_count (list, "foo", (ta_list_cmp_func_t) strcmp));

  otherlist = ta_list_append (otherlist, "a");
  otherlist = ta_list_append (otherlist, "b");
  otherlist = ta_list_append (otherlist, "c");

  printf ("otherlist items:\n");
  for (node = otherlist; node; node = node->next)
    printf (" %s\n", (char *) node->data);
  printf ("\n");

  /* extend, index and item */

  /* ps.: do not free otherlist. If you do that, you are going to free
   * a piece of the extended list. */
  extended = ta_list_extend (list, otherlist);
  printf ("extended list items:\n");
  for (node = extended; node; node = node->next)
    printf (" %s\n", (char *) node->data);
  printf ("\n");

  printf ("the index of `a' in extended list is: %d\n\n",
          ta_list_index (extended, "a"));

  printf ("value of 4th position of the extended list is: %s\n\n",
          (char *) ta_list_item (extended, 4));

  /* remove */

  newlist = ta_list_append (newlist, "1");
  newlist = ta_list_append (newlist, "2");
  newlist = ta_list_append (newlist, "3");

  printf ("removing element `1' from newlist:\n");
  newlist = ta_list_remove (newlist, "1", NULL);
  printf ("new size: %d\n", ta_list_len (newlist));
  for (node = newlist; node; node = node->next)
    printf (" %s\n", (char *) node->data);
  printf ("\n");

  /* insert */

  printf ("reinserting `1' in the 1st position:\n");
  newlist = ta_list_insert (newlist, "1", 0);

  printf ("inserting `1.5' at the 2nd position:\n");
  newlist = ta_list_insert (newlist, "1.5", 1);

  printf ("inserting `2.5' at the 3rd position:\n");
  newlist = ta_list_insert (newlist, "2.5", 3);

  printf ("inserting `3.5' at the 5th position:\n");
  newlist = ta_list_insert (newlist, "3.5", 5);

  printf ("new size: %d\n", ta_list_len (newlist));
  for (node = newlist; node; node = node->next)
    printf (" %s\n", (char *) node->data);
  printf ("\n");

  ta_list_free (newlist);
  newlist = NULL;

  /* reverse */

  newlist = ta_list_append (newlist, "a");
  newlist = ta_list_append (newlist, "b");
  newlist = ta_list_append (newlist, "c");

  printf ("reversed newlist items:\n");
  newlist = ta_list_reverse (newlist);
  for (node = newlist; node; node = node->next)
    printf (" %s\n", (char *) node->data);
  printf ("\n");

  ta_list_free (list);
  ta_list_free (newlist);
  return 0;
}
