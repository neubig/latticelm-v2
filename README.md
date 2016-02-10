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
