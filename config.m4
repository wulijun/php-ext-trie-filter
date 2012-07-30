dnl $Id$
dnl config.m4 for extension trie_filter

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(trie_filter, for trie_filter support,
[  --with-trie_filter             Include trie_filter support])

if test "$PHP_TRIE_FILTER" != "no"; then
  SEARCH_PATH="/usr/local /usr"
  SEARCH_FOR="/include/datrie/trie.h"
  if test -r $PHP_TRIE_FILTER/$SEARCH_FOR; then
      TRIE_FILTER_DIR=$PHP_TRIE_FILTER
  else
      AC_MSG_CHECKING([for trie_filter files in default path])
      for i in $SEARCH_PATH ; do
          if test -r $i/$SEARCH_FOR; then
              TRIE_FILTER_DIR=$i
              AC_MSG_RESULT(found in $i)
          fi
      done
  fi

  if test -z "$TRIE_FILTER_DIR"; then
      AC_MSG_RESULT([not found])
      AC_MSG_ERROR([Please install the libdatrie])
  fi

  PHP_ADD_INCLUDE($TRIE_FILTER_DIR/include)

  LIBNAME=datrie
  LIBSYMBOL=trie_new_from_file

  PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  [
      PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $TRIE_FILTER_DIR/lib, TRIE_FILTER_SHARED_LIBADD)
      AC_DEFINE(HAVE_TRIE_FILTERLIB,1,[libdatrie found and included])
  ],[
      AC_MSG_ERROR([wrong libdatrie version or lib not found])
  ],[
      -L$TRIE_FILTER_DIR/lib -ldatrie
  ])

  PHP_SUBST(TRIE_FILTER_SHARED_LIBADD)
  PHP_NEW_EXTENSION(trie_filter, trie_filter.c, $ext_shared)
fi
