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

#ifndef _TANINGIA_ERROR_H_
#define _TANINGIA_ERROR_H_

typedef struct _ta_error_t ta_error_t;

/**
 * @name: ta_error_new
 * @type: constructor ta_error
 */
ta_error_t *ta_error_new (void);

/**
 * @name: ta_error_free
 * @type: destructor ta_error
 */
void ta_error_free (ta_error_t *error);

/**
 * @name: ta_error_get_code
 * @type: getter ta_error:code
 */
int ta_error_get_code (ta_error_t *error);

/**
 * @name: ta_error_set_code
 * @type: setter ta_error:code
 */
void ta_error_set_code (ta_error_t *error, int code);

/**
 * @name: ta_error_get_name
 * @type: getter ta_error:name
 */
const char *ta_error_get_name (ta_error_t *error);

/**
 * @name: ta_error_set_name
 * @type: setter ta_error:name
 */
void ta_error_set_name (ta_error_t *error, const char *name);

/**
 * @name: ta_error_get_message
 * @type: getter ta_error:message
 */
const char *ta_error_get_message (ta_error_t *error);

/**
 * @name: ta_error_set_message
 * @type: setter ta_error:message
 */
void ta_error_set_message (ta_error_t *error, const char *message, ...);

/**
 * @name: ta_error_set_full
 * @type: method ta_error
 *
 * A shortcut to set all error attributes at once.
 */
void ta_error_set_full (ta_error_t *error, int code, const char *name,
                        const char *message, ...);

#endif /* _TANINGIA_ERROR_H_ */
