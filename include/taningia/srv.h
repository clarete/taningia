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

#include <sys/types.h>
#include <taningia/object.h>
#include <taningia/list.h>

typedef struct
{
  ta_object_t parent;
  u_int32_t ttl;
  u_int16_t _class;        /* avoiding clash with c++ reserved word */
  u_int16_t priority;
  u_int16_t weight;
  u_int16_t port;
  char *host;
} ta_srv_target_t;

int ta_srv_init (void);
ta_list_t *ta_srv_query_domain (const char *name, const char *domain);

const char *ta_srv_target_get_host (ta_srv_target_t *target);
u_int16_t ta_srv_target_get_port (ta_srv_target_t *target);
u_int16_t ta_srv_target_get_weight (ta_srv_target_t *target);
u_int16_t ta_srv_target_get_class (ta_srv_target_t *target);
u_int16_t ta_srv_target_get_priority (ta_srv_target_t *target);
u_int32_t ta_srv_target_get_ttl (ta_srv_target_t *target);

#ifdef __cplusplus
}
#endif

#endif /* _TANINGIA_XMPP_H_ */
