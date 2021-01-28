/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
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

#ifndef IMGPORT
#define IMGPORT

#include <yarp/sig/Image.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/PortReaderBuffer.h>
#include <yarp/os/Time.h>
#include <yarp/conf/numeric.h>

#include <string>

#include <QObject>
#include <QVideoFrame>
#include "signalhandler.h"

/*! \class InputCallback
    \brief This class is a callback class that receives the video frame from the YARP backend
*/

#ifdef YARP_LITTLE_ENDIAN
class InputCallback: public QObject, public yarp::os::TypedReaderCallback<yarp::sig::ImageOf<yarp::sig::PixelBgra> >
#else
class InputCallback: public QObject, public yarp::os::TypedReaderCallback<yarp::sig::ImageOf<yarp::sig::PixelRgb> >
#endif
{

public:
    InputCallback();
    ~InputCallback();
    void setSignalHandler(SignalHandler*);

#ifdef YARP_LITTLE_ENDIAN
    using yarp::os::TypedReaderCallback<yarp::sig::ImageOf<yarp::sig::PixelBgra> >::onRead;
    void onRead(yarp::sig::ImageOf<yarp::sig::PixelBgra> &img) override;
#else
    using yarp::os::TypedReaderCallback<yarp::sig::ImageOf<yarp::sig::PixelRgb> >::onRead;
    void onRead(yarp::sig::ImageOf<yarp::sig::PixelRgba> &img) override;
#endif

private:
    int counter;
    SignalHandler *sigHandler;
};

#endif
