noinst_PROGRAMS = xmpp-client xmpp-client-2 log iri atom list srv

xmpp_client_SOURCES = xmpp-client.c
xmpp_client_CFLAGS = -I$(top_srcdir)/include $(IKSEMEL_CFLAGS)
xmpp_client_LDFLAGS = $(top_builddir)/src/libtaningia.la $(IKSEMEL_LIBS)

xmpp_client_2_SOURCES = xmpp-client-2.c
xmpp_client_2_CFLAGS = -I$(top_srcdir)/include $(IKSEMEL_CFLAGS)
xmpp_client_2_LDFLAGS = $(top_builddir)/src/libtaningia.la $(IKSEMEL_LIBS)

log_SOURCES = log.c
log_CFLAGS = -I$(top_srcdir)/include
log_LDFLAGS = $(top_builddir)/src/libtaningia.la

iri_SOURCES = iri.c
iri_CFLAGS = -I$(top_srcdir)/include
iri_LDFLAGS = $(top_builddir)/src/libtaningia.la

atom_SOURCES = atom.c
atom_CFLAGS = -I$(top_srcdir)/include $(IKSEMEL_CFLAGS)
atom_LDFLAGS = $(top_builddir)/src/libtaningia.la $(IKSEMEL_LIBS)

list_SOURCES = list.c
list_CFLAGS = -I$(top_srcdir)/include
list_LDFLAGS = $(top_builddir)/src/libtaningia.la

srv_SOURCES = srv.c
srv_CFLAGS = -I$(top_srcdir)/include
srv_LDFLAGS = $(top_builddir)/src/libtaningia.la
