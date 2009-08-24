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

#ifndef _TANINGIA_XMPP_H_
#define _TANINGIA_XMPP_H_

#include <iksemel.h>
#include <taningia/taningia.h>
#include <taningia/filter.h>
#include <taningia/log.h>
#include <taningia/error.h>

typedef struct _TXmpp TXmpp;

TXmpp             *t_xmpp_new                      (const char          *jid,
                                                    const char          *password,
                                                    const char          *host _optional_,
                                                    int                  port _optional_);
 
void               t_xmpp_free                     (TXmpp               *ctx);
 
const char        *t_xmpp_get_jid                  (TXmpp               *ctx);

void               t_xmpp_set_jid                  (TXmpp               *ctx,
                                                    const char          *jid);

const char        *t_xmpp_get_password             (TXmpp               *ctx);

void               t_xmpp_set_password             (TXmpp               *ctx,
                                                    const char          *password);

const char        *t_xmpp_get_host                 (TXmpp               *ctx);

void               t_xmpp_set_host                 (TXmpp               *ctx,
                                                    const char          *host);

int                t_xmpp_get_port                 (TXmpp               *ctx);

void               t_xmpp_set_port                 (TXmpp               *ctx,
                                                    int                  port);

TFilter           *t_xmpp_get_filter_events        (TXmpp               *ctx);

TFilter           *t_xmpp_get_filter_ids           (TXmpp               *ctx);

TLog              *t_xmpp_get_logger               (TXmpp               *ctx);

TError            *t_xmpp_get_error                (TXmpp               *ctx);

int                t_xmpp_send                     (TXmpp               *ctx,
                                                    iks                 *node);

int                t_xmpp_run                      (TXmpp               *ctx);

void               t_xmpp_stop                     (TXmpp               *ctx);

int                t_xmpp_reconnect                (TXmpp               *ctx);

int                t_xmpp_is_running               (TXmpp               *ctx);

#endif /* _TANINGIA_XMPP_H_ */
