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

typedef struct _TXmppClient TXmppClient;

TXmppClient       *t_xmpp_client_new                  (const char       *jid,
                                                       const char       *password,
                                                       const char       *host _optional_,
                                                       int               port _optional_);
 
void               t_xmpp_client_free                 (TXmppClient      *ctx);

const char        *t_xmpp_client_get_jid              (TXmppClient      *ctx);

void               t_xmpp_client_set_jid              (TXmppClient      *ctx,
                                                       const char       *jid);

const char        *t_xmpp_client_get_password         (TXmppClient      *ctx);

void               t_xmpp_client_set_password         (TXmppClient      *ctx,
                                                       const char       *password);

const char        *t_xmpp_client_get_host             (TXmppClient      *ctx);

void               t_xmpp_client_set_host             (TXmppClient      *ctx,
                                                       const char       *host);

int                t_xmpp_client_get_port              (TXmppClient     *ctx);

void               t_xmpp_client_set_port              (TXmppClient     *ctx,
                                                        int              port);

TFilter           *t_xmpp_client_get_filter_events     (TXmppClient     *ctx);

TFilter           *t_xmpp_client_get_filter_ids        (TXmppClient     *ctx);

TLog              *t_xmpp_client_get_logger            (TXmppClient     *ctx);

TError            *t_xmpp_client_get_error             (TXmppClient     *ctx);

int                t_xmpp_client_send                  (TXmppClient     *ctx,
                                                        iks             *node);

int                t_xmpp_client_run                   (TXmppClient     *ctx);

void               t_xmpp_client_stop                  (TXmppClient     *ctx);

int                t_xmpp_client_reconnect             (TXmppClient     *ctx);

int                t_xmpp_client_is_running            (TXmppClient     *ctx);

#endif /* _TANINGIA_XMPP_CLIENT_H_ */
