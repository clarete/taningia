/*
 * Copyright (C) 2009  Lincoln de Sousa <lincoln@minaslivre.org>
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

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <taningia/object.h>
#include <taningia/iri.h>
#include <taningia/error.h>

static void
ta_iri_free (ta_iri_t *iri)
{
  if (iri->scheme)
    free (iri->scheme);
  if (iri->user)
    free (iri->user);
  if (iri->host)
    free (iri->host);
  if (iri->path)
    free (iri->path);
  if (iri->query)
    free (iri->query);
  if (iri->fragment)
    free (iri->fragment);
}

void
ta_iri_init (ta_iri_t *iri)
{
  ta_object_init (TA_CAST_OBJECT (iri), (ta_free_func_t) ta_iri_free);
  iri->scheme = NULL;
  iri->user = NULL;
  iri->host = NULL;
  iri->port = 0;
  iri->path = NULL;
  iri->query = NULL;
  iri->fragment = NULL;
}

ta_iri_t *
ta_iri_new (void)
{
  ta_iri_t *iri;
  iri = malloc (sizeof (ta_iri_t));
  ta_iri_init (iri);
  return iri;
}

const char *
ta_iri_get_scheme (ta_iri_t *iri)
{
  return iri->scheme;
}

void
ta_iri_set_scheme (ta_iri_t *iri, const char *scheme)
{
  if (iri->scheme)
    free (iri->scheme);
  iri->scheme = strdup (scheme);
}

const char *
ta_iri_get_user (ta_iri_t *iri)
{
  return iri->user;
}

void
ta_iri_set_user (ta_iri_t *iri, const char *user)
{
  if (iri->user)
    free (iri->user);
  iri->user = strdup (user);
}

const char *
ta_iri_get_host (ta_iri_t *iri)
{
  return iri->host;
}

void
ta_iri_set_host (ta_iri_t *iri, const char *host)
{
  if (iri->host)
    free (iri->host);
  iri->host = strdup (host);
}

int
ta_iri_get_port (ta_iri_t *iri)
{
  return iri->port;
}

void
ta_iri_set_port (ta_iri_t *iri, int port)
{
  iri->port = port;
}

const char *
ta_iri_get_path (ta_iri_t *iri)
{
  return iri->path;
}

void
ta_iri_set_path (ta_iri_t *iri, const char *path)
{
  if (iri->path)
    free (iri->path);
  iri->path = strdup (path);
}

const char *
ta_iri_get_query (ta_iri_t *iri)
{
  return iri->query;
}

void
ta_iri_set_query (ta_iri_t *iri, const char *query)
{
  if (iri->query)
    free (iri->query);
  iri->query = strdup (query);
}

const char *
ta_iri_get_fragment (ta_iri_t *iri)
{
  return iri->fragment;
}

void
ta_iri_set_fragment (ta_iri_t *iri, const char *fragment)
{
  if (iri->fragment)
    free (iri->fragment);
  iri->fragment = strdup (fragment);
}

char *
ta_iri_to_string (ta_iri_t *iri)
{
  size_t static_part;
  size_t scheme_size,
    host_size,
    user_size = 0,
    path_size = 0,
    query_size = 0,
    fragment_size = 0;
  size_t rlen;
  char *ret;
  int position = 0;

  /* This is actually required in the RFC to continue, but meh, our
   * modern world requires us to allow people to create iris with no
   * scheme */
  scheme_size = iri->scheme ? strlen (iri->scheme) : 0;
  host_size = iri->host ? strlen (iri->host) : 0;

  /* Calculating the size of our return var */
  static_part = 3 + 4;          /* ://, portnumber */
  if (iri->user)
    {
      user_size = strlen (iri->user);
      static_part++;            /* @ */
    }
  if (iri->port)
    static_part++;              /* : */
  if (iri->path)
    path_size = strlen (iri->path);
  if (iri->query)
    {
      query_size = strlen (iri->query);
      static_part++;            /* ? */
    }
  if (iri->fragment)
    {
      fragment_size = strlen (iri->fragment);
      static_part++;            /* # */
    }

  rlen = scheme_size + user_size +
    host_size +  path_size +
    query_size + fragment_size +
    static_part + 1;
  ret = malloc (rlen);

  /* Copying scheme to the ret var. */
  memcpy (ret, iri->scheme, scheme_size);
  position += iri->scheme ? scheme_size : 0;

  /* So, if we have a host set in the uri, we add the double
   * slashes!*/
  if (iri->host && iri->scheme)
    {
      memcpy ((ret+position), "://", 3);
      position += 3;
    }

  if (!iri->host)
    {
      memcpy ((ret+position), ":", 1);
      position++;
    }

  /* User handling */
  if (iri->user)
    {
      memcpy ((ret+position), iri->user, user_size);
      position += user_size;
      memcpy ((ret+position), "@", 1);
      position++;
    }

  /* Host */
  if (iri->host)
    {
      memcpy ((ret+position), iri->host, host_size);
      position += host_size;
    }

  /* Port number */
  if (iri->port)
    {
      char port[4];
      size_t port_size;
      memcpy ((ret+position), ":", 1);
      position++;
      sprintf (port, "%d", iri->port);
      port_size = strlen (port);
      memcpy ((ret+position), port, port_size);
      position += port_size;
    }
  if (iri->path)
    {
      memcpy ((ret+position), iri->path, path_size);
      position += path_size;
    }
  if (iri->query)
    {
      memcpy ((ret+position), "?", 1);
      position++;
      memcpy ((ret+position), iri->query, query_size);
      position += query_size;
    }
  if (iri->fragment)
    {
      memcpy ((ret+position), "#", 1);
      position++;
      memcpy ((ret+position), iri->fragment, fragment_size);
      position += fragment_size;
    }

  ret[position] = '\0';
  return ret;
}

int
ta_iri_set_from_string (ta_iri_t *iri, const char *string)

{
  const char *p, *ihier_part, *query, *fragment;
  char c;
  size_t size;

  /* This is what we need to parse here:
   *
   * IRI = scheme ":" ihier-part [ "?" iquery ] [ "#" ifragment ]
   *
   */

  p = string;

  if (!isalpha(p[0]))
    {
      ta_error_set (TA_IRI_PARSING_ERROR,
                    "Schema should start with an alpha char");
      return TA_ERROR;
    }

  /* Getting "scheme" part */
  while (1)
    {
      c = *p;
      if (c == ':')
        break;
      /* As said in the IRI rfc, the scheme part only accept
       * a-Z0-9+.- chars*/
      if (!isalnum (c) &&
          !(c == '-' || c == '+' || c == '.'))
        {
          ta_error_set (TA_IRI_PARSING_ERROR,
                        "Schema should only have the following chars: "
                        "[a-Z][0-9][-+.]");
          return TA_ERROR;
        }
      c = *p++;
    }
  size = p - string;
  iri->scheme = malloc (size+1);
  memcpy (iri->scheme, string, size+1);
  iri->scheme[size] = '\0';

  /* Delimiter to the end of the ihier part */
  query = strchr (p, '?');
  if (query)
    {
      /* Removing the `?' char */
      query++;
      fragment = strchr (query, '#');
      if (fragment)
        {
          /* Removing number sign (#) */
          fragment++;
          iri->query = strndup (query, fragment-1 - query);
          iri->fragment = strdup (fragment);
        }
      else
        iri->query = strdup (query);
    }
  else
    {
      fragment = strchr (p, '#');
      if (fragment)
        iri->fragment = strdup (fragment+1);
    }

  /* Marks the end of the ihier_part */
  ihier_part = p+1;

  /* Parsing the authority section
   *
   * [ iuserinfo "@" ] ihost [ ":" port ]
   *
   */

  if (ihier_part[0] == '/' &&
      ihier_part[1] == '/')
    {
      const char *userinfo, *port, *path, *host_end;
      const char *port_start;
      char *port_str;
      char port_iter;
      size_t port_len = 0;

      /* Removing slashes */
      ihier_part += 2;

      /* iuserinfo */
      userinfo = strrchr (ihier_part, '@');
      if (userinfo)
        {
          iri->user = strndup (ihier_part, userinfo - ihier_part);
          port_start = userinfo+1;
        }
      else
        port_start = ihier_part;

      /* Port */
      port = NULL;
      while ((port_iter = *port_start++))
        {
          /* We just found the port, time to get out */
          if (port_iter == ':')
            {
              port = port_start - 1;
              break;
            }

          /* we Couldn't find the port and we reached either the query
           * or the fragment */
          if (port_iter == '?' || port_iter == '#')
            break;
        }

      /* port = strchr (port_start, ':'); */
      if (port)
        {
          const char *pp;
          char ch;

          pp = port+1;

          /* Only digits are going to be added to the port
           * attribute. This while breaks in the first non digit char
           * and it will be the first char of `iri->path' */
          while ((ch = *pp++))
            {
              if (!isdigit (ch))
                break;
              port_len++;
            }

          /* We're inside an if that make it sure that the user tried
           * to pass us a port to parse. If no digit is found, there
           * is something wrong, so we can abort parsing the IRI. */
          if (port_len == 0)
            {
              ta_error_set (TA_IRI_PARSING_ERROR, "Invalid port number");
              return TA_ERROR;
            }

          port_str = strndup (port+1, port_len);
          port_str[port_len] = '\0';
          iri->port = atoi (port_str);
          free (port_str);
        }

      /* Path */

      path = strchr (ihier_part, '/');

      /* If authority section exists, if it exists, path should start
       * with "/" as said in the following rule:
       *
       *   ipath-abempty = *( "/" isegment )
       *
       */
      if (path && path[0] != '/')
        {
          ta_error_set (TA_IRI_PARSING_ERROR,
                        "Path should start with a '/' since authority "
                        "section was filled");
          return TA_ERROR;
        }
      if (path)
        {
          const char *pathp;
          char ch;
          int path_len;

          pathp = path;
          path_len = 0;
          while ((ch = *pathp++))
            {
              if (ch == '?' || ch == '#')
                break;
              path_len++;
            }
          iri->path = strndup (path, path_len);
        }

      /* Host */

      /* If a user is found, start host from it, otherwise, start from
       * the ihier_part. The host end will be determined by either the
       * port, the path, the query, or the fragment start. */
      host_end =
        (port) ? port :
        (query) ? query - 1 :
        (fragment) ? fragment :
        path;

      iri->host = strndup ((userinfo ? userinfo + 1 : ihier_part),
                           host_end -
                           (userinfo ? userinfo : ihier_part - 1) -
                           1);
    }
  else
    {
      if (query)
        {
          int end = query - 1 - ihier_part;
          iri->path = strndup (ihier_part, end);
        }
      else if (!query && fragment)
        iri->path = strndup (ihier_part, fragment - ihier_part);
      else
        iri->path = strdup (ihier_part);
    }

  return TA_OK;
}

static void
ta_tag_free (ta_tag_t *tag)
{
  if (tag->authority)
    free (tag->authority);
  if (tag->date)
    free (tag->date);
  if (tag->specific)
    free (tag->specific);
}

void
ta_tag_init (ta_tag_t *tag)
{
  ta_object_init (TA_CAST_OBJECT (tag), (ta_free_func_t) ta_tag_free);
  ta_iri_init (TA_CAST_IRI (tag));
  tag->authority = NULL;
  tag->date = NULL;
  tag->specific = NULL;
}

ta_tag_t *
ta_tag_new (void)
{
  ta_tag_t *tag;
  tag = malloc (sizeof (ta_tag_t));
  ta_tag_init (tag);
  return tag;
}

/* This helper function will be called at the end of all setters that
 * change the iri path in some way to update it in the base class and
 * make it possible to use ta_iri_to_string to our tag type.
 */
static void
_ta_tag_update_path (ta_tag_t *tag)
{
  ta_iri_t *parent = TA_CAST_IRI (tag);
  if (parent->path)
    free (parent->path);
  parent->path = malloc (strlen (tag->authority) +
                         strlen (tag->date) +
                         strlen (tag->specific) +
                         2);
  sprintf (parent->path, "%s,%s:%s",
           tag->authority, tag->date, tag->specific);
}

const char *
ta_tag_get_authority (ta_tag_t *tag)
{
  return tag->authority;
}

void
ta_tag_set_authority (ta_tag_t *tag, const char *authority)
{
  if (tag->authority)
    free (tag->authority);
  tag->authority = strdup (authority);
  _ta_tag_update_path (tag);
}

const char *
ta_tag_get_date (ta_tag_t *tag)
{
  return tag->date;
}

void
ta_tag_set_date (ta_tag_t *tag, const char *date)
{
  if (tag->date)
    free (tag->date);
  tag->date = strdup (date);
  _ta_tag_update_path (tag);
}

const char *
ta_tag_get_specific (ta_tag_t *tag)
{
  return tag->specific;
}

void
ta_tag_set_specific (ta_tag_t *tag, const char *specific)
{
  if (tag->specific)
    free (tag->specific);
  tag->specific = strdup (specific);
  _ta_tag_update_path (tag);
}

int
ta_tag_set_from_string (ta_tag_t *tag, const char *tagstr)
{
  /* The rest of our function will use parameters set by the next
   * line. If something wrong happens, user will need to handle this
   * error like any other error caused by the tag parsing*/
  if (!ta_iri_set_from_string (TA_CAST_IRI (tag), tagstr))
    return 0;
  else
    {
      const char *path, *date, *specific;
      path = ta_iri_get_path (TA_CAST_IRI (tag));

      /* Looking for the first separator.
       *
       *   taggingEntity = authorityName "," date
       *
       */
      date = strchr (path, ',');
      if (!date)
        {
          ta_error_set (TA_TAG_PARSING_ERROR, "Date field missing in tag");
          return 0;
        }
      tag->authority = strndup (path, date - path);

      /* Looking for the specific part */
      specific = strchr (date, ':');
      if (!specific)
        {
          ta_error_set (TA_TAG_PARSING_ERROR, "Domain specific not provided");
          return 0;
        }

      /* Storing already found date info. TODO: Validate date */
      tag->date = strndup (date+1, specific - date - 1);

      /* Altough specification says that `query' is part of specific,
       * it was already parsed `iri_set_from_string', so let's go to
       * the end of the string */
      tag->specific = strdup (specific+1);
      return 1;
    }
}
