#!/bin/bash
#
# Simple script to test Paul Epstein's sudoku solver.
# usage: run the command ./test.sh on a unix terminal
#
# Author: Ronald Macmaster
# Date: 6/22/16


# build the paul column

echo -e "\nPaul\n";
for var in {1..10}
do
	echo "Test $var:";
	echo $(time ./paul.exe);
done

# build the epi column

echo -e "\nEPI\n";
for var in {1..10}
do
	echo "Test $var:";
	echo $(time ./epi.exe);
done
