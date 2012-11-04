/* check_errors.c - This file is part of libtaningia
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
#include <taningia/error.h>


START_TEST (test_error_last_first_call_must_be_null)
{
  /* Given that no error was set before */
  const ta_error_t *error;

  /* When I call the last_error() function */
  error = ta_error_last ();

  /* Then I get a NULL value */
  fail_unless (error == NULL, "The last error must be NULL if no error is set");
}
END_TEST;


START_TEST (test_error_set)
{
  const ta_error_t *error;

  /* Given that I set an error */
  ta_error_set (123, "This is my msg: '%s', %d", "string", 26);

  /* When I get the last error */
  error = ta_error_last ();

  /* Then I see the error I set in the last error pointer */
  fail_unless (error->code == 123, "Wrong error code after ta_error_set()");
  fail_unless (strcmp (error->message, "This is my msg: 'string', 26") == 0,
               "Wrong error message after ta_error_set()");
}
END_TEST;


START_TEST (test_error_set_twice)
{
  const ta_error_t *error;

  /* Given that I set two different errors */
  ta_error_set (123, "This is my msg: '%s', %d", "string", 26);
  ta_error_set (321, "Another %s", "stuff");

  /* When I get the last error */
  error = ta_error_last ();

  /* Then I see the error I set in the last error pointer */
  fail_unless (error->code == 321, "Wrong error code after ta_error_set()");
  fail_unless (strcmp (error->message, "Another stuff") == 0,
               "Wrong error message after ta_error_set()");
}
END_TEST;


START_TEST (test_error_clear)
{
  /* Given that I set an error */
  ta_error_set (321, "Error msg");

  /* When I clear the error */
  ta_error_clear ();

  /* Then I see that no errors are set anymore */
  fail_unless (ta_error_last () == NULL,
               "The last error must be NULL after _clear()");
}
END_TEST;


Suite *
error_suite ()
{
  Suite *s = suite_create ("taningia::error");
  TCase *tc_core = tcase_create ("Core");
  tcase_add_test (tc_core, test_error_last_first_call_must_be_null);
  tcase_add_test (tc_core, test_error_set);
  tcase_add_test (tc_core, test_error_set_twice);
  tcase_add_test (tc_core, test_error_clear);
  suite_add_tcase (s, tc_core);
  return s;
}
