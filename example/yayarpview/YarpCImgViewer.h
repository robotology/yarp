/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2010 Renaud Detry
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
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

