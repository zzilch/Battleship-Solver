#!/bin/bash
for i in {1..36}
do
./build/solver 1 $i ./data.txt
./build/solver 3 $i ./data.txt
done