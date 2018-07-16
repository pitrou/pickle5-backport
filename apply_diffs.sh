#!/bin/sh

# Port the newest contents of the CPython "pickle5" branch and apply them
# to this project tree.

CPYTHON=../default

cp -f $CPYTHON/Include/picklebufobject.h pickle5/
cp -f $CPYTHON/Objects/picklebufobject.c pickle5/
cp -f $CPYTHON/Modules/_pickle.c pickle5/
cp -f $CPYTHON/Modules/clinic/_pickle.c.h pickle5/clinic/
cp -f $CPYTHON/Lib/pickle.py pickle5/
cp -f $CPYTHON/Lib/pickletools.py pickle5/
cp -f $CPYTHON/Lib/test/pickletester.py pickle5/test/
cp -f $CPYTHON/Lib/test/test_pickle.py pickle5/test/

patch -p0
