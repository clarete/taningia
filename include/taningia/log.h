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

typedef struct _ta_log_t ta_log_t;

typedef enum {
  TA_LOG_DEBUG,
  TA_LOG_INFO,
  TA_LOG_WARN,
  TA_LOG_ERROR,
  TA_LOG_CRITICAL
} ta_log_level_t;

typedef int *(*ta_log_handler_func_t) (ta_log_t *, ta_log_level_t,
                                       const char *, void *);

/**
 * @name: ta_log_new
 * @type: constructor ta_log
 */
ta_log_t *ta_log_new (const char *domain_name);

/**
 * @name: ta_log_free
 * @type: destructor ta_log
 */
void ta_log_free (ta_log_t *log);

/**
 * @name: ta_log_set_use_colors
 * @type: setter ta_log:use_colors
 */
void ta_log_set_use_colors (ta_log_t *log, int use_colors);

/**
 * @name: ta_log_get_use_colors
 * @type: getter ta_log:use_colors
 */
int ta_log_get_use_colors (ta_log_t *log);

/**
 * @name: ta_log_set_level
 * @type: setter ta_log:level
 */
void ta_log_set_level (ta_log_t *log, ta_log_level_t level);

/**
 * @name: ta_log_get_level
 * @type: getter ta_log:level
 */
ta_log_level_t ta_log_get_level (ta_log_t *log);

/**
 * @name: ta_log_set_handler
 * @type: method ta_log
 * @param handler: The handler function
 * @param user_data: Parameter to be passed to the handler function
 *
 * Sets a hook to be called when any of the info, warn, debug, error
 * or critical methods of a log instance is called.
 */
void ta_log_set_handler (ta_log_t *log, ta_log_handler_func_t handler,
                         void *user_data);

/**
 * @name: ta_log_info
 * @type: method ta_log
 *
 * Log an info message.
 */
void ta_log_info (ta_log_t *log, const char *fmt, ...);

/**
 * @name: ta_log_warn
 * @type: method ta_log
 *
 * Log a warning.
 */
void ta_log_warn (ta_log_t *log, const char *fmt, ...);

/**
 * @name: ta_log_debug
 * @type: method ta_log
 *
 * Log a debug message.
 */
void ta_log_debug (ta_log_t *log, const char *fmt, ...);

/**
 * @name: ta_log_error
 * @type: method ta_log
 *
 * Log an error message.
 */
void ta_log_error (ta_log_t *log, const char *fmt, ...);

/**
 * @name: ta_log_critical
 * @type: method ta_log
 *
 * Log a critical message.
 */
void ta_log_critical (ta_log_t *log, const char *fmt, ...);

#endif /* _TANINGIA_TA_LOG_H_ */
