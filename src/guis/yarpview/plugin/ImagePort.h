/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: LGPL-2.1-or-later
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
