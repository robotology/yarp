/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/all.h>
#include <yarp/sig/all.h>

#include "MjpegDecompression.h"

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::mjpeg;

int main(int argc, char *argv[]) {
    Network yarp;

    Property options;
    options.fromCommand(argc,argv);

    ConstString inName = options.check("in",Value("/mjpeg/in")).asString();
    ConstString outName = options.check("out",Value("/mjpeg/out")).asString();

    BufferedPort<ManagedBytes> in; 
    // BufferedPort<Bottle> would also work fine, but less efficient

    BufferedPort<ImageOf<PixelRgb> > out;

    if (!in.open(inName)) return 1;
    if (!out.open(outName)) return 1;

    MjpegDecompression decompression;
    if (decompression.isAutomatic()) {
        fprintf(stderr,"For this test, turn MJPEG_AUTOCOMPRESS off\n");
        return 1;
    }
    while (true) {
        ManagedBytes *data = in.read();
        if (!data) continue;
        if (!decompression.decompress(data->bytes(),out.prepare())) {
            fprintf(stderr,"Decompression failed!\n");
            return 1;
        }
        out.write();
    }

    return 0;
}

