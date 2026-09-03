/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

namespace yarp yarp.dev

struct yReturnValue {
} (
  yarp.name = "yarp::dev::ReturnValue"
  yarp.includefile = "yarp/dev/ReturnValue.h"
)

struct yTrackballData {
} (
  yarp.name = "yarp::dev::TrackballData"
  yarp.includefile = "yarp/dev/TrackballData.h"
)

struct yTouchData {
} (
  yarp.name = "yarp::dev::TouchData"
  yarp.includefile = "yarp/dev/TouchData.h"
)

struct yStickData {
} (
  yarp.name = "yarp::dev::StickData"
  yarp.includefile = "yarp/dev/StickData.h"
)

enum yJoyStickMode {
} (
  yarp.name = "yarp::dev::IJoypadController::JoypadCtrl_coordinateMode"
  yarp.includefile = "yarp/dev/IJoypadController.h"
  yarp.enumbase = "int32_t"
)

//-----------------------------

struct ButtonDataList
{
    1: list<double> value;
}

struct AxisDataList
{
    1: list<double> value;
}

struct HatsDataList
{
    1: list<byte> value;
}

struct MultipleTouches
{
    1: list<yTouchData> touches;
}

struct TouchesDataList
{
    1: list<MultipleTouches> value;
}

struct StickDataList
{
    1: list<yStickData> value;
}

struct TrackballDataList
{
    1: list<yTrackballData> value;
}

struct AllJoyData
{
    1: list<double>           ButtonDataVal;
    2: list<double>           AxisDataVal;
    3: list<byte>             HatsDataVal;
    4: list<MultipleTouches>  TouchDataVal;
    5: list<yStickData>       StickDataVal;
    6: list<yTrackballData>   TrackballDataVal;
}

//-----------------------------

struct return_getAxisCount{
    1: yReturnValue ret;
    2: i32 axis_count;
}

struct return_getTrackballCount{
    1: yReturnValue ret;
    2: i32 Trackball_count;
}

struct return_getHatCount{
    1: yReturnValue ret;
    2: i32 Hat_count;
}

struct return_getTouchSurfaceCount{
    1: yReturnValue ret;
    2: i32 touch_count;
}

struct return_getStickCount{
    1: yReturnValue ret;
    2: i32 stick_count;
}

struct return_getButtonCount{
    1: yReturnValue ret;
    2: i32 button_count;
}

struct return_getButton{
    1: yReturnValue ret;
    2: double value;
}

struct return_getTrackball{
    1: yReturnValue ret;
    2: yTrackballData value;
}

struct return_getHat{
    1: yReturnValue ret;
    2: i32 value;
}

struct return_getAxis{
    1: yReturnValue ret;
    2: double value;
}

struct return_getStick{
    1: yReturnValue ret;
    2: yStickData value;
    3: i16 coordinate_mode_enum;
}

struct return_getTouch{
    1: yReturnValue ret;
    2: list<yTouchData> value;
}

struct return_getAllAxes{
    1: yReturnValue ret;
    2: list<double> value;
}

service IJoypadControlMsgs {
    return_getAxisCount         getAxisCount();
    return_getButtonCount       getButtonCount();
    return_getTrackballCount    getTrackballCount();
    return_getHatCount          getHatCount();
    return_getTouchSurfaceCount getTouchSurfaceCount();
    return_getStickCount        getStickCount();

    return_getButton            getButton(1: i32 button_id);
    return_getTrackball         getTrackball(1: i32 trackball_id);
    return_getHat               getHat(1: i32 hat_id);
    return_getAxis              getAxis(1: i32 axis_id);
    return_getAllAxes           getAllAxes();
    return_getStick             getStick(1: i32 stick_id, 2: yJoyStickMode mode);
    return_getTouch             getTouch(1: i32 touch_id);
}
