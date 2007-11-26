// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


/*
 *
 * Confusingly, this is NOT the source of the program currently 
 * called "yarpview"!  That is in src/yarp-imageviewer.
 *
 */


#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

int main(int argc, char *argv[]) {
    Network yarp;

    PolyDriver source, sink;

    Property pSource;
    pSource.put("device","remote_grabber");
    pSource.put("local","/foo");
    source.open(pSource);

    Property pSink;
    pSink.put("device","ffmpeg_writer");
    pSink.put("out","test.mpg");
    pSink.put("framerate",25);
    sink.open(pSink);

    IFrameGrabberImage *iSource;
    source.view(iSource);
    IFrameWriterImage *iSink;
    sink.view(iSink);

    bool done = false;

    if (iSource==NULL||iSink==NULL) {
        if (iSource!=NULL) {
            printf("Cannot find image source\n");
        }
        if (iSink!=NULL) {
            printf("Cannot find image sink\n");
        }
        done = true;
    }

    ImageOf<PixelRgb> image;
    while (!done) {
        bool ok = iSource->getImage(image);
        if (ok) {
            ok = iSink->putImage(image);
        } 
        if (!ok) {
            done = true;
        }
    }

    sink.close();
    source.close();

    return 0;
}

