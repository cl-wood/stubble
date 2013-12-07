#!/bin/bash -x
rm -f input/input.* output/output.* taint/taint.* mutations/mutation.*
rm -f results.out file.out taint.out
rm -f buffer.out.*
rm -f inputFile.txt.*
rm -f output.*
echo > taint/explored.0

