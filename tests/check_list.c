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
#include <stdlib.h>
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


START_TEST (test_list_len)
{
  /* Given that I have a list with a couple elements */
  int len = 0;
  ta_list_t *l = NULL;

  l = ta_list_append (l, "a");
  l = ta_list_append (l, "b");
  l = ta_list_append (l, "c");
  l = ta_list_append (l, "d");
  l = ta_list_append (l, "3");

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


Suite *
list_suite ()
{
  Suite *s = suite_create ("taningia::list");
  TCase *tc_core = tcase_create ("Core");
  tcase_add_test (tc_core, test_list_append);
  tcase_add_test (tc_core, test_list_len);
  tcase_add_test (tc_core, test_list_count);
  suite_add_tcase (s, tc_core);
  return s;
}


int
main (void)
{
  int number_failed;
  Suite *s = list_suite ();
  SRunner *sr = srunner_create (s);

  srunner_run_all (sr, CK_NORMAL);
  number_failed = srunner_ntests_failed (sr);
  srunner_free (sr);

  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
