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
} srv_recorde_t;

int
ta_srv_init (void)
{
  return res_init ();
}

static void
ta_srv_target_free (ta_srv_target_t *target)
{
  if (target->name)
    free (target->name);
  if (target->domain)
    free (target->domain);
  if (target->host)
    free (target->host);
}

void
ta_srv_target_init (ta_srv_target_t *target, const char *name,
                    const char *domain)
{
  ta_object_init (TA_CAST_OBJECT (target),
                  (ta_free_func_t) ta_srv_target_free);
  target->name = strdup (name);
  target->domain = strdup (domain);
}

ta_srv_target_t *
ta_srv_target_new (const char *name, const char *domain)
{
  ta_srv_target_t *target;
  target = malloc (sizeof (ta_srv_target_t));
  ta_srv_target_init (target, name, domain);
  return target;
}

const char *
ta_srv_target_get_host (ta_srv_target_t *target)
{
  return target->host;
}

int
ta_srv_target_get_port (ta_srv_target_t *target)
{
  return target->port;
}

int
ta_srv_target_query_domain (ta_srv_target_t *target)
{
  ta_list_t *targets = NULL;
  HEADER *message;
  u_char answer[1024];
  u_char *p, *end;
  int len, count;

  /* Values to be read for each target */
  char buf[1024];
  u_int16_t type, rdlength;

  len = res_querydomain (target->name, target->domain, C_IN, T_SRV, answer,
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
      srv_recorde_t record;

      p += dn_expand (answer, end, p, buf, sizeof (buf));
      GETSHORT (type, p);
      GETSHORT (record.class, p);
      GETLONG (record.ttl, p);
      GETSHORT (rdlength, p);

      /* We're not interested in non IN SRV records */
      if (type != T_SRV || record.class != C_IN)
        {
          /* skipping to the next target */
          p += rdlength;
          continue;
        }

      GETSHORT (record.priority, p);
      GETSHORT (record.weight, p);
      GETSHORT (record.port, p);
      p += dn_expand (answer, end, p, buf, sizeof (buf));
      record.target = strdup (buf);

      printf ("%s:%d\n", buf, record.port);

      targets = ta_list_append (targets, &record);
    }

  return 0;
}
