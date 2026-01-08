/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
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
#include <yarp/dev/Drivers.h>
#include <yarp/dev/IFrameGrabberImage.h>
#include <yarp/dev/IFrameGrabberControls.h>
#include <yarp/dev/IFrameGrabberControlsDC1394.h>
#include <yarp/dev/IFrameWriterImage.h>
#include <yarp/dev/GazeControl.h>
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
#include <yarp/dev/ISpeechSynthesizer.h>
#include <yarp/dev/ISpeechTranscription.h>
#include <yarp/dev/ILLM.h>
#include <yarp/dev/IChatBot.h>
#include <yarp/dev/IBattery.h>
#include <yarp/dev/ILocalization2D.h>
#include <yarp/dev/IMap2D.h>
#include <yarp/dev/INavigation2D.h>

#ifndef YARP_NO_MATH
#include <yarp/dev/IFrameTransform.h>
#endif // YARP_NO_MATH


#endif // YARP_DEV_ALL_H
