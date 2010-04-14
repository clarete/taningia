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

#include <taningia/object.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_DATE_SIZE 64

typedef struct _ta_log ta_log_t;

typedef enum _ta_log_level ta_log_level_t;

typedef int *(*ta_log_handler_func_t) (ta_log_t *, ta_log_level_t,
                                       const char *, void *);

enum _ta_log_level {
  TA_LOG_DEBUG,
  TA_LOG_INFO,
  TA_LOG_WARN,
  TA_LOG_ERROR,
  TA_LOG_CRITICAL
};

struct _ta_log
{
  ta_object_t parent;
  char *name;
  ta_log_level_t level;
  ta_log_handler_func_t handler;
  int use_colors;
  void *handler_data;
  char *date_format;
};

/**
 * @name: ta_log::new
 * @type: allocator
 */
ta_log_t *ta_log_new (const char *domain_name);

/**
 * @name: ta_log::init
 * @type: constructor
 */
void ta_log_init (ta_log_t *log, const char *domain_name);

/**
 * @name: ta_log::set_use_colors
 * @type: setter
 */
void ta_log_set_use_colors (ta_log_t *log, int use_colors);

/**
 * @name: ta_log::get_use_colors
 * @type: getter
 */
int ta_log_get_use_colors (ta_log_t *log);

/**
 * @name: ta_log::set_level
 * @type: setter
 */
void ta_log_set_level (ta_log_t *log, ta_log_level_t level);

/**
 * @name: ta_log::get_level
 * @type: getter
 */
ta_log_level_t ta_log_get_level (ta_log_t *log);

/**
 * @name: ta_log::set_handler
 * @type: method
 * @param handler: The handler function
 * @param user_data: Parameter to be passed to the handler function
 *
 * Sets a hook to be called when any of the info, warn, debug, error
 * or critical methods of a log instance is called.
 */
void ta_log_set_handler (ta_log_t *log, ta_log_handler_func_t handler,
                         void *user_data);

/**
 * @name: ta_log::info
 * @type: method
 *
 * Log an info message.
 */
void ta_log_info (ta_log_t *log, const char *fmt, ...);

/**
 * @name: ta_log::warn
 * @type: method
 *
 * Log a warning.
 */
void ta_log_warn (ta_log_t *log, const char *fmt, ...);

/**
 * @name: ta_log::debug
 * @type: method
 *
 * Log a debug message.
 */
void ta_log_debug (ta_log_t *log, const char *fmt, ...);

/**
 * @name: ta_log::error
 * @type: method
 *
 * Log an error message.
 */
void ta_log_error (ta_log_t *log, const char *fmt, ...);

/**
 * @name: ta_log::critical
 * @type: method
 *
 * Log a critical message.
 */
void ta_log_critical (ta_log_t *log, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* _TANINGIA_TA_LOG_H_ */
