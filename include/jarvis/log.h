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

#ifndef _JARVIS_LOG_H_
#define _JARVIS_LOG_H_

typedef struct _JLog JLog;

typedef enum {
  JLOG_INFO     = 1 << 1,
  JLOG_WARN     = 1 << 2,
  JLOG_DEBUG    = 1 << 3,
  JLOG_CRITICAL = 1 << 4,
  JLOG_ERROR    = 1 << 5,
} JLogLevel;

typedef int *(*JLogHandlerFunc) (JLog *log, JLogLevel level, const char *);

JLog     *j_log_new            (const char *domain_name);

void      j_log_free           (JLog *log);

void      j_log_set_handler    (JLog *log, JLogHandlerFunc handler);

void      j_log_set_use_colors (JLog *log, int use_colors);

int       j_log_get_use_colors (JLog *log);

void      j_log_set_level      (JLog *log, JLogLevel level);

JLogLevel j_log_get_level      (JLog *log);

void      j_log_info           (JLog *log, const char *fmt, ...);

void      j_log_warn           (JLog *log, const char *fmt, ...);

void      j_log_debug          (JLog *log, const char *fmt, ...);

void      j_log_error          (JLog *log, const char *fmt, ...);

void      j_log_critical       (JLog *log, const char *fmt, ...);

#endif /* _JARVIS_LOG_H_ */
