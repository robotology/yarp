/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_DEV_ALL_H
#define YARP_DEV_ALL_H

#include <yarp/dev/AudioGrabberInterfaces.h>
#include <yarp/dev/AudioVisualInterfaces.h>
#include <yarp/dev/CalibratorInterfaces.h>
#include <yarp/dev/CartesianControl.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/IAxisInfo.h>
#include <yarp/dev/IAmplifierControl.h>
#include <yarp/dev/IControlDebug.h>
#include <yarp/dev/IControlLimits.h>
#include <yarp/dev/ControlBoardPid.h>
#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/DriverLinkCreator.h>
#include <yarp/dev/Drivers.h>
#include <yarp/dev/FrameGrabberInterfaces.h>
#include <yarp/dev/FrameGrabberControl2.h>
#include <yarp/dev/GazeControl.h>
#include <yarp/dev/IGenericSensor.h>
#include <yarp/dev/GPUInterface.h>
#include <yarp/dev/IAnalogSensor.h>
#include <yarp/dev/IRGBDSensor.h>
#include <yarp/dev/IHapticDevice.h>
#include <yarp/dev/IRangefinder2D.h>
#include <yarp/dev/IRGBDSensor.h>
#include <yarp/dev/IVisualServoing.h>
#include <yarp/dev/MultipleAnalogSensorsInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/ServiceInterfaces.h>
#include <yarp/dev/IWrapper.h>
#include <yarp/dev/IMultipleWrapper.h>

#ifndef YARP_NO_DEPRECATED // since YARP 3.3
#define YARP_INCLUDING_DEPRECATED_HEADER_ON_PURPOSE
#include <yarp/dev/DataSource.h>
#undef YARP_INCLUDING_DEPRECATED_HEADER_ON_PURPOSE
#endif // YARP_NO_DEPRECATED

#endif // YARP_DEV_ALL_H
