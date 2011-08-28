/*
 * Copyright (C) 2009 Lincoln de Sousa <lincoln@minaslivre.org>
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
#include <taningia/log.h>

/* You can attach an arbitrary code to the log system by setting a
 * callback to a log object by calling `ta_log_set_handler'. As we can
 * see, the callback receives the log instance, the level that the
 * message is being fired, the (already formatted) message and a user
 * parameter.
 *
 * PS.: If such a log level is disabled, the handler callback will
 * *NOT* be called.
 */
int
handler (ta_log_t *log, ta_log_level_t level, const char *msg, void *user_data)
{
  printf ("Calling handler callback: %s\n", msg);

  /* If you want to prevent the message to be written in the error
   * output, return a true value here. */
  return 0;
}

int
main (int argc, char **argv)
{
  ta_log_t *log;

  /* All log instances must have a name. It will be useful when we
   * implement such a global log manager. It will provide a way for
   * retrieving an already registered logger. */
  log = ta_log_new ("blah");

  /* Enabling colored outupt that is disabled by default. */
  ta_log_set_use_colors (log, 1);

  /* As explained in the `handler' function declaration, this is the
   * way for attaching a handler callback in the log object. */
  ta_log_set_handler (log, (ta_log_handler_func_t) handler, &argc);

  /* Default log level is TA_LOG_CRITICAL | TA_LOG_ERROR | TA_LOG_WARN, to
   * receive info and debug messages I'll just extend the flag
   * field */
  ta_log_set_level (log, TA_LOG_DEBUG);

  /* Now, the messages... */
  ta_log_debug (log, "Hey, I'm a debug message");
  ta_log_info (log, "Testing, 1, %d, %d, testing", 2, 3);
  ta_log_warn (log, "This is a warning message");
  ta_log_error (log, "Humm, something got screwd... I'm an error message");
  ta_log_critical (log, "hello there, I'm a critical message");

  /* Cleaning our log object */
  ta_object_unref (log);
  return 0;
}
