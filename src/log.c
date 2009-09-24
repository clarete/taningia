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
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <taningia/log.h>
#include <time.h>

struct _ta_log_t
{
  char *name;
  ta_log_level_t level;
  ta_log_handler_func_t handler;
  int use_colors;
  void *handler_data;
  char *date_format;
};

ta_log_t *
ta_log_new (const char *domain_name)
{
  ta_log_t *log;
  log = malloc (sizeof (ta_log_t));
  log->name = strdup (domain_name);
  log->level = TA_LOG_CRITICAL | TA_LOG_ERROR | TA_LOG_WARN;
  log->handler = NULL;
  log->handler_data = NULL;
  log->use_colors = 0;
  log->date_format = strdup ("%x %X");
  return log;
}

void
ta_log_free (ta_log_t *log)
{
  free (log->name);
  free (log->date_format);
  free (log);
}

void
ta_log_set_handler (ta_log_t *log, ta_log_handler_func_t handler, void *user_data)
{
  log->handler = handler;
  log->handler_data = user_data;
}

void
ta_log_set_use_colors (ta_log_t *log, int use_colors)
{
  log->use_colors = use_colors;
}

int
ta_log_get_use_colors (ta_log_t *log)
{
  return log->use_colors;
}

void
ta_log_set_level (ta_log_t *log, ta_log_level_t level)
{
  log->level = level;
}

ta_log_level_t
ta_log_get_level (ta_log_t *log)
{
  return log->level;
}

void
ta_log_set_date_format (ta_log_t *log, const char * date_format)
{
  if (log->date_format)
    free (log->date_format);
  log->date_format = strdup (date_format);
}

const char *
ta_log_get_date_format (ta_log_t *log)
{
  return log->date_format;
}

/* I had problems to make it as a simple static function because of
 * the varargs stuff. I'll try to implement it better soon */
#define get_message(fmt, argp)                  \
  int n, size = 50;                             \
  char *msg, *np;                               \
  va_list argp;                                 \
                                                \
  if ((msg = malloc (size)) == NULL)            \
    msg = NULL;                                 \
  else                                          \
    while (1)                                   \
      {                                         \
        va_start (argp, fmt);                   \
        n = vsnprintf (msg, size, fmt, argp);   \
        va_end (argp);                          \
        if (n > -1 && n < size)                 \
          break;                                \
        if (n > -1)                             \
          size = n+1;                           \
        else                                    \
          size *= 2;                            \
        if ((np = realloc (msg, size)) == NULL) \
          {                                     \
            free (msg);                         \
            msg = NULL;                         \
            break;                              \
          }                                     \
        else                                    \
          msg = np;                             \
      }

static char *
_ta_log_localtime (ta_log_t * log)
{
  struct tm * timeinfo;
  time_t rawtime = time (NULL);
  char *buffer = malloc (MAX_DATE_SIZE);

  time (&rawtime);
  timeinfo = localtime (&rawtime);

  if (log->date_format == NULL)
    return NULL;

  if (strftime (buffer, MAX_DATE_SIZE, log->date_format, timeinfo))
    return buffer;

  fprintf (stderr, "Oh god, fix it! Date format too long!");
  return NULL;
}

void
ta_log_info (ta_log_t *log, const char *fmt, ...)
{
  char *ta_log_time = _ta_log_localtime (log);

  if (!(log->level & TA_LOG_INFO))
    return;

  get_message (fmt, argp);
  if (msg == NULL)
    return;

  if (log->handler)
    if (log->handler (log, TA_LOG_INFO, msg, log->handler_data))
      return;

  if (!log->use_colors)
    fprintf (stderr, "[  INFO ] [ %s ] %s\n", ta_log_time, msg);
  else
    /* Cyan */
    fprintf (stderr,
             "\033[;36m[  INFO ] [ %s ]\033[;0m \033[;0m%s\033[;0m\n",
             ta_log_time,
             msg);

  free (ta_log_time);
}

void
ta_log_warn (ta_log_t *log, const char *fmt, ...)
{
  char *ta_log_time = _ta_log_localtime (log);

  if (!(log->level & TA_LOG_WARN))
    return;

  get_message (fmt, argp);
  if (msg == NULL)
    return;

  if (log->handler)
    if (log->handler (log, TA_LOG_WARN, msg, log->handler_data))
      return;

  if (!log->use_colors)
    fprintf (stderr, "[  WARN ] [ %s ] %s\n", ta_log_time, msg);
  else
    /* Yellow */
    fprintf (stderr,
             "\033[;33m[  WARN ] [ %s ]\033[;0m \033[;0m%s\033[;0m\n",
             ta_log_time,
             msg);

  free (ta_log_time);
}

void
ta_log_debug (ta_log_t *log, const char *fmt, ...)
{
  char *ta_log_time = _ta_log_localtime (log);

  if (!(log->level & TA_LOG_DEBUG))
    return;

  get_message (fmt, argp);
  if (msg == NULL)
    return;

  if (log->handler)
    if (log->handler (log, TA_LOG_DEBUG, msg, log->handler_data))
      return;

  if (!log->use_colors)
    fprintf (stderr, "[ DEBUG ] [ %s ] %s\n", ta_log_time, msg);
  else
    /* Blue */
    fprintf (stderr,
             "\033[;34m[ DEBUG ] [ %s ]\033[;0m \033[0;0m%s\033[;0m\n",
             ta_log_time,
             msg);

  free (ta_log_time);
}

void
ta_log_critical (ta_log_t *log, const char *fmt, ...)
{
  char *ta_log_time = _ta_log_localtime (log);

  if (!(log->level & TA_LOG_CRITICAL))
    return;

  get_message (fmt, argp);
  if (msg == NULL)
    return;

  if (log->handler)
    if (log->handler (log, TA_LOG_CRITICAL, msg, log->handler_data))
      return;

  if (!log->use_colors)
    fprintf (stderr, "[ CRITI ] [ %s ] %s\n", ta_log_time, msg);
  else
    /* Red with no bold */
    fprintf (stderr,
             "\033[;31m[ CRITI ] [ %s ]\033[;0m \033[;0m%s\033[;0m\n",
             ta_log_time,
             msg);

  free (ta_log_time);
}

void
ta_log_error (ta_log_t *log, const char *fmt, ...)
{
  char *ta_log_time = _ta_log_localtime (log);

  if (!(log->level & TA_LOG_ERROR))
    return;

  get_message (fmt, argp);
  if (msg == NULL)
    return;

  if (log->handler)
    if (log->handler (log, TA_LOG_ERROR, msg, log->handler_data))
      return;

  if (!log->use_colors)
    fprintf (stderr, "[ ERROR ] [ %s ] %s\n", ta_log_time, msg);
  else
    /* Red foreground */
    fprintf (stderr,
             "\033[1;31m[ ERROR ] [ %s ]\033[1;0m \033[1;1m%s\033[1;0m\n",
             ta_log_time,
             msg);

  free (ta_log_time);
}
