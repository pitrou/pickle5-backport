#!/bin/sh

# DIFF=colordiff
DIFF="diff -u"

CPYTHON=../default

$DIFF $CPYTHON/Include/picklebufobject.h pickle5/
$DIFF $CPYTHON/Objects/picklebufobject.c pickle5/
$DIFF $CPYTHON/Modules/_pickle.c pickle5/
$DIFF $CPYTHON/Lib/pickle.py pickle5/
