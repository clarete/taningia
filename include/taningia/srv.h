/*
 * Copyright (C) 2010  Lincoln de Sousa <lincoln@minaslivre.org>
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

#ifndef _TANINGIA_SRV_H_
#define _TANINGIA_SRV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <taningia/object.h>
#include <taningia/list.h>

typedef struct {
  ta_object_t parent;
  char *name;
  char *domain;
} ta_srv_resolver_t;

int ta_srv_init (void);

ta_srv_resolver_t *ta_srv_resolver_new (const char *name, const char *domain);
void ta_srv_resolver_init (ta_srv_resolver_t *resolver, const char *name,
                           const char *domain);
ta_list_t *ta_srv_resolver_query_domain (ta_srv_resolver_t *resolver);

#ifdef __cplusplus
}
#endif

#endif /* _TANINGIA_XMPP_H_ */
