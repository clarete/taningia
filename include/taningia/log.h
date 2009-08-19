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

#ifndef _TANINGIA_LOG_H_
#define _TANINGIA_LOG_H_

typedef struct _TLog TLog;

typedef enum {
  TLOG_INFO     = 1 << 1,
  TLOG_WARN     = 1 << 2,
  TLOG_DEBUG    = 1 << 3,
  TLOG_CRITICAL = 1 << 4,
  TLOG_ERROR    = 1 << 5,
} TLogLevel;

typedef int *(*TLogHandlerFunc) (TLog *log, TLogLevel level, const char *);

TLog     *t_log_new            (const char *domain_name);

void      t_log_free           (TLog *log);

void      t_log_set_handler    (TLog *log, TLogHandlerFunc handler);

void      t_log_set_use_colors (TLog *log, int use_colors);

int       t_log_get_use_colors (TLog *log);

void      t_log_set_level      (TLog *log, TLogLevel level);

TLogLevel t_log_get_level      (TLog *log);

void      t_log_info           (TLog *log, const char *fmt, ...);

void      t_log_warn           (TLog *log, const char *fmt, ...);

void      t_log_debug          (TLog *log, const char *fmt, ...);

void      t_log_error          (TLog *log, const char *fmt, ...);

void      t_log_critical       (TLog *log, const char *fmt, ...);

#endif /* _TANINGIA_LOG_H_ */
