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

#include <stdio.h>
#include <unistd.h>             /* sleep() */
#include <iksemel.h>
#include <taningia/xmpp.h>
#include <taningia/log.h>

int
connected_cb (void *data, ikspak *pak)
{
  xmpp_client_t *client;
  iks *node;
  client = (xmpp_client_t *) data;
  printf ("connected: =)\n");
  printf ("Sending presence and sleeping some time\n");

  /* Sending presence info */
  node = iks_make_pres (IKS_SHOW_AVAILABLE, "Client connected!");
  xmpp_client_send (client, node);
  iks_delete (node);

  /* Sleeping some time before disconnection */
  sleep (3);
  xmpp_client_disconnect (client);
  return IKS_FILTER_EAT;
}

int
not_connected_cb (void *data, ikspak *pak)
{
  xmpp_client_t *client;
  char *node_str = iks_string (iks_stack (pak->x), pak->x);
  client = (xmpp_client_t *) data;
  printf ("not connected: =(\n");
  printf ("%s\n", node_str);
  xmpp_client_disconnect (client);
  return IKS_FILTER_EAT;
}

int
main (int argc, char **argv)
{
  xmpp_client_t *xmpp;
  log_t *logger;
  const char *jid, *passwd, *host;

  /* Getting connection parameters from the command line */
  if (argc < 3)
    {
      fprintf (stderr, "Usage: %s: <jid> <passwd> <host>\n", argv[0]);
      return 1;
    }

  jid = argv[1];
  passwd = argv[2];
  host = argv[3];

  /* To initialize a client, you can pass the jid, password, host and
   * port number. Host can be NULL, in this case the domain part of
   * jid will be used. And if port is 0, it will default to 5222 */
  xmpp = xmpp_client_new (jid, passwd, host, 5222);

  /* You can access and configure log object attached to the brand new
   * xmpp client instance. To know more about the log object, look at
   * the `log-system.c' example */
  logger = xmpp_client_get_logger (xmpp);
  log_set_level (logger, log_get_level (logger) | LOG_INFO | LOG_DEBUG);
  log_set_use_colors (logger, 1);

  /* Connecting to the host */
  if (!xmpp_client_connect (xmpp))
    {
      error_t *error;
      error = xmpp_client_get_error (xmpp);
      fprintf (stderr, "%s: %s\n", error_get_name (error),
               error_get_message (error));
      xmpp_client_free (xmpp);
      return 1;
    }

  /* Setting callbacks to handle successful and failure
   * authentications */
  xmpp_client_set_auth_success_cb (xmpp, connected_cb, ((void *) xmpp));
  xmpp_client_set_auth_failure_cb (xmpp, not_connected_cb, ((void *) xmpp));

  /* Starting client main loop. Requesting to do not detach the loop
   * to another thread. Authentication will be done here too. */
  if (!xmpp_client_run (xmpp, 0))
    {
      error_t *error;
      error = xmpp_client_get_error (xmpp);
      fprintf (stderr, "%s: %s\n", error_get_name (error),
               error_get_message (error));
      xmpp_client_free (xmpp);
      return 1;
    }

  /* Cleaning up things */
  xmpp_client_free (xmpp);
  return 0;
}
