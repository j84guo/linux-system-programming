#!/bin/bash

# copies 2 MB in 1 byte chunks
dd bs=1 count=2097152 if=/dev/zero of=pirate.txt

# copies 2 MB in 1024 byte chunks
dd bs=1024 count=2048 if=/dev/zero of=pirate.txt

# copies 2 MB in 1031 byte chunks
# this takes longer than 102 byte chunks because 
# the requests become unaligned with block boundaries
# we can infer that the block size is likely an integer
# multiple of 1024 or an integer divisor
dd bs=1031 count=2048 if=/dev/zero of=pirate.txt
