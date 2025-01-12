#!/bin/bash

cd gkom_environment_rendering
mkdir -p build
cd build
cmake ..
make
./gkom_environment_rendering