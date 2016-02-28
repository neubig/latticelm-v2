#!/usr/bin/python

from __future__ import print_function
import sys

def from_plf(plf):
    """ Takes a string representing a single lattice in Python Lattice Format
    (PLF) and outputs a textual representation for use with latticelm."""

    # Secure your lattices, I guess?
    try:
        plf = eval(plf)
    except:
        return []

    num_nodes = len(plf)+1 # Add one for the starting node.

    i = 0
    output_lattice = []
    for outgoing_edges in plf:
        for edge in outgoing_edges:
            # Converting logs to negative logs with the last argument.
            output_lattice.append("%d\t%d\t%s\t%s\t%f" % (
                    i, i+edge[2], edge[0], edge[0], edge[1]*-1))
        i += 1
    return output_lattice

def convert_file(input_fn, output_fn):
    """ Takes input and output filenames as strings. Reading PLF lattices from
    the input file outputs lattices in a format to be read by latticelm. """
    with open(input_fn) as input_file:
        input_lines = input_file.readlines()
    with open(output_fn, "w") as output_file:
        for plf in input_lines:
            output_lattice = from_plf(plf)
            #print(output_lattice)
            #raw_input()
            for line in output_lattice:
                print(line, file=output_file)
            print("",file=output_file)

#plf = "((('einen',1.0,1),),(('wettbewerbsbedingten',0.5,2),('wettbewerbs',0.25,1),('wettbewerb',0.25, 1),),(('bedingten',1.0,1),),(('preissturz',0.5,2),('preis',0.5,1),),(('sturz',1.0,1),),)"
#from_plf(plf)

input_fn, output_fn = sys.argv[1:]
convert_file(input_fn, output_fn)
