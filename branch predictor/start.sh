#!/bin/bash

make;

./predictor DIST-INT-1
./predictor DIST-INT-2
./predictor DIST-FP-1
./predictor DIST-FP-2
./predictor DIST-MM-1
./predictor DIST-MM-2
./predictor DIST-SERV-1
./predictor DIST-SERV-2

