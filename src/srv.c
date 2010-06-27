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

/* Forward declarations */

ta_srv_target_t *ta_srv_target_new (void);
void ta_srv_target_init (ta_srv_target_t *target);

/* ta_srv_* functions and their static dependencies */

int
ta_srv_init (void)
{
  return res_init ();
}

static int
cmp_targets (ta_list_t *a, ta_list_t *b)
{
  ta_srv_target_t *ta, *tb;
  ta = (ta_srv_target_t *) a->data;
  tb = (ta_srv_target_t *) b->data;

  if (ta->priority == tb->priority)
    return ta->weight - tb->weight;
  else
    return ta->priority - tb->priority;
}

ta_list_t *
ta_srv_query_domain (const char *name, const char *domain)
{
  HEADER *message;
  u_char answer[1024];
  u_char *p, *end;
  int len, count;
  ta_list_t
    *targets = NULL,
    *result = NULL,
    *node = NULL;

  /* Values to be read for each target */
  char buf[1024];
  u_int16_t type, rdlength;

  len = res_querydomain (name, domain, C_IN, T_SRV, answer, sizeof (answer));
  if (len <= 0)
    return NULL;

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
      ta_srv_target_t *t = ta_srv_target_new ();

      p += dn_expand (answer, end, p, buf, sizeof (buf));
      GETSHORT (type, p);
      GETSHORT (t->_class, p);
      GETLONG (t->ttl, p);
      GETSHORT (rdlength, p);

      /* We're not interested in non IN SRV records */
      if (type != T_SRV || t->_class != C_IN)
        {
          /* skipping to the next target */
          p += rdlength;
          continue;
        }

      GETSHORT (t->priority, p);
      GETSHORT (t->weight, p);
      GETSHORT (t->port, p);
      p += dn_expand (answer, end, p, buf, sizeof (buf));
      t->host = strdup (buf);
      targets = ta_list_append (targets, t);
    }

  /* Here we start to handle what the section "The format of the SRV
   * RR" of the RFC 2782 says about the ordering of targets. Based on
   * the priority and weight fields.
   *
   * In the first step, we just sort targets list by its priority
   * putting targets with the same priority in the right order (lowers
   * first) except the ones that has weight equals to 0. That are
   * placed in the first positions. */
  targets = ta_list_sort (targets, cmp_targets);

  /* Now we'll group all targets with same priority respecting their
   * weight attribute. */

  return targets;
}

/* ta_srv_target_* stuff */

static void
ta_srv_target_free (ta_srv_target_t *target)
{
  if (target->host)
    {
      free (target->host);
      target->host = NULL;
    }
}

void
ta_srv_target_init (ta_srv_target_t *target)
{
  ta_object_init (TA_CAST_OBJECT (target),
                  (ta_free_func_t) ta_srv_target_free);
  target->ttl = 0;
  target->_class = 0;
  target->priority = 0;
  target->weight = 0;
  target->port = 0;
  target->host = NULL;
}

ta_srv_target_t *
ta_srv_target_new (void)
{
  ta_srv_target_t *target = malloc (sizeof (ta_srv_target_t));
  ta_srv_target_init (target);
  return target;
}

u_int16_t
ta_srv_target_get_port (ta_srv_target_t *target)
{
  return target->port;
}

u_int16_t
ta_srv_target_get_weight (ta_srv_target_t *target)
{
  return target->weight;
}

u_int16_t
ta_srv_target_get_class (ta_srv_target_t *target)
{
  return target->_class;
}

u_int16_t
ta_srv_target_get_priority (ta_srv_target_t *target)
{
  return target->priority;
}

u_int32_t
ta_srv_target_get_ttl (ta_srv_target_t *target)
{
  return target->ttl;
}

const char *
ta_srv_target_get_host (ta_srv_target_t *target)
{
  return target->host;
}
