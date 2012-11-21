/* check_buf.c - This file is part of the taningia library
 *
 * Copyright (C) 2012  Lincoln de Sousa <lincoln@comum.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>

#include <check.h>
#include <stdarg.h>
#include <string.h>
#include <taningia/buf.h>


/* This is not a public function but we definitely have to test it */
extern int _buf_realloc (ta_buf_t *b, int requested_size);


START_TEST (test_buf_initial_values)
{
  /* Given that I have an empty buffer */
  ta_buf_t b = TA_BUF_INIT;

  /* When I initialize it */
  ta_buf_alloc (&b, 0);

  /* Then I see that all the values of my buffer structure were filled
   * properly */
  fail_if (b.ptr != NULL, "Internal ptr should be initialized with NULL");
  fail_if (b.string_length != 0, "The internal ptr is NULL, string_length should be 0");
  fail_if (b.allocated_size != 0, "The required size is 0, so allocated_size should be 0");
}
END_TEST


START_TEST (test_buf_reallocation)
{
  /* Given that I have an empty buffer */
  ta_buf_t b = TA_BUF_INIT;

  /* When I initialize it with a given size */
  ta_buf_alloc (&b, 5);

  /* Then I see that the internal allocated size should be multiplied
   * for 1.5 and aligned to be multiple of 8 */
  fail_if (b.allocated_size != 8);

  /* When I realloc it again */
  _buf_realloc (&b, 35);

  /* Then I see that the allocated size was both multiplied and
   * aligned */
  fail_if (b.allocated_size != 48);
}
END_TEST


START_TEST (test_buf_cat)
{
  /* Given that I have a new buffer */
  ta_buf_t b = TA_BUF_INIT;
  ta_buf_alloc (&b, 8);

  /* When I concatenate some text */
  ta_buf_cat (&b, "some stuff");
  ta_buf_cat (&b, " some other stuff");

  /* Then I see that the buffer has everything that I appended */
  fail_if (ta_buf_cstr (&b) == NULL);
  fail_unless (strcmp (ta_buf_cstr (&b), "some stuff some other stuff") == 0,
               "Error while concatenating strings");

  /* Cleaning up the mess */
  ta_buf_dealloc (&b);
}
END_TEST


static void
_proxy_to_test_vcatf (ta_buf_t *b, const char *format, ...)
{
  va_list args;
  va_start (args, format);
  ta_buf_vcatf (b, format, args);
  va_end (args);
}


START_TEST (test_buf_vcatf)
{
  /* Given that I have a new buffer */
  ta_buf_t b = TA_BUF_INIT;
  ta_buf_alloc (&b, 32);

  /* When I call a proxy varargs enabled function to concatenate some
   * formatted text */
  _proxy_to_test_vcatf (&b, "name: %s, age: %d", "lincoln", 26);

  /* Then I see that the buffer has the correct data */
  fail_unless (strcmp (ta_buf_cstr (&b), "name: lincoln, age: 26") == 0,
               "Error while concatenating formatted strings vcatf");

  /* Cleaning up the mess */
  ta_buf_dealloc (&b);
}
END_TEST


START_TEST (test_buf_catf)
{
  /* Given that I have a new buffer */
  ta_buf_t b = TA_BUF_INIT;
  ta_buf_alloc (&b, 32);

  /* When I concatenate some text */
  ta_buf_catf (&b, "name: %s, ", "lincoln");
  ta_buf_catf (&b, "age: %d", 26);

  /* Then I see that the buffer has everything that I appended */
  fail_unless (strcmp (ta_buf_cstr (&b), "name: lincoln, age: 26") == 0,
               "Error while concatenating formatted strings with catf");

  /* Cleaning up the mess */
  ta_buf_dealloc (&b);
}
END_TEST


START_TEST (test_buf_dealloc)
{
  /* Given that I have a buffer with some stuff inside */
  ta_buf_t b = TA_BUF_INIT;
  ta_buf_alloc (&b, 32);
  ta_buf_cat (&b, "stuff stuff stuff");

  /* When I dealloc it */
  ta_buf_dealloc (&b);

  /* Then I see that it was reset */
  fail_if (b.string_length != 0, "The string length must be 0 after deallocating a buffer");
  fail_if (b.allocated_size != 0, "The allocated size must be 0 after deallocating a buffer");
  fail_if (b.ptr != NULL, "The string pointer should be NULL after deallocating a buffer");
}
END_TEST


Suite *
buf_suite ()
{
  Suite *s = suite_create ("taningia::buf");
  TCase *tc_core = tcase_create ("Core");
  tcase_add_test (tc_core, test_buf_initial_values);
  tcase_add_test (tc_core, test_buf_reallocation);
  tcase_add_test (tc_core, test_buf_cat);
  tcase_add_test (tc_core, test_buf_vcatf);
  tcase_add_test (tc_core, test_buf_catf);
  suite_add_tcase (s, tc_core);
  return s;
}
