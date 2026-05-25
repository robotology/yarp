/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/BufferedPort.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Time.h>
#include <mutex>

//TODO: finish the single port mode.. the struct below is for this purpose
struct JoyData : public yarp::os::Portable
{
    yarp::sig::Vector Buttons;
    yarp::sig::Vector Sticks;
    yarp::sig::Vector Axes;
    yarp::sig::Vector Balls;
    yarp::sig::Vector Touch;
    yarp::sig::VectorOf<unsigned char> Hats;

    bool read(yarp::os::ConnectionReader& connection) override;
    bool write(yarp::os::ConnectionWriter& connection) const override;
};
