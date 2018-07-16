#!/bin/sh

# Generate the source diff necessary to port the CPython "pickle5" branch
# to this project tree.  This (ideally) only needs to be done once.

# DIFF=colordiff
DIFF="diff -u"

CPYTHON=../default

$DIFF $CPYTHON/Include/picklebufobject.h pickle5/
$DIFF $CPYTHON/Objects/picklebufobject.c pickle5/
$DIFF $CPYTHON/Modules/_pickle.c pickle5/
$DIFF $CPYTHON/Lib/pickle.py pickle5/
$DIFF $CPYTHON/Lib/pickletools.py pickle5/
$DIFF $CPYTHON/Lib/test/pickletester.py pickle5/test/
$DIFF $CPYTHON/Lib/test/test_pickle.py pickle5/test/
