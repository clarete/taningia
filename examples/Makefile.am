noinst_PROGRAMS = xmpp-client log iri atom list

xmpp_client_SOURCES = xmpp-client.c
xmpp_client_CFLAGS = -I$(top_srcdir)/include $(IKSEMEL_CFLAGS)
xmpp_client_LDFLAGS = $(top_builddir)/src/libtaningia.la

log_SOURCES = log.c
log_CFLAGS = -I$(top_srcdir)/include
log_LDFLAGS = $(top_builddir)/src/libtaningia.la

iri_SOURCES = iri.c
iri_CFLAGS = -I$(top_srcdir)/include
iri_LDFLAGS = $(top_builddir)/src/libtaningia.la

atom_SOURCES = atom.c
atom_CFLAGS = -I$(top_srcdir)/include $(IKSEMEL_CFLAGS)
atom_LDFLAGS = $(top_builddir)/src/libtaningia.la

list_SOURCES = list.c
list_CFLAGS = -I$(top_srcdir)/include
list_LDFLAGS = $(top_builddir)/src/libtaningia.la
