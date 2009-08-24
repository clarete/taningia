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

typedef struct _TError TError;

TError      *t_error_new          (const char *domain_name);

void         t_error_free         (TError *error);

int          t_error_get_code     (TError *error);

void         t_error_set_code     (TError *error, int code);

const char  *t_error_get_message  (TError *error);

void         t_error_set_message  (TError *error, const char *message, ...);

#endif /* _TANINGIA_ERROR_H_ */
