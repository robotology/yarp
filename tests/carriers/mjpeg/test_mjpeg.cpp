/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/all.h>
#include <yarp/sig/all.h>

#include <MjpegDecompression.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::mjpeg;

int main(int argc, char *argv[]) {
    Network yarp;

    Property options;
    options.fromCommand(argc,argv);

    std::string inName = options.check("in",Value("/mjpeg/in")).asString();
    std::string outName = options.check("out",Value("/mjpeg/out")).asString();

    BufferedPort<ManagedBytes> in; 
    // BufferedPort<Bottle> would also work fine, but less efficient

    BufferedPort<FlexImage> out;

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

