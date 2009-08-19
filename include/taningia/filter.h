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

#ifndef _TANINGIA_FILTER_H_
#define _TANINGIA_FILTER_H_

#include <taningia/taningia.h>

typedef struct _TFilter TFilter;

typedef int (*TFilterCallback) (void *, void *, void *);

TFilter            *t_filter_new                   (void                *data _optional_);

void                t_filter_free                  (TFilter             *filter);

void               *t_filter_get_data              (TFilter             *filter);

void                t_filter_add                   (TFilter             *filter,
                                                    const char          *name,
                                                    TFilterCallback      cb,
                                                    void                *data _optional_);

int                 t_filter_call                  (TFilter             *filter,
                                                    const char          *name,
                                                    void                *data _optional_);

#endif /* _TANINGIA_FILTER_H_ */
