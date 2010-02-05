lib_LTLIBRARIES = libtaningia.la
libtaningia_la_SOURCES = error.c xmpp.c pubsub.c iri.c atom.c log.c	\
	list.c hashtable.c hashtable.h hashtable-utils.c		\
	hashtable-utils.h
libtaningia_la_LDFLAGS = -version-info 0:1 -no-undefined
libtaningia_la_CFLAGS = $(IKSEMEL_CFLAGS) -I$(top_srcdir)/include
libtaningia_la_LIBADD = $(IKSEMEL_LIBS) -lpthread
