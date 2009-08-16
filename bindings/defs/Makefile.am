INCDIR = $(top_srcdir)/include/$(PACKAGE)

all:
	$(PYTHON) scanner.py $(PACKAGE) $(INCDIR)/atom.h	\
		 $(INCDIR)/filter.h $(INCDIR)/pubsub.h		\
		 $(INCDIR)/xmpp.h

EXTRA_DIST = $(PACKAGE).json
