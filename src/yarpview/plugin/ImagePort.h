/* 
 * Copyright (C) 2009 RobotCub Consortium
 * Author Lorenzo Natale
 * email:   lorenzo.natale@robotcub.org
 * website: www.robotcub.org
 *
 * Modified by: Davide Perrone
 * Date: Feb 2014
 * email:   dperrone@aitek.it
 * website: www.aitek.it
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LICENSE
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
