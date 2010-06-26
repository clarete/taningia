/*
 * Copyright (C) 2010 Lincoln de Sousa <lincoln@minaslivre.org>
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
#include <stdlib.h>
#include <taningia/srv.h>

int
main (int argc, char **argv)
{
  ta_srv_target_t *target;
  if (argc < 3)
    {
      fprintf (stderr, "Name and/or domain missing\n");
      exit (EXIT_FAILURE);
    }

  /* Initializing libresolv stuff. Like reading resolv.conf */
  ta_srv_init ();

  /* Quering something like _xmpp-client._tcp.jabber-br.org or
   * _http._tcp.comum.org is supported. The _http._tcp and
   * _xmpp-client._tcp can be passed in the first parameter and
   * jabber-br.org and comum.org in the second one. */
  target = ta_srv_target_new (argv[1], argv[2]);

  /* To know if the name was already resolved or not, you can test if
   * ta_srv_target_get_host() is null. */
  if (ta_srv_target_query_domain (target) == 0)
    {
      printf ("Target found:\n");
      printf (" Host: %s\n", ta_srv_target_get_host (target));
      printf (" Port: %d\n", ta_srv_target_get_port (target));
    }
  ta_object_unref (target);
  return 0;
}
