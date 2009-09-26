DEFSDIR = $(top_srcdir)/bindings/defs
DEFSFILE = $(DEFSDIR)/$(PACKAGE).json

pyexecdir = $(pythondir)/taningia
pyexec_LTLIBRARIES = atom.la error.la iri.la log.la pubsub.la xmpp.la
pyexec_SCRIPTS = __init__.py

DEPS = listconverter.c

atom_la_SOURCES = \
	atommodule.c \
	atommodule.h \
	$(DEPS)

atom_la_CFLAGS = \
	-DPyAtom_BUILD_CORE \
	-DPyLog_BUILD_CORE \
	-DPyPubsub_BUILD_CORE \
	-DPyXmpp_BUILD_CORE

error_la_SOURCES = \
	errormodule.c \
	errormodule.h \
	$(DEPS)

error_la_CFLAGS = \
	-DPyAtom_BUILD_CORE \
	-DPyError_BUILD_CORE \
	-DPyIri_BUILD_CORE \
	-DPyLog_BUILD_CORE \
	-DPyPubsub_BUILD_CORE \
	-DPyXmpp_BUILD_CORE

iri_la_SOURCES = \
	irimodule.c \
	irimodule.h \
	$(DEPS)

iri_la_CFLAGS = \
	-DPyAtom_BUILD_CORE \
	-DPyIri_BUILD_CORE \
	-DPyLog_BUILD_CORE \
	-DPyPubsub_BUILD_CORE \
	-DPyXmpp_BUILD_CORE

log_la_SOURCES = \
	logmodule.c \
	logmodule.h \
	$(DEPS)

log_la_CFLAGS = \
	-DPyAtom_BUILD_CORE \
	-DPyIri_BUILD_CORE \
	-DPyLog_BUILD_CORE \
	-DPyPubsub_BUILD_CORE \
	-DPyXmpp_BUILD_CORE

pubsub_la_SOURCES = \
	pubsubmodule.c \
	pubsubmodule.h \
	$(DEPS)

pubsub_la_CFLAGS = \
	-DPyAtom_BUILD_CORE \
	-DPyIri_BUILD_CORE \
	-DPyPubsub_BUILD_CORE \
	-DPyXmpp_BUILD_CORE

xmpp_la_SOURCES = \
	xmppmodule.c \
	xmppmodule.h \
	$(DEPS)

xmpp_la_CFLAGS = \
	-DPyAtom_BUILD_CORE \
	-DPyIri_BUILD_CORE \
	-DPyPubsub_BUILD_CORE \
	-DPyXmpp_BUILD_CORE

%module.c:
	PYTHONPATH=$(DEFSDIR) $(PYTHON) writer.py $(DEFSFILE)

AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(IKSEMEL_CFLAGS) \
	$(PYTHON_CFLAGS) \
	$(PYIKSEMEL_CFLAGS) \
	-I$(top_srcdir)/include

AM_LDFLAGS = \
	-module -avoid-version \
	$(GLIB_LIBS) \
	$(IKSEMEL_LIBS) \
	$(PYTHON_LIBS) \
	$(PYIKSEMEL_LIBS) \
	$(top_builddir)/src/libtaningia.la

EXTRA_DIST = writer.py overrides.py \
	$(atom_la_SOURCES) \
	$(error_la_SOURCES) \
	$(iri_la_SOURCES) \
	$(log_la_SOURCES) \
	$(pubsub_la_SOURCES) \
	$(xmpp_la_SOURCES)

CLEANFILES = *module.[ch]
