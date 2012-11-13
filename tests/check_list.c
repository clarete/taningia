/* check_list.c - This file is part of libtaningia
 *
 * Copyright (C) 2012  Lincoln de Sousa <lincoln@comum.org>
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


#include <check.h>
#include <string.h>
#include <taningia/list.h>


START_TEST (test_list_append)
{
  /* Given that I declare a NULL list */
  ta_list_t *l = NULL;

  /* When I start appending a couple elements to this NULL list */
  l = ta_list_append (l, "1");
  l = ta_list_append (l, "2");

  /* Then I see that its first element contains the string I've informed
   * above and the next element after the first one is also right */
  fail_unless (strcmp (l->data, "1") == 0,
               "The first element does not match the content informed");
  fail_unless (strcmp (l->next->data, "2") == 0,
               "The second element does not match the content informed");

  /* Just cleaning things up */
  ta_list_free (l);
}
END_TEST


START_TEST (test_list_prepend)
{
  /* Given that I have a list with two elements */
  ta_list_t *elves = NULL;
  elves = ta_list_append (elves, "Galadriel");
  elves = ta_list_append (elves, "Legolas");

  /* When I prepend new elements to the list */
  elves = ta_list_prepend (elves, "Turgon");
  elves = ta_list_prepend (elves, "Fëanor");

  /* Then I see that my items prepended later appear in the first
   * positions */
  fail_unless (strcmp (ta_list_item (elves, 0), "Fëanor") == 0,
               "First element mismatch");
  fail_unless (strcmp (ta_list_item (elves, 1), "Turgon") == 0,
               "Second element mismatch");
  fail_unless (strcmp (ta_list_item (elves, 2), "Galadriel") == 0,
               "Third element mismatch");
  fail_unless (strcmp (ta_list_item (elves, 3), "Legolas") == 0,
               "Fourth element mismatch");
}
END_TEST


START_TEST (test_list_len)
{
  /* Given that I have a list with a couple elements */
  int len = 0;
  ta_list_t *l = NULL;

  l = ta_list_append (l, "a");
  l = ta_list_append (l, "b");
  l = ta_list_append (l, "c");
  l = ta_list_append (l, "d");
  l = ta_list_append (l, "e");

  /* When I get the length of this list */
  len = ta_list_len (l);

  /* Than I see that everything is correct */
  fail_unless (len == 5, "The list length does not match");
}
END_TEST


START_TEST (test_list_count)
{
  /* Given that I have a list with a couple names */
  int count = 0;
  ta_list_t *l = NULL;

  l = ta_list_append (l, "lincoln");
  l = ta_list_append (l, "metal");
  l = ta_list_append (l, "lincoln");
  l = ta_list_append (l, "jester");
  l = ta_list_append (l, "guerrinha");

  /* When I want to know how many times my name is on that list */
  count = ta_list_count (l, "lincoln", (ta_list_cmp_func_t) strcmp);

  /* Then I see the right count number */
  fail_unless (count == 2, "Element count number does not match");
}
END_TEST


START_TEST (test_list_first_last)
{
  /* Given that I have a list of a couple of vars pointing to different
   * positions on it */
  ta_list_t *a, *b, *c, *d, *e, *f;
  ta_list_t *first, *last;
  a = ta_list_append (NULL, "first");
  b = ta_list_append (a, "second");
  c = ta_list_append (b, "third");
  d = ta_list_append (c, "fourth");
  e = ta_list_append (d, "fifth");
  f = ta_list_append (e, "sixth");

  /* When I try to find the first and the last elements */
  first = ta_list_first (d);
  last = ta_list_last (b);

  /* Then I see that the right elements were found */
  fail_unless (strcmp (first->data, "first") == 0,
               "Could not find the first element");
  fail_unless (strcmp (last->data, "sixth") == 0,
               "Could not find the last element");
}
END_TEST


START_TEST (test_list_pop)
{
  /* Given that I have a list of a couple elements */
  ta_list_t *mylist = NULL;
  ta_list_t *popped_element = NULL;
  mylist = ta_list_append (mylist, "a");
  mylist = ta_list_append (mylist, "b");
  mylist = ta_list_append (mylist, "c");

  /* When I pop an item from the list */
  mylist = ta_list_pop (mylist, &popped_element);

  /* Then I see that the first added element was removed and returned
   * and that the list head was changed, as well as it's size */
  fail_unless (strcmp (popped_element->data, "c") == 0,
               "Wrong element popped out");
  fail_unless (strcmp((ta_list_last (mylist))->data, "b") == 0,
               "Wrong tail set after pop()");
  fail_unless (ta_list_len (mylist) == 2, "Element not removed on pop");
}
END_TEST


START_TEST (test_list_pop_from_empty_list)
{
  /* Given that I have an empty list */
  ta_list_t *mylist = NULL;
  ta_list_t *popped_element = NULL;

  /* When I pop an item from the list */
  mylist = ta_list_pop (mylist, &popped_element);

  /* Then I see that the popped element is NULL */
  fail_unless (popped_element == NULL, "Popping from an empty list failed");
  fail_unless (mylist == NULL, "Popping from an empty list failed");
}
END_TEST


START_TEST (test_list_pop_until_the_list_is_empty)
{
  /* Given that I have a list with two elements */
  ta_list_t *mylist = NULL;
  ta_list_t *p1 = NULL, *p2 = NULL;
  mylist = ta_list_prepend (mylist, "1");
  mylist = ta_list_prepend (mylist, "2");

  /* When I pop two elements */
  mylist = ta_list_pop (mylist, &p1);
  mylist = ta_list_pop (mylist, &p2);

  /* Then I see the right values popped out and the list empty */
  fail_unless (strcmp (p1->data, "1") == 0,
               "Wrong element popped from the list");
  fail_unless (strcmp (p2->data, "2") == 0,
               "Wrong element popped from the list");
  fail_unless (mylist == NULL,
               "The list is not empty after popping everything from it");
}
END_TEST


START_TEST (test_list_extend)
{
  /* Given that I have two lists */
  ta_list_t *one = NULL, *other = NULL;

  one = ta_list_append (one, "1");
  one = ta_list_append (one, "2");
  one = ta_list_append (one, "3");
  one = ta_list_append (one, "4");

  other = ta_list_append (other, "5");
  other = ta_list_append (other, "6");
  other = ta_list_append (other, "7");
  other = ta_list_append (other, "8");

  /* When I extend the first one with the second one */
  one = ta_list_extend (one, other);

  /* Then I see that the first list was extended with the second one */
  fail_unless (ta_list_len (one) == 8, "New list size does not match");
  fail_unless (strcmp ((ta_list_last (one))->data, "8") == 0,
               "The last element of the first list is wrong");
  fail_unless (strcmp ((ta_list_first (other))->data, "1") == 0,
               "The first element of the other list is wrong");
}
END_TEST


START_TEST (test_list_index)
{
  /* Given I that have a list with some elements */
  ta_list_t *l = NULL;
  int pos1, pos2;
  l = ta_list_append (l, "Aragorn");
  l = ta_list_append (l, "Sam");
  l = ta_list_append (l, "Frodo");
  l = ta_list_append (l, "Legolas");
  l = ta_list_append (l, "Boromir");

  /* When I try to find Frodo and Gimli */
  pos1 = ta_list_index (l, "Frodo");
  pos2 = ta_list_index (l, "Gimli");

  /* Then I see that I can find Frodo, but I cannot find Gimli */
  fail_unless (pos1 == 2, "Frodo cannot be found");
  fail_unless (pos2 == -1, "Gimli was found, but it was not supposed to");
}
END_TEST


START_TEST (test_list_item)
{
  /* Given I that have a list with some elements */
  ta_list_t *l = NULL;
  char *narya, *vilya, *nenya;
  l = ta_list_append (l, "Narya");
  l = ta_list_append (l, "Vilya");
  l = ta_list_append (l, "Nenya");

  /* Then I try to get the elvish kings rings of power */
  narya = ta_list_item (l, 0);
  vilya = ta_list_item (l, 1);
  nenya = ta_list_item (l, 2);

  /* Then I see that all the indexes matches the expected content */
  fail_unless (strcmp (narya, "Narya") == 0,
               "Content does not match the index");
  fail_unless (strcmp (vilya, "Vilya") == 0,
               "Content does not match the index");
  fail_unless (strcmp (nenya, "Nenya") == 0,
               "Content does not match the index");
}
END_TEST


START_TEST (test_list_insert)
{
  /* Given that I have a list of with some Nazgûl aliases */
  ta_list_t *nazgul = NULL;
  nazgul = ta_list_append (nazgul, "Ulairi");
  nazgul = ta_list_append (nazgul, "Black Riders");

  /* When I insert new elements */
  nazgul = ta_list_insert (nazgul, "Fell Riders", 0);
  nazgul = ta_list_insert (nazgul, "Ringwraiths", 2);
  nazgul = ta_list_insert (nazgul, "The Nine", 1);

  /* Than I see that they were added to the right positions */
  fail_unless (strcmp (ta_list_item (nazgul, 0), "Fell Riders") == 0,
               "Item is placed in the wrong position");
  fail_unless (strcmp (ta_list_item (nazgul, 1), "The Nine") == 0,
               "Item is placed in the wrong position");
  fail_unless (strcmp (ta_list_item (nazgul, 2), "Ulairi") == 0,
               "Item is placed in the wrong position");
  fail_unless (strcmp (ta_list_item (nazgul, 3), "Ringwraiths") == 0,
               "Item is placed in the wrong position");
  fail_unless (strcmp (ta_list_item (nazgul, 4), "Black Riders") == 0,
               "Item is placed in the wrong position");
}
END_TEST


START_TEST (test_list_insert_in_an_empty_list)
{
  /* Given that I have an empty list */
  ta_list_t *mystuff = NULL;

  /* When I try to insert to the 0th position of an empty list */
  mystuff = ta_list_insert (mystuff, "The One Ring", 0);

  /* Then I see that it was added as the first and only element */
  fail_unless (ta_list_len (mystuff) == 1, "Insert in 0th pos didn't work");

  /* Deallocating */
  ta_list_free (mystuff);
}
END_TEST


START_TEST (test_list_remove)
{
  /* Given that I have a list of with some important places */
  ta_list_t *places = NULL;
  char *removed = NULL;
  places = ta_list_append (places, "Mordor");
  places = ta_list_append (places, "Rhun");
  places = ta_list_append (places, "Rohan");
  places = ta_list_append (places, "Lindon");
  places = ta_list_append (places, "Eriador");

  /* When I remove a couple elements */
  places = ta_list_remove (places, "Mordor", NULL);
  places = ta_list_remove (places, "Lindon", (void *) removed);

  /* Then I see that the list is now without the elements and I got the
   * removed element using the last parameter of the _remove()
   * function */
  fail_unless (ta_list_len (places) == 3,
               "List size is wrong, items were not removed");
  fail_unless (ta_list_index (places, "Rhun") == 0,
               "Remove failed, item is in the wrong place");
  fail_unless (ta_list_index (places, "Rohan") == 1,
               "Remove failed, item is in the wrong place");
  fail_unless (ta_list_index (places, "Eriador") == 2,
               "Remove failed, item is in the wrong place");
}
END_TEST


START_TEST (test_list_reverse)
{
  /* Given that I have a list of elements */
  ta_list_t *l = NULL;
  l = ta_list_append (l, "1");
  l = ta_list_append (l, "2");
  l = ta_list_append (l, "3");
  l = ta_list_append (l, "4");

  /* When I reverse it */
  l = ta_list_reverse (l);

  /* Then I see the items were reversed */
  fail_unless (ta_list_index (l, "4") == 0,
               "Reverse failed, item is in the wrong place");
  fail_unless (ta_list_index (l, "3") == 1,
               "Reverse failed, item is in the wrong place");
  fail_unless (ta_list_index (l, "2") == 2,
               "Reverse failed, item is in the wrong place");
  fail_unless (ta_list_index (l, "1") == 3,
               "Reverse failed, item is in the wrong place");
}
END_TEST


static int
_strcmp_wrapper (ta_list_t *a, ta_list_t *b)
{
  return strcmp (a->data, b->data);
}


START_TEST (test_list_sort)
{
  /* Given that I have a list of unsorted elements */
  ta_list_t *names = NULL;
  names = ta_list_append (names, "Yavanna");
  names = ta_list_append (names, "Olwe");
  names = ta_list_append (names, "Melkor");
  names = ta_list_append (names, "Tulkas");
  names = ta_list_append (names, "Aule");
  names = ta_list_append (names, "Mandos");
  names = ta_list_append (names, "Nienna");

  /* When I sort this list */
  names = ta_list_sort (names, (ta_list_cmp_func_t) _strcmp_wrapper);

  /* Then I see that the results were ordered correctly */
  fail_unless (ta_list_index (names, "Aule") == 0,
               "Sort failed, item is in the wrong place");
  fail_unless (ta_list_index (names, "Mandos") == 1,
               "Sort failed, item is in the wrong place");
  fail_unless (ta_list_index (names, "Melkor") == 2,
               "Sort failed, item is in the wrong place");
  fail_unless (ta_list_index (names, "Nienna") == 3,
               "Sort failed, item is in the wrong place");
  fail_unless (ta_list_index (names, "Olwe") == 4,
               "Sort failed, item is in the wrong place");
  fail_unless (ta_list_index (names, "Tulkas") == 5,
               "Sort failed, item is in the wrong place");
  fail_unless (ta_list_index (names, "Yavanna") == 6,
               "Sort failed, item is in the wrong place");
}
END_TEST


Suite *
list_suite ()
{
  Suite *s = suite_create ("taningia::list");
  TCase *tc_core = tcase_create ("Core");
  tcase_add_test (tc_core, test_list_append);
  tcase_add_test (tc_core, test_list_prepend);
  tcase_add_test (tc_core, test_list_len);
  tcase_add_test (tc_core, test_list_count);
  tcase_add_test (tc_core, test_list_first_last);
  tcase_add_test (tc_core, test_list_pop);
  tcase_add_test (tc_core, test_list_pop_from_empty_list);
  tcase_add_test (tc_core, test_list_pop_until_the_list_is_empty);
  tcase_add_test (tc_core, test_list_extend);
  tcase_add_test (tc_core, test_list_index);
  tcase_add_test (tc_core, test_list_item);
  tcase_add_test (tc_core, test_list_insert);
  tcase_add_test (tc_core, test_list_insert_in_an_empty_list);
  tcase_add_test (tc_core, test_list_remove);
  tcase_add_test (tc_core, test_list_reverse);
  tcase_add_test (tc_core, test_list_sort);
  suite_add_tcase (s, tc_core);
  return s;
}
