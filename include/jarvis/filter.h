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

#ifndef _JARVIS_FILTER_H_
#define _JARVIS_FILTER_H_

typedef struct _JFilter JFilter;

typedef int (*JFilterCallback) (void *, void *, void *);

JFilter            *j_filter_new                   (void                *data);

void                j_filter_free                  (JFilter             *filter);

void                j_filter_add                   (JFilter             *filter,
                                                    const char          *name,
                                                    JFilterCallback      cb,
                                                    void                *data);

int                 j_filter_call                  (JFilter             *filter,
                                                    const char          *name,
                                                    void                *data);

#endif /* _JARVIS_FILTER_H_ */