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

#ifndef _TANINGIA_LOG_H_
#define _TANINGIA_LOG_H_

#define MAX_DATE_SIZE 64

typedef struct _log_t log_t;

typedef enum {
  LOG_INFO     = 1 << 1,
  LOG_WARN     = 1 << 2,
  LOG_DEBUG    = 1 << 3,
  LOG_CRITICAL = 1 << 4,
  LOG_ERROR    = 1 << 5,
} log_level_t;

typedef int *(*log_handler_func_t) (log_t *, log_level_t, const char *, void *);

/**
 * @name: log_new
 * @type: constructor log
 */
log_t *log_new (const char *domain_name);

/**
 * @name: log_free
 * @type: destructor log
 */
void log_free (log_t *log);

/**
 * @name: log_set_use_colors
 * @type: setter log:use_colors
 */
void log_set_use_colors (log_t *log, int use_colors);

/**
 * @name: log_get_use_colors
 * @type: getter log:use_colors
 */
int log_get_use_colors (log_t *log);

/**
 * @name: log_set_level
 * @type: setter log:level
 */
void log_set_level (log_t *log, log_level_t level);

/**
 * @name: log_get_level
 * @type: getter log:level
 */
log_level_t log_get_level (log_t *log);

/**
 * @name: log_set_handler
 * @type: method log
 * @param handler: The handler function
 * @param user_data: Parameter to be passed to the handler function
 *
 * Sets a hook to be called when any of the info, warn, debug, error
 * or critical methods of a log instance is called.
 */
void log_set_handler (log_t *log, log_handler_func_t handler, void *user_data);

/**
 * @name: log_info
 * @type: method log
 *
 * Log an info message.
 */
void log_info (log_t *log, const char *fmt, ...);

/**
 * @name: log_warn
 * @type: method log
 *
 * Log a warning.
 */
void log_warn (log_t *log, const char *fmt, ...);

/**
 * @name: log_debug
 * @type: method log
 *
 * Log a debug message.
 */
void log_debug (log_t *log, const char *fmt, ...);

/**
 * @name: log_error
 * @type: method log
 *
 * Log an error message.
 */
void log_error (log_t *log, const char *fmt, ...);

/**
 * @name: log_critical
 * @type: method log
 *
 * Log a critical message.
 */
void log_critical (log_t *log, const char *fmt, ...);

#endif /* _TANINGIA_LOG_H_ */
