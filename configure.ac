#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.61])
AC_INIT([taningia], [0.2.2], [http://projects.comum.org/taningia])
AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE
AC_PROG_RANLIB
LT_INIT

AM_SILENT_RULES([yes])

# Being as picky as possible
WARNING_FLAGS="-Wall -Wextra -Werror -pedantic -std=c99"
AC_SUBST([WARNING_FLAGS])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CXX
AC_PROG_LIBTOOL

# Checks for libraries.
AC_CHECK_LIB([pthread], [pthread_create])
AC_CHECK_LIB([resolv], [res_querydomain],
              enable_libresolv=yes,
              enable_libresolv=no)
AM_CONDITIONAL([ENABLE_LIBRESOLV], [test "$enable_libresolv" = "yes"])

# Checking for pthread
ACX_PTHREAD([
        AC_SUBST([PTHREAD_CFLAGS])
        AC_SUBST([PTHREAD_LIBS])
	CC="$PTHREAD_CC"
],[
	AC_MSG_WARN([Compiling without threads support])
])

# Checks for header files.
AC_CHECK_HEADERS([string.h])
AC_HEADER_RESOLV

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_FUNC_REALLOC
AC_CHECK_FUNCS([strchr strdup strndup strrchr memmove memset])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_C_INLINE

if test "$ac_cv_c_inline" != no ; then
   AC_DEFINE([HAVE_INLINE], [1], [The compiler has support for inline functions])
   AC_SUBST([HAVE_INLINE])
fi

# Checks for .pc packages
PKG_CHECK_MODULES([IKSEMEL], [iksemel])
AC_SUBST([IKSEMEL_CFLAGS])
AC_SUBST([IKSEMEL_LIBS])

PKG_CHECK_MODULES([CHECK], [check >= 0.9.4])
AC_SUBST([CHECK_CFLAGS])
AC_SUBST([CHECK_LIBS])

AC_CONFIG_FILES([
  Makefile
  include/Makefile
  include/taningia/Makefile
  src/Makefile
  examples/Makefile
  tests/Makefile
  taningia.pc
])

AC_OUTPUT
