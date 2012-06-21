dnl $Id$
dnl config.m4 for extension dd

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(dd, for dd support,
dnl Make sure that the comment is aligned:
dnl [  --with-dd             Include dd support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(dd, whether to enable dd support,
Make sure that the comment is aligned:
[  --enable-dd           Enable dd support])

if test "$PHP_DD" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-dd -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/dd.h"  # you most likely want to change this
  dnl if test -r $PHP_DD/$SEARCH_FOR; then # path given as parameter
  dnl   DD_DIR=$PHP_DD
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for dd files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       DD_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$DD_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the dd distribution])
  dnl fi

  dnl # --with-dd -> add include path
  dnl PHP_ADD_INCLUDE($DD_DIR/include)

  dnl # --with-dd -> check for lib and symbol presence
  dnl LIBNAME=dd # you may want to change this
  dnl LIBSYMBOL=dd # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $DD_DIR/lib, DD_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_DDLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong dd lib version or lib not found])
  dnl ],[
  dnl   -L$DD_DIR/lib -lm
  dnl ])
  dnl
  dnl PHP_SUBST(DD_SHARED_LIBADD)

  PHP_ADD_LIBRARY(stdc++, 1, DD_SHARED_LIBADD)
  PHP_ADD_LIBRARY(mcrypt, 1, DD_SHARED_LIBADD)
  PHP_ADD_LIBRARY(curl,   1, DD_SHARED_LIBADD)
  PHP_NEW_EXTENSION(dd, dd.cc dd_cookie.cc base64.cc md5.cc dd_stat.cc http.c, $ext_shared)
  PHP_REQUIRE_CXX()
  PHP_SUBST(DD_SHARED_LIBADD)
fi
