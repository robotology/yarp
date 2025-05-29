#!/bin/sh

# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

let x=1
let top=200
while [ $x -le $top ]; do
    echo $x of $top
    echo Hello | yarp write /write
    let x=x+1
done
