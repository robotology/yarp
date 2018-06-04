/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/sig/FrameTransform.h>
#include <yarp/math/Math.h>
#include <cstdio>

using namespace yarp::sig;
using namespace yarp::math;

void FrameTransform::rotFromRPY(double R, double P, double Y)
{
    double               rot[3] = { R, P, Y };
    size_t               i = 3;
    yarp::sig::Vector    rotV;
    yarp::sig::Matrix    rotM;
    rotV = yarp::sig::Vector(i, rot);
    rotM = rpy2dcm(rotV);
    rotation.fromRotationMatrix(rotM);
}

yarp::sig::Vector FrameTransform::getRPYRot() const
{
    yarp::sig::Vector rotV;
    yarp::sig::Matrix rotM;
    rotM = rotation.toRotationMatrix4x4();
    rotV = dcm2rpy(rotM);
    return rotV;
}

yarp::sig::Matrix FrameTransform::toMatrix() const
{
    yarp::sig::Vector rotV;
    yarp::sig::Matrix t_mat(4, 4);
    t_mat = rotation.toRotationMatrix4x4();
    t_mat[0][3] = translation.tX;
    t_mat[1][3] = translation.tY;
    t_mat[2][3] = translation.tZ;
    return t_mat;
}
