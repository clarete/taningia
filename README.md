# Taningia README file

## Intro

Taningia aims to be a toolkit for writting programs that focus in a
certain type of comunication: The _near_ realtime one. This library has
the XMPP protocol as its base for things we call _comunication_ and the
Atom standard (XML) for describing content.

## How it works

One of the strongest arguments that made me start to write it is that
software should comunicate with other software and with people. Another
idea is thate people and software should have theier presence in a
network.

## Instalation

We are using autotools as our building system, so to compile and install
taningia, you only need to run::

```bash
$ ./configure && make && sudo make install
```

If you need more details, please read the INSTALL file.

## Dependencies

 * libiksemel (1.2, 1.2+ seems to be broken)

## Language support

All code is written in C. There are two main reasons for this::

 * It is fast and portable
 * I like C

I've also started to work in a python binding for taningia. It is far to
be considered minimally good but it is already possible to taste some
nice things of our lib through python =D

Feel free to send me a new port to any language you like!
