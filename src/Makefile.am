lib_LTLIBRARIES = libtaningia.la
libtaningia_la_SOURCES = log.c object.c global.c error.c buf.c xmpp.c	\
	pubsub.c iri.c atom.c list.c hashtable.c hashtable.h		\
	hashtable-utils.c hashtable-utils.h

libtaningia_la_LDFLAGS = -version-info 0:2 -no-undefined
libtaningia_la_CFLAGS = $(WARNING_FLAGS) $(PTHREAD_CFLAGS) $(IKSEMEL_CFLAGS) -I$(top_srcdir)/include
libtaningia_la_LIBADD = $(PTHREAD_LIBS) $(IKSEMEL_LIBS)

libtaningia_la_SOURCES += srv.c
libtaningia_la_LIBADD += -lresolv
