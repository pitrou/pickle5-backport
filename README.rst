========================================================================
`pickle5` -- An experimental backport of the pickle 5 protocol (PEP 574)
========================================================================

.. warning::
   PEP 574 is still in draft phase.  Everything published here, including
   the exposed APIs, is subject to change in the future.


This package backports the features and APIs added in
`PEP 574 <https://www.python.org/dev/peps/pep-0574/>`_.  It should
work with Python 3.6 and 3.7.

Basic usage is similar to the ``pickle`` module, except that the module
to be imported is ``pickle5``:

.. code-block:: python

   import pickle5 as pickle

   pb = pickle.PickleBuffer(b"foo")
   data = pickle.dumps(pb, protocol=5)
   assert pickle.loads(data) == b"foo"

Detailed documentation can be found in PEP 574 and the standard
`pickle documentation <https://docs.python.org/3.8/library/pickle.html>`_.

.. image:: https://travis-ci.org/pitrou/pickle5-backport.svg?branch=master
    :target: https://travis-ci.org/pitrou/pickle5-backport
