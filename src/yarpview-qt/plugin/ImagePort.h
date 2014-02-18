// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/* 
 * Copyright (C) 2009 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author Lorenzo Natale
 * email:   lorenzo.natale@robotcub.org
 * website: www.robotcub.org
 *
 * Modified by: Davide Perrone
 * Date: Feb 2014
 * email:   dperrone@aitek.it
 * website: www.aitek.it
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef __IMGPORT__
#define __IMGPORT__

#include <yarp/sig/Image.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/PortReaderBuffer.h>
#include <yarp/os/Time.h>

#include <string>

#include <QObject>
#include <QVideoFrame>
#include "signalhandler.h"

/*! \class InputCallback
    \brief This class is a callback class that receives the video frame from the YARP backend
*/
class InputCallback: public QObject, public yarp::os::TypedReaderCallback<yarp::sig::FlexImage>
{

public:
    InputCallback();
    ~InputCallback();
    void setSignalHandler(SignalHandler*);
    void onRead(yarp::sig::FlexImage &img);
 
private:
    int counter;
    SignalHandler *sigHandler;
};

#endif
