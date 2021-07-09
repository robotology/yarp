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
#include <yarp/dev/DriverLinkCreator.h>
#include <yarp/dev/Drivers.h>
#include <yarp/dev/IFrameGrabberImage.h>
#include <yarp/dev/IFrameGrabberControls.h>
#include <yarp/dev/IFrameGrabberControlsDC1394.h>
#include <yarp/dev/IFrameWriterImage.h>
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

#ifndef YARP_NO_DEPRECATED // since YARP 3.0
#define YARP_INCLUDING_DEPRECATED_HEADER_YARP_DEV_FRAMEGRABBERCONTROL2_H_ON_PURPOSE
#include <yarp/dev/FrameGrabberControl2.h>
#undef YARP_INCLUDING_DEPRECATED_HEADER_YARP_DEV_FRAMEGRABBERCONTROL2_H_ON_PURPOSE
#endif // YARP_NO_DEPRECATED

#ifndef YARP_NO_DEPRECATED // since YARP 3.3
#define YARP_INCLUDING_DEPRECATED_HEADER_YARP_DEV_DATASOURCE_H_ON_PURPOSE
#include <yarp/dev/DataSource.h>
#undef YARP_INCLUDING_DEPRECATED_HEADER_YARP_DEV_DATASOURCE_H_ON_PURPOSE
#endif // YARP_NO_DEPRECATED

#ifndef YARP_NO_DEPRECATED // since YARP 3.5
#define YARP_INCLUDING_DEPRECATED_HEADER_YARP_DEV_FRAMEGRABBER_H_ON_PURPOSE
#include <yarp/dev/IFrameGrabber.h>
#undef YARP_INCLUDING_DEPRECATED_HEADER_YARP_DEV_FRAMEGRABBER_H_ON_PURPOSE
#define YARP_INCLUDING_DEPRECATED_HEADER_YARP_DEV_FRAMEGRABBERRGB_H_ON_PURPOSE
#include <yarp/dev/IFrameGrabberRgb.h>
#undef YARP_INCLUDING_DEPRECATED_HEADER_YARP_DEV_FRAMEGRABBERRGB_H_ON_PURPOSE
#endif // YARP_NO_DEPRECATED

#endif // YARP_DEV_ALL_H
