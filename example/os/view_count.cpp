/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
