// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
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
