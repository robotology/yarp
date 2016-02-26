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
    
    BufferedPort<Bottle> out;
    out.open(argv[1]);
    
    for (int i=10; i>=0; i--) {
        printf("at %d\n", i);
	    Bottle& msg = out.prepare();
		msg.clear();
		msg.addString("countdown");
		msg.addInt(i);
		out.write();
		Time::delay(1);
    }
    return 0;
}
