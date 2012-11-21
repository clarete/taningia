/* check.c - This file is part of libtaningia
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


#include <stdlib.h>
#include <check.h>
#include <taningia/global.h>

Suite *list_suite (void);
Suite *iri_suite (void);
Suite *error_suite (void);
Suite *buf_suite (void);

int
main (void)
{
  int number_failed;
  SRunner *sr;

  /* initializing global state */
  ta_global_state_setup ();

  sr = srunner_create (list_suite ());
  srunner_add_suite(sr, iri_suite ());
  srunner_add_suite(sr, error_suite ());
  srunner_add_suite(sr, buf_suite ());

  srunner_run_all (sr, CK_NORMAL);
  number_failed = srunner_ntests_failed (sr);
  srunner_free (sr);

  /* Setting global state down */
  ta_global_state_teardown ();

  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
