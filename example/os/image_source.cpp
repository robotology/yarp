/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/all.h>
#include <yarp/sig/all.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::sig::draw;

int main(int argc, char *argv[]) {
    Network yarp;
    BufferedPort<ImageOf<PixelRgb> > port;

    Property options;
    options.fromCommand(argc,argv);
    port.open(options.check("name",Value("/image")).asString());
  
    int ct = 0;
    while (true) {
        ImageOf<PixelRgb>& img = port.prepare();
        img.resize(100,100);
        img.zero();
        PixelRgb blue(0,0,255);
        addCircle(img,blue,ct,50,10);
        ct = (ct+5)%100;
        port.write();
        Time::delay(0.25);
    }
  
    return 0;
}
