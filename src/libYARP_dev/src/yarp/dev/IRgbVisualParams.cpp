/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/IRgbVisualParams.h>

yarp::dev::IRgbVisualParams::~IRgbVisualParams() = default;

yarp::dev::CameraConfig::CameraConfig(int w, int h, double rate, YarpVocabPixelTypesEnum enc)
{
    width = w;
    height = h;
    framerate = rate;
    pixelCoding = enc;
}

bool yarp::dev::CameraConfig::write(yarp::os::ConnectionWriter& connection) const
{
    connection.appendInt32(BOTTLE_TAG_LIST);
    connection.appendInt32(4);
    connection.appendInt32(BOTTLE_TAG_INT32); // 1
    connection.appendInt32(width);
    connection.appendInt32(BOTTLE_TAG_INT32); // 2
    connection.appendInt32(height);
    connection.appendInt32(BOTTLE_TAG_FLOAT64); // 3
    connection.appendFloat64(framerate);
    connection.appendInt32(BOTTLE_TAG_INT32); // 4
    connection.appendInt32(pixelCoding);

    connection.convertTextMode();
    return !connection.isError();
}

bool yarp::dev::CameraConfig::read(yarp::os::ConnectionReader& connection)
{
    // auto-convert text mode interaction
    connection.convertTextMode();

    bool ok = true;
    connection.expectInt32();
    connection.expectInt32();

    connection.expectInt32();
    width = connection.expectInt32(); //1
    connection.expectInt32();
    height = connection.expectInt32(); //2
    connection.expectInt32();
    framerate = connection.expectFloat64(); //3
    connection.expectInt32();
    pixelCoding = (YarpVocabPixelTypesEnum)(connection.expectInt32()); //4

    return !connection.isError();
}
