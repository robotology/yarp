/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/sig/IntrinsicParams.h>
#include <yarp/os/Time.h>

using namespace yarp::sig;

IntrinsicParams::IntrinsicParams(): physFocalLength(0.0),principalPointX(0.0),
                   principalPointY(0.0), focalLengthX(0.0), focalLengthY(0.0),
                   distortionModel(), isOptional(false) {}

IntrinsicParams::IntrinsicParams(const yarp::os::Property &intrinsic, bool isOptional): isOptional(isOptional)
{
    fromProperty(intrinsic);
}

void IntrinsicParams::toProperty(yarp::os::Property& intrinsic) const
{
    intrinsic.put("physFocalLength", physFocalLength);
    intrinsic.put("focalLengthX", focalLengthX);
    intrinsic.put("focalLengthY", focalLengthY);
    intrinsic.put("principalPointX", principalPointX);
    intrinsic.put("principalPointY", principalPointY);
    intrinsic.put("stamp", yarp::os::Time::now());

    if (distortionModel.type != YarpDistortion::YARP_PLUMB_BOB) {
        intrinsic.put("distortionModel", "none");
        return;
    }
    intrinsic.put("distortionModel", "plumb_bob");
    intrinsic.put("k1", distortionModel.k1);
    intrinsic.put("k2", distortionModel.k2);
    intrinsic.put("t1", distortionModel.t1);
    intrinsic.put("t2", distortionModel.t2);
    intrinsic.put("k3", distortionModel.k3);
}

void IntrinsicParams::fromProperty(const yarp::os::Property& intrinsic)
{
    yAssert(intrinsic.check("focalLengthX")    &&
            intrinsic.check("focalLengthY")    &&
            intrinsic.check("principalPointX") &&
            intrinsic.check("principalPointY"));
    focalLengthX    = intrinsic.find("focalLengthX").asFloat64();
    focalLengthY    = intrinsic.find("focalLengthY").asFloat64();
    principalPointX = intrinsic.find("principalPointX").asFloat64();
    principalPointY = intrinsic.find("principalPointY").asFloat64();

    // The physical focal length is optional
    physFocalLength = intrinsic.check("physFocalLength", yarp::os::Value(0.0)).asFloat64();

    // The distortion parameters are optional
    if (intrinsic.find("distortionModel").asString() !=  "plumb_bob") {
        return;
    }
    distortionModel.type = YarpDistortion::YARP_PLUMB_BOB;
    distortionModel.k1 = intrinsic.check("k1", yarp::os::Value(0.0)).asFloat64();
    distortionModel.k2 = intrinsic.check("k2", yarp::os::Value(0.0)).asFloat64();
    distortionModel.t1 = intrinsic.check("t1", yarp::os::Value(0.0)).asFloat64();
    distortionModel.t2 = intrinsic.check("t2", yarp::os::Value(0.0)).asFloat64();
    distortionModel.k3 = intrinsic.check("k3", yarp::os::Value(0.0)).asFloat64();
}

bool IntrinsicParams::read(yarp::os::ConnectionReader& reader) {
    yarp::os::Property prop;
    bool ok = prop.read(reader);
    if (ok)
    {
        fromProperty(prop);
    }
    return ok;
}
bool IntrinsicParams::write(yarp::os::ConnectionWriter& writer) const {
    yarp::os::Property prop;
    toProperty(prop);
    return prop.write(writer);
}
