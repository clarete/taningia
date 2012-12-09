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

/* This example is quite closer to xmpp-client. The only difference is
 * that we do not provide any explanation in `main()' function. The
 * only stuff we test here is the `ta_xmpp_client_send_and_filter()'
 * function. So, pay special attention to the `auth_cb()' function.
 */

#include <stdio.h>
#include <iksemel.h>
#include <taningia/xmpp.h>
#include <taningia/log.h>

/* This is the user defined callback that will be fired when our
 * message being sent in `auth_cb()' function is answered by the
 * server. */
static void
stanza_answered (ta_xmpp_client_t *client, iks *node, void *TA_UNUSED(data))
{
  printf ("Answer received from the server: %s\n",
          iks_string (iks_stack (node), node));
  printf ("Disconnecting\n");
  ta_xmpp_client_disconnect (client);
}

/* This function registers the `stanza_answered' callback when sending
 * the presence message. */
static int
auth_cb (ta_xmpp_client_t *client, void *TA_UNUSED(data))
{
  iks *node;
  /* Sending presence info */
  node = iks_make_pres (IKS_SHOW_AVAILABLE, "Client connected!");

  /* You cannot use `_send_and_filter()' with a NULL id. */
  iks_insert_attrib (node, "id", "mymsg");
  ta_xmpp_client_send_and_filter (client, node, stanza_answered, NULL, NULL);
  iks_delete (node);

  return 0;
}

/* Just to say the user that its authentication has failed. */
static int
auth_failed_cb (ta_xmpp_client_t *client, void *data)
{
  ikspak *pak = (ikspak *) data;
  char *node_str = iks_string (iks_stack (pak->x), pak->x);
  printf ("not authenticated: =(\n");
  printf ("%s\n", node_str);
  ta_xmpp_client_disconnect (client);
  return 0;
}

int
main (int argc, char **argv)
{
  ta_xmpp_client_t *xmpp;
  ta_log_t *logger;
  const char *jid, *passwd, *host = NULL;

  if (argc < 3)
    {
      fprintf (stderr, "Usage: %s: <jid> <passwd> [<host>]\n", argv[0]);
      return 1;
    }

  jid = argv[1];
  passwd = argv[2];

  if (argc == 4)
    host = argv[3];

  xmpp = ta_xmpp_client_new (jid, passwd, host, 5222);
  ta_xmpp_client_event_connect (xmpp, "authenticated",
                                (ta_xmpp_client_hook_t) auth_cb,
                                NULL);
  ta_xmpp_client_event_connect (xmpp, "authentication-failed",
                                (ta_xmpp_client_hook_t) auth_failed_cb,
                                NULL);
  logger = ta_xmpp_client_get_logger (xmpp);
  ta_log_set_level (logger, TA_LOG_DEBUG);
  ta_log_set_use_colors (logger, 1);
  if (ta_xmpp_client_connect (xmpp) != TA_OK)
    {
      const ta_error_t *error = ta_error_last ();
      fprintf (stderr, "(%d) %s\n", error->code, error->message);
      ta_object_unref (xmpp);
      return 1;
    }
  if (ta_xmpp_client_run (xmpp, 0) != TA_OK)
    {
      const ta_error_t *error = ta_error_last ();
      fprintf (stderr, "(%d) %s\n", error->code, error->message);
      ta_object_unref (xmpp);
      return 1;
    }
  ta_object_unref (xmpp);
  return 0;
}
