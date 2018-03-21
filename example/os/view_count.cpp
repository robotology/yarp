/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */


#include <yarp/os/all.h>
#include <stdio.h>
using namespace yarp::os;

int main(int argc, char *argv[]) {
	if (argc!=2) return 1;
	Network yarp;
    
    BufferedPort<Bottle> in;
    in.open(argv[1]);
    
	int count = 1;
	while (count>0) {
	    Bottle *msg = in.read();
		count = msg->get(1).asInt();
		printf("at %d\n", count);
	}
    
    return 0;
}
