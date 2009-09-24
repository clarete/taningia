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

struct _log_t
{
  char *name;
  log_level_t level;
  log_handler_func_t handler;
  int use_colors;
  void *handler_data;
  char *date_format;
};

log_t *
log_new (const char *domain_name)
{
  log_t *log;
  log = malloc (sizeof (log_t));
  log->name = strdup (domain_name);
  log->level = LOG_CRITICAL | LOG_ERROR | LOG_WARN;
  log->handler = NULL;
  log->handler_data = NULL;
  log->use_colors = 0;
  log->date_format = strdup ("%x %X");
  return log;
}

void
log_free (log_t *log)
{
  free (log->name);
  free (log->date_format);
  free (log);
}

void
log_set_handler (log_t *log, log_handler_func_t handler, void *user_data)
{
  log->handler = handler;
  log->handler_data = user_data;
}

void
log_set_use_colors (log_t *log, int use_colors)
{
  log->use_colors = use_colors;
}

int
log_get_use_colors (log_t *log)
{
  return log->use_colors;
}

void
log_set_level (log_t *log, log_level_t level)
{
  log->level = level;
}

log_level_t
log_get_level (log_t *log)
{
  return log->level;
}

void
log_set_date_format (log_t *log, const char * date_format)
{
  if (log->date_format)
    free (log->date_format);
  log->date_format = strdup (date_format);
}

const char *
log_get_date_format (log_t *log)
{
  return log->date_format;
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

static char *
_log_localtime (log_t * log)
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
log_info (log_t *log, const char *fmt, ...)
{
  char *log_time = _log_localtime (log);

  if (!(log->level & LOG_INFO))
    return;

  get_message (fmt, argp);
  if (msg == NULL)
    return;

  if (log->handler)
    if (log->handler (log, LOG_INFO, msg, log->handler_data))
      return;

  if (!log->use_colors)
    fprintf (stderr, "[  INFO ] [ %s ] %s\n", log_time, msg);
  else
    /* Cyan */
    fprintf (stderr,
             "\033[;36m[  INFO ] [ %s ]\033[;0m \033[;0m%s\033[;0m\n",
             log_time,
             msg);

  free (log_time);
}

void
log_warn (log_t *log, const char *fmt, ...)
{
  char *log_time = _log_localtime (log);

  if (!(log->level & LOG_WARN))
    return;

  get_message (fmt, argp);
  if (msg == NULL)
    return;

  if (log->handler)
    if (log->handler (log, LOG_WARN, msg, log->handler_data))
      return;

  if (!log->use_colors)
    fprintf (stderr, "[  WARN ] [ %s ] %s\n", log_time, msg);
  else
    /* Yellow */
    fprintf (stderr,
             "\033[;33m[  WARN ] [ %s ]\033[;0m \033[;0m%s\033[;0m\n",
             log_time,
             msg);

  free (log_time);
}

void
log_debug (log_t *log, const char *fmt, ...)
{
  char *log_time = _log_localtime (log);

  if (!(log->level & LOG_DEBUG))
    return;

  get_message (fmt, argp);
  if (msg == NULL)
    return;

  if (log->handler)
    if (log->handler (log, LOG_DEBUG, msg, log->handler_data))
      return;

  if (!log->use_colors)
    fprintf (stderr, "[ DEBUG ] [ %s ] %s\n", log_time, msg);
  else
    /* Blue */
    fprintf (stderr,
             "\033[;34m[ DEBUG ] [ %s ]\033[;0m \033[0;0m%s\033[;0m\n",
             log_time,
             msg);

  free (log_time);
}

void
log_critical (log_t *log, const char *fmt, ...)
{
  char *log_time = _log_localtime (log);

  if (!(log->level & LOG_CRITICAL))
    return;

  get_message (fmt, argp);
  if (msg == NULL)
    return;

  if (log->handler)
    if (log->handler (log, LOG_CRITICAL, msg, log->handler_data))
      return;

  if (!log->use_colors)
    fprintf (stderr, "[ CRITI ] [ %s ] %s\n", log_time, msg);
  else
    /* Red with no bold */
    fprintf (stderr,
             "\033[;31m[ CRITI ] [ %s ]\033[;0m \033[;0m%s\033[;0m\n",
             log_time,
             msg);

  free (log_time);
}

void
log_error (log_t *log, const char *fmt, ...)
{
  char *log_time = _log_localtime (log);

  if (!(log->level & LOG_ERROR))
    return;

  get_message (fmt, argp);
  if (msg == NULL)
    return;

  if (log->handler)
    if (log->handler (log, LOG_ERROR, msg, log->handler_data))
      return;

  if (!log->use_colors)
    fprintf (stderr, "[ ERROR ] [ %s ] %s\n", log_time, msg);
  else
    /* Red foreground */
    fprintf (stderr,
             "\033[1;31m[ ERROR ] [ %s ]\033[1;0m \033[1;1m%s\033[1;0m\n",
             log_time,
             msg);

  free (log_time);
}
