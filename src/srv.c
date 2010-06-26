/*
 * Copyright (C) 2010  Lincoln de Sousa <lincoln@comum.org>
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

#include <stdlib.h>
#include <string.h>
#include <resolv.h>
#include <taningia/list.h>
#include <taningia/srv.h>

typedef struct
{
  u_int32_t ttl;
  u_int16_t class;
  u_int16_t priority;
  u_int16_t weight;
  u_int16_t port;
  char *target;
} target_t;

int
ta_srv_init (void)
{
  return res_init ();
}

static void
ta_srv_resolver_free (ta_srv_resolver_t *resolver)
{
  if (resolver->name)
    free (resolver->name);
  if (resolver->domain)
    free (resolver->domain);
  if (resolver->host)
    free (resolver->host);
}

void
ta_srv_resolver_init (ta_srv_resolver_t *resolver, const char *name,
                      const char *domain)
{
  ta_object_init (TA_CAST_OBJECT (resolver),
                  (ta_free_func_t) ta_srv_resolver_free);
  resolver->name = strdup (name);
  resolver->domain = strdup (domain);
}

ta_srv_resolver_t *
ta_srv_resolver_new (const char *name, const char *domain)
{
  ta_srv_resolver_t *resolver;
  resolver = malloc (sizeof (ta_srv_resolver_t));
  ta_srv_resolver_init (resolver, name, domain);
  return resolver;
}

ta_list_t *
ta_srv_resolver_query_domain (ta_srv_resolver_t *resolver)
{
  HEADER *message;
  u_char answer[1024];
  u_char *p, *end;
  int len, count;
  ta_list_t
    *targets = NULL,
    *result = NULL,
    *node = NULL;
  target_t *t;

  /* Values to be read for each target */
  char buf[1024];
  u_int16_t type, rdlength;

  len = res_querydomain (resolver->name, resolver->domain, C_IN, T_SRV, answer,
                         sizeof (answer));
  if (len <= 0)
    return -1;

  /* HEADER type defined in `arpa/nameser_compat.h'. See 4.1.1. Header
   * section format in RFC 1035. */
  message = (HEADER *) answer;

  /* Finding bounds of received answer */
  p = answer + sizeof (HEADER);
  end = answer + len;

  /* We don't need to handle query section */
  count = ntohs (message->qdcount);
  while (count-- && p < end)
    {
      p += dn_expand (answer, end, p, buf, sizeof (buf));
      p += 4;
    }

  /* Reading the answer section that is what we really want */
  count = ntohs (message->ancount);
  while (count-- && p < end)
    {
      target_t t;

      p += dn_expand (answer, end, p, buf, sizeof (buf));
      GETSHORT (type, p);
      GETSHORT (t.class, p);
      GETLONG (t.ttl, p);
      GETSHORT (rdlength, p);

      /* We're not interested in non IN SRV records */
      if (type != T_SRV || t.class != C_IN)
        {
          /* skipping to the next target */
          p += rdlength;
          continue;
        }

      GETSHORT (t.priority, p);
      GETSHORT (t.weight, p);
      GETSHORT (t.port, p);
      p += dn_expand (answer, end, p, buf, sizeof (buf));
      t.target = strdup (buf);

      printf ("%s:%d\n", buf, t.port);

      targets = ta_list_append (targets, &t);
    }

  return 0;
}
