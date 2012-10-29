/* check_iri.c - This file is part of libtaningia
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
#include <taningia/iri.h>


START_TEST (test_iri_setters_getters)
{
  /* Given that I have an empty iri */
  ta_iri_t *iri = ta_iri_new ();

  /* When I set data on it */
  ta_iri_set_scheme (iri, "http");
  ta_iri_set_user (iri, "lincoln");
  ta_iri_set_host (iri, "comum.org");
  ta_iri_set_port (iri, 80);
  ta_iri_set_path (iri, "/this/is/a/test");
  ta_iri_set_query (iri, "p1=v1");
  ta_iri_set_fragment (iri, "stuff");

  /* Then I see that all attributes were set correctly */
  fail_unless (strcmp (ta_iri_get_scheme (iri), "http") == 0,
               "set_schema failed");
  fail_unless (strcmp (ta_iri_get_user (iri), "lincoln") == 0,
               "set_user failed");
  fail_unless (strcmp (ta_iri_get_host (iri), "comum.org") == 0,
               "set_user failed");
  fail_unless (ta_iri_get_port (iri) == 80, "set_port failed");
  fail_unless (strcmp (ta_iri_get_path (iri), "/this/is/a/test") == 0,
               "set_path failed");
  fail_unless (strcmp (ta_iri_get_query (iri), "p1=v1") == 0,
               "set_query failed");
  fail_unless (strcmp (ta_iri_get_fragment (iri), "stuff") == 0,
               "set_fragment failed");
}
END_TEST


START_TEST (test_iri_to_string)
{
  /* Given that I have some iris with some data */
  ta_iri_t *iri1 = ta_iri_new ();
  ta_iri_t *iri2 = ta_iri_new ();
  ta_iri_t *iri3 = ta_iri_new ();
  char *str1, *str2, *str3;

  str1 = str2 = str3 = NULL;
  ta_iri_set_scheme (iri1, "http");
  ta_iri_set_host (iri1, "comum.org");
  ta_iri_set_path (iri1, "/this/is/a/test");
  ta_iri_set_query (iri1, "p1=v1");

  ta_iri_set_scheme (iri2, "ssh");
  ta_iri_set_user (iri2, "lincoln");
  ta_iri_set_host (iri2, "guake.org");

  ta_iri_set_host (iri3, "minaslivre.org");
  ta_iri_set_fragment (iri3, "stuff");
  ta_iri_set_port (iri3, 22);

  /* When I get the string representatino of these iris */
  str1 = ta_iri_to_string (iri1);
  str2 = ta_iri_to_string (iri2);
  str3 = ta_iri_to_string (iri3);

  /* I see that we got the right values */
  fail_unless (strcmp (str1, "http://comum.org/this/is/a/test?p1=v1") == 0,
               "Wrong iri generated");
  fail_unless (strcmp (str2, "ssh://lincoln@guake.org") == 0,
               "Wrong iri generated");
  fail_unless (strcmp (str3, "minaslivre.org:22#stuff") == 0,
               "Wrong iri generated");
}
END_TEST


Suite *
iri_suite ()
{
  Suite *s = suite_create ("taningia::iri");
  TCase *tc_core = tcase_create ("Core");
  tcase_add_test (tc_core, test_iri_setters_getters);
  tcase_add_test (tc_core, test_iri_to_string);
  suite_add_tcase (s, tc_core);
  return s;
}
