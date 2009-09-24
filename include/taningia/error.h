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

#include <taningia/taningia.h>

typedef struct _error_t error_t;

/**
 * @name: error_new
 * @type: constructor error
 */
error_t *error_new (void);

/**
 * @name: error_free
 * @type: destructor error
 */
void error_free (error_t *error);

/**
 * @name: error_get_code
 * @type: getter error:code
 */
int error_get_code (error_t *error);

/**
 * @name: error_set_code
 * @type: setter error:code
 */
void error_set_code (error_t *error, int code);

/**
 * @name: error_get_name
 * @type: getter error:name
 */
const char *error_get_name (error_t *error);

/**
 * @name: error_set_name
 * @type: setter error:name
 */
void error_set_name (error_t *error, const char *name);

/**
 * @name: error_get_message
 * @type: getter error:message
 */
const char *error_get_message (error_t *error);

/**
 * @name: error_set_message
 * @type: setter error:message
 */
void error_set_message (error_t *error, const char *message, ...);

/**
 * @name: error_set_full
 * @type: method error
 *
 * A shortcut to set all error attributes at once.
 */
void error_set_full (error_t *error, int code, const char *name,
                     const char *message, ...);

#endif /* _TANINGIA_ERROR_H_ */
