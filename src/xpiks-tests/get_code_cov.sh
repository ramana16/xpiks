#!/bin/bash

echo "Calculating code coverage for directory: $1"

for filename in `find $1 | egrep '\.cpp'`; 
do
    gcov-5 -n -o . $filename > /dev/null; 
done
