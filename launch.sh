#!/bin/bash

#please make sur that make was already done for main_API
echo 'please make sur that make was already done for main_API'
cd ./src
make clean
make all
cd ../examples
make clean
make all
for i in {0 10 1}
do
  ./test_dot
done

