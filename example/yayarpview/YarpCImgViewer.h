/*
 * Copyright: (C) 2010 Renaud Detry
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

/**
 * @file
 */


#ifndef YarpCImgViewer_h
#define YarpCImgViewer_h

#include <ace/ACE.h>

#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <string>

/* Avoid including CImg.h in the header (it takes hours to parse) */
namespace cimg_library
{
    template<typename T> struct CImg;
    template<typename T> struct CImgl;
    struct CImgDisplay;
}


/**
 * @class YarpCImgViewer YarpCImgViewer.h
 *
 * @brief A YarpCImgViewer.
 * @author Renaud Detry <renaud.detry@student.ulg.ac.be>
 *
 * This class forwards video from a YARP port to an X11/WIN32
 * window.
 */

class YarpCImgViewer
{
public:

    /**
     * @arg \c portName
     */
    YarpCImgViewer(const char* portName);
    ~YarpCImgViewer();

    void acquire();
    void broadcast();

    void setUdelay(int udelay);
    void dumpAtLoc(const char* dumpName);

    /**
     * demo method
     */
    static int main(int argc, char ** argv);

private:
    yarp::os::BufferedPort< yarp::sig::ImageOf<yarp::sig::PixelRgb> > videoPort;

    std::string portName;
    std::string dumpName;
    bool verbose;
    int dumpID;
    int udelay;
    cimg_library::CImgDisplay *viewerWindow;
    cimg_library::CImg<unsigned char> *viewerImage;
};


#endif

