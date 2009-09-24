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
  list_t *list = NULL;
  list_t *otherlist = NULL;
  list_t *node = NULL;
  list_t *extended = NULL;
  list_t *newlist = NULL;

  list = list_append (list, "foo");
  list = list_append (list, "bar");
  list = list_append (list, "foo");
  list = list_append (list, "baz");

  /* len, first, last and count */

  printf ("list length: %d\n\n", list_len (list));
  printf ("first item value: %s\n\n", (char *) (list_first (list))->data);
  printf ("last item value: %s\n\n", (char *) (list_last (list))->data);
  printf ("list items:\n");
  for (node = list; node; node = node->next)
    printf (" %s\n", (char *) node->data);
  printf ("\n");
  printf ("counts of 'foo': %d\n\n",
          list_count (list, "foo", (list_cmp_func_t) strcmp));

  otherlist = list_append (otherlist, "a");
  otherlist = list_append (otherlist, "b");
  otherlist = list_append (otherlist, "c");

  printf ("otherlist items:\n");
  for (node = otherlist; node; node = node->next)
    printf (" %s\n", (char *) node->data);
  printf ("\n");

  /* extend, index and item */

  extended = list_extend (list, otherlist);
  printf ("extended list items:\n");
  for (node = extended; node; node = node->next)
    printf (" %s\n", (char *) node->data);
  printf ("\n");

  printf ("the index of `a' in extended list is: %d\n\n",
          list_index (extended, "a"));

  printf ("value of 4th position of the extended list is: %s\n\n",
          (char *) list_item (extended, 4));

  /* remove */

  newlist = list_append (newlist, "1");
  newlist = list_append (newlist, "2");
  newlist = list_append (newlist, "3");

  printf ("removing element `1' from newlist:\n");
  newlist = list_remove (newlist, "1");
  printf ("new size: %d\n", list_len (newlist));
  for (node = newlist; node; node = node->next)
    printf (" %s\n", (char *) node->data);
  printf ("\n");

  /* insert */

  printf ("reinserting `1' in the first position:\n");
  newlist = list_insert (newlist, "1", 0);

  printf ("inserting `1.5' at the second position:\n");
  newlist = list_insert (newlist, "1.5", 1);

  printf ("inserting `2.5' at the second position:\n");
  newlist = list_insert (newlist, "2.5", 3);

  printf ("inserting `3.5' at the second position:\n");
  newlist = list_insert (newlist, "3.5", 5);

  printf ("new size: %d\n", list_len (newlist));
  for (node = newlist; node; node = node->next)
    printf (" %s\n", (char *) node->data);
  printf ("\n");

  list_free (newlist);
  newlist = NULL;

  /* reverse */

  newlist = list_append (newlist, "a");
  newlist = list_append (newlist, "b");
  newlist = list_append (newlist, "c");

  printf ("reversed newlist items:\n");
  for (node = list_reverse (newlist); node; node = node->next)
    printf (" %s\n", (char *) node->data);
  printf ("\n");

  list_free (list);
  list_free (otherlist);
  list_free (newlist);
  return 0;
}
