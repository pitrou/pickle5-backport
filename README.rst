==========================================================
`pickle5` -- A backport of the pickle 5 protocol (PEP 574)
==========================================================

This package backports all features and APIs added in the ``pickle`` module
in Python 3.8.0, including the
`PEP 574 <https://www.python.org/dev/peps/pep-0574/>`_ additions.  It should
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
   :align: left

.. image:: https://ci.appveyor.com/api/projects/status/hk4lrl81wp0q283o/branch/master?svg=true
   :target: https://ci.appveyor.com/project/pitrou/pickle5-backport/branch/master
   :align: left
