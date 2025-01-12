#!/bin/bash

mkdir -p build
cd build
cmake ..
make
./gkom_environment_rendering