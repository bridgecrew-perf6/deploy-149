#!/bin/bash

cd ./src
make clean
make all
cd ../examples
make clean
make all
./test_dot
