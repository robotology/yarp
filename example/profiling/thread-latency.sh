#!/bin/bash

# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

RATES="1 5 10 20 30 40 50 60 70 80 90 100 200 300 400 500 1000 2000"
NFRAMES=1000
KER=2.6.24

for rate in $RATES
  do
  echo "Testing thread latency at $rate"
  ../thread_latency --iterations $NFRAMES --period $rate
  reportFile="latency-$rate-$KER.txt"
  mv dump.txt $reportFile
done
