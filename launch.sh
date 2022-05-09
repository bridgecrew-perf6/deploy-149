#!/bin/bash

#please make sur that make was already done for main_API
cd ./src
make clean
make all
cd ../examples
make clean
make all
./test_dot
