/*
 * Copyright: (C) 2010 Renaud Detry
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

/**
 * @file
 */


#include <stdio.h>
#include <unistd.h>
#include <string>

#define cimg_debug 0
#include "CImg.h"

#include "YarpCImgViewer.h"

#include "stmac.h"

using namespace cimg_library;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::sig::draw;


YarpCImgViewer::YarpCImgViewer(const char* portName) :
    portName(portName), verbose(false),
    dumpID(0), udelay(500000),
    viewerWindow(NULL), viewerImage(NULL)
{
    videoPort.open(portName);

    viewerWindow = new CImgDisplay(512, 512, "viewer", 0);
    viewerImage = new CImg<unsigned char>(512, 512, 1, 3, 0);
}


YarpCImgViewer::~YarpCImgViewer()
{
    delete viewerWindow;
    delete viewerImage;
}


void YarpCImgViewer::acquire()
{
    viewerWindow->title("Acquisition");

    while (!viewerWindow->closed)
        {
            ImageOf<PixelRgb> *imgIn = videoPort.read(true);

            if (imgIn == NULL)
                {
                    // This is actually normal behavior right now -- it happens when
                    // MCAST disconnects are occurring on the network.  Eventually
                    // this will be hidden by the library, but until then NULLs 
                    // can be just silently ignored as "normal".
                    //fprintf(stderr, "Port::read returned NULL\n");
                    continue;
                }

            if (viewerImage->dimx() != imgIn->width() ||
                viewerImage->dimy() != imgIn->height())
                {
                    delete viewerImage;
                    viewerImage = new CImg<unsigned char>(imgIn->width(), imgIn->height(), 1, 3);
                    viewerWindow->resize(*viewerImage);
                }

            // Pixel by pixel copy.
            // This is not efficient, but a raw memcpy between buffers fails.
            // fixme: copy line by line
            for (int i = 0; i < imgIn->width(); i++)
                {
                    for (int j = 0; j < imgIn->height(); j++)
                        {
                            PixelRgb& pixel = imgIn->pixel(i,j);
                            (*viewerImage)(i,j,0,0) = pixel.r;
                            (*viewerImage)(i,j,0,1) = pixel.g;
                            (*viewerImage)(i,j,0,2) = pixel.b;
                        }
                }


            if (!dumpName.empty())
                {
                    char number[1024];
                    snprintf(number, 1024, dumpName.c_str(), dumpID++);

                    viewerImage->save(number);
                }

            viewerImage->display(*viewerWindow);
        }
}

void YarpCImgViewer::broadcast()
{
    viewerWindow->title("Broadcast");

    while (!viewerWindow->closed)
        {

            if (!dumpName.empty())
                {
                    char number[1024];
                    snprintf(number, 1024, dumpName.c_str(), dumpID++);

                    try {
                        viewerImage->load(number);
                    } catch (CImgException &e) {
                        dumpID = 0;
                        char number[1024];
                        snprintf(number, 1024, dumpName.c_str(), dumpID++);
                        viewerImage->load(number);
                    }
                }
            else
                {
                    fprintf(stderr, "Error: no dump name\n");
                    exit(1);
                }

            ImageOf<PixelRgb>& imgOut = videoPort.prepare();

            imgOut.resize(viewerImage->dimx(), viewerImage->dimy());

            // Pixel by pixel copy.
            // This is not efficient, but a raw memcpy between buffers fails.
            // fixme: copy line by line

            for (int i = 0; i < viewerImage->dimx(); i++)
                for (int j = 0; j < viewerImage->dimy(); j++)
                    imgOut.pixel(i,j) = PixelRgb((*viewerImage)(i,j,0,0),
                                                 (*viewerImage)(i,j,0,1),
                                                 (*viewerImage)(i,j,0,2));

            videoPort.write();

            if (viewerImage->dimx() != viewerWindow->dimx() ||
                viewerImage->dimy() != viewerWindow->dimy())
                viewerWindow->resize(*viewerImage);


            viewerImage->display(*viewerWindow);
            usleep(udelay);
        }
}


void YarpCImgViewer::dumpAtLoc(const char* dumpName)
{
    this->dumpName = dumpName;
}

void YarpCImgViewer::setUdelay(int udelay)
{
    this->udelay = udelay;
}


int YarpCImgViewer::main(int argc, char ** argv)
{
    if (argc<3)
        {
            printf("USAGE: %s --name /USERNAME/port\n", argv[0]);
            return 0;
        }
    Network yarp;

    Property options;
    options.fromCommand(argc,argv);

    Value& portName = options.find("name");
    if (portName.isNull())
        {
            fprintf(stderr, "Error: you must specify a --name\n");
            exit(1);
        }

    YarpCImgViewer ycv(portName.asString().c_str());

    Value& dumpName = options.find("dump");
    if (!dumpName.isNull())
        ycv.dumpAtLoc(dumpName.asString().c_str());

    Value& udelay = options.find("udelay");
    if (!udelay.isNull())
        ycv.setUdelay(udelay.asInt());

    Value& behavior = options.find("behave");
    if (!behavior.isNull() &&
        std::string(behavior.asString().c_str()) == std::string("broadcast"))
        ycv.broadcast();
    else
        ycv.acquire();

    return 0;
}

