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
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <jarvis/log.h>

struct _JLog
{
  char *name;
  JLogLevel level;
  JLogHandlerFunc handler;
  int use_colors;
};

JLog *
j_log_new (const char *domain_name)
{
  JLog *log;
  log = malloc (sizeof (JLog));
  log->name = strdup (domain_name);
  log->level = JLOG_CRITICAL | JLOG_ERROR | JLOG_WARN;
  log->handler = NULL;
  log->use_colors = 0;
  return log;
}

void
j_log_free (JLog *log)
{
  free (log->name);
  free (log);
}

void
j_log_set_handler (JLog *log, JLogHandlerFunc handler)
{
  log->handler = handler;
}

void
j_log_set_use_colors (JLog *log, int use_colors)
{
  log->use_colors = use_colors;
}

int
j_log_get_use_colors (JLog *log)
{
  return log->use_colors;
}

void
j_log_set_level (JLog *log, JLogLevel level)
{
  log->level = level;
}

JLogLevel
j_log_get_level (JLog *log)
{
  return log->level;
}

/* I had problems to make it as a simple static function because of
 * the varargs stuff. I'll try to implement it better soon */
#define get_message(fmt, argp)                          \
  int n, size = 50;                                     \
  char *msg, *np;                                       \
  va_list argp;                                         \
                                                        \
  if ((msg = malloc (size)) == NULL)                    \
    msg = NULL;                                         \
  else                                                  \
    while (1)                                           \
      {                                                 \
        va_start (argp, fmt);                           \
        n = vsnprintf (msg, size, fmt, argp);           \
        va_end (argp);                                  \
        if (n > -1 && n < size)                         \
          break;                                        \
        if (n > -1)                                     \
          size = n+1;                                   \
        else                                            \
          size *= 2;                                    \
        if ((np = realloc (msg, size)) == NULL)         \
          {                                             \
            free (msg);                                 \
            msg = NULL;                                 \
            break;                                      \
          }                                             \
        else                                            \
          msg = np;                                     \
      }

void
j_log_info (JLog *log, const char *fmt, ...)
{
  if (!(log->level & JLOG_INFO))
    return;

  get_message (fmt, argp);
  if (msg == NULL)
    return;

  if (log->handler)
    if (log->handler (log, JLOG_INFO, msg))
      return;

  if (log->use_colors)
    /* Cyan */
    fprintf (stderr, "\033[;36m[  INFO]\033[;0m \033[;0m%s\033[;0m\n", msg);
  else
    fprintf (stderr, "%s\n", msg);
}

void
j_log_warn (JLog *log, const char *fmt, ...)
{
  if (!(log->level & JLOG_WARN))
    return;

  get_message (fmt, argp);
  if (msg == NULL)
    return;

  if (log->handler)
    if (log->handler (log, JLOG_WARN, msg))
      return;

  if (!log->use_colors)
    fprintf (stderr, "%s\n", msg);
  else
    /* Yellow */
    fprintf (stderr, "\033[;33m[  WARN]\033[;0m \033[;0m%s\033[;0m\n", msg);
}

void
j_log_debug (JLog *log, const char *fmt, ...)
{
  if (!(log->level & JLOG_DEBUG))
    return;

  get_message (fmt, argp);
  if (msg == NULL)
    return;

  if (log->handler)
    if (log->handler (log, JLOG_DEBUG, msg))
      return;

  if (!log->use_colors)
    fprintf (stderr, "%s\n", msg);
  else
    /* Blue */
    fprintf (stderr, "\033[;34m[ DEBUG]\033[;0m \033[0;0m%s\033[;0m\n", msg);
}

void
j_log_critical (JLog *log, const char *fmt, ...)
{
  if (!(log->level & JLOG_CRITICAL))
    return;

  get_message (fmt, argp);
  if (msg == NULL)
    return;

  if (log->handler)
    if (log->handler (log, JLOG_CRITICAL, msg))
      return;

  if (!log->use_colors)
    fprintf (stderr, "%s\n", msg);
  else
    /* Red with no bold */
    fprintf (stderr, "\033[;31m[ CRITI]\033[;0m \033[;0m%s\033[;0m\n", msg);
}

void
j_log_error (JLog *log, const char *fmt, ...)
{
  if (!(log->level & JLOG_ERROR))
    return;

  get_message (fmt, argp);
  if (msg == NULL)
    return;

  if (log->handler)
    if (log->handler (log, JLOG_ERROR, msg))
      return;

  if (!log->use_colors)
    fprintf (stderr, "%s\n", msg);
  else
    /* Red foreground */
    fprintf (stderr, "\033[1;31m[ ERROR]\033[1;0m \033[1;1m%s\033[1;0m\n", msg);
}
