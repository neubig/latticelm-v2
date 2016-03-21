latticelm: A toolkit for unsupervised segmentation/language model learning
==============================================================================
by Graham Neubig (neubig@is.naist.jp)

Install
-------

First, in terms of standard libraries, you must have autotools, libtool, and Boost. If
you are on Ubuntu/Debian linux, you can install them below:

    $ sudo apt-get install autotools libtool libboost-all

You must install OpenFST separately.

Once these two packages are installed, run the following commands, specifying the
correct path for openfst.

    $ autoreconf -i
    $ ./configure --with-openfst=/path/to/openfst
    $ make

Usage
-----

TODO

TODO
----

TODOs that need to be fixed before things are usable:

* Implement hierarchical LM that performs word segmentation
* Fix compile path for openfst
* Currently not performing interpolation for existing ngrams

TODOs that would be nice to fix:

* Trim nodes that have no customers
* Implement beam search/slice sampling
* Implement Metropolis-Hastings
* Ability to evaluate phoneme error rate during learning
* Unit tests
