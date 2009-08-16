CC = gcc
CFLAGS = $(shell pkg-config --cflags jarvis glib-2.0 iksemel) $(shell python-config --cflags)
CFLAGS += -I/home/lincoln/Work/python-iksemel
CFLAGS += -fPIC -Wall -O0 -g
LIBS = $(shell pkg-config --libs  jarvis glib-2.0 iksemel) $(shell python-config --libs)
LIBS +=
OBJECTS = jarvismodule.o

all: $(OBJECTS)
	$(CC) -shared -o jarvis.so $(LIBS) $(OBJECTS)

gen:
	python writer.py jarvis ../../include/jarvis/atom.h	\
		../../include/jarvis/pubsub.h			\
		../../include/jarvis/filter.h			\
		../../include/jarvis/xmpp.h

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *~ *.pyc jarvis.so jarvismodule.[ch] $(OBJECTS)
