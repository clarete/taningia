INCDIR = $(top_srcdir)/include/$(PACKAGE)

all:
	$(PYTHON) scanner.py $(PACKAGE) $(INCDIR)/atom.h		\
		 $(INCDIR)/error.h $(INCDIR)/filter.h			\
		 $(INCDIR)/pubsub.h $(INCDIR)/xmpp.h $(INCDIR)/log.h	\
		 $(INCDIR)/iri.h

EXTRA_DIST = $(PACKAGE).json

CLEANFILES = $(PACKAGE).json
