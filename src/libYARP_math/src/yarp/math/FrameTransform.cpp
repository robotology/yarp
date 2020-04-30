/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/math/FrameTransform.h>

#include <cstdio>


yarp::math::FrameTransform::FrameTransform() :
        timestamp(0)
{
    translation.set(0, 0, 0);
}

yarp::math::FrameTransform::FrameTransform (const std::string& parent,
                                            const std::string& child,
                                            double             inTX,
                                            double             inTY,
                                            double             inTZ,
                                            double             inRX,
                                            double             inRY,
                                            double             inRZ,
                                            double             inRW) :
        src_frame_id(parent),
        dst_frame_id(child),
        translation{inTX, inTY, inTZ},
        rotation(inRX, inRY, inRZ, inRW)
{
}

yarp::math::FrameTransform::~FrameTransform() = default;

void yarp::math::FrameTransform::transFromVec(double X, double Y, double Z)
{
    translation.set(X, Y, Z);
}

void yarp::math::FrameTransform::rotFromRPY(double R, double P, double Y)
{
    double               rot[3] = { R, P, Y };
    size_t               i = 3;
    yarp::sig::Vector    rotV;
    yarp::sig::Matrix    rotM;
    rotV = yarp::sig::Vector(i, rot);
    rotM = rpy2dcm(rotV);
    rotation.fromRotationMatrix(rotM);
}

yarp::sig::Vector yarp::math::FrameTransform::getRPYRot() const
{
    yarp::sig::Vector rotV;
    yarp::sig::Matrix rotM;
    rotM = rotation.toRotationMatrix4x4();
    rotV = dcm2rpy(rotM);
    return rotV;
}

yarp::sig::Matrix yarp::math::FrameTransform::toMatrix() const
{
    yarp::sig::Vector rotV;
    yarp::sig::Matrix t_mat(4,4);
    t_mat = rotation.toRotationMatrix4x4();
    t_mat[0][3] = translation.tX;
    t_mat[1][3] = translation.tY;
    t_mat[2][3] = translation.tZ;
    return t_mat;
}

bool yarp::math::FrameTransform::fromMatrix(const yarp::sig::Matrix& mat)
{
    if (mat.cols() != 4 || mat.rows() != 4)
    {
        yError("FrameTransform::fromMatrix() failed, matrix should be = 4x4");
        yAssert(mat.cols() == 4 && mat.rows() == 4);
        return false;
    }

    yarp::sig::Vector q;

    translation.tX = mat[0][3];
    translation.tY = mat[1][3];
    translation.tZ = mat[2][3];
    rotation.fromRotationMatrix(mat);
    return true;
}



std::string yarp::math::FrameTransform::toString() const
{
    char buff[1024];
    sprintf(buff, "%s -> %s \n tran: %f %f %f \n rot: %f %f %f %f \n\n",
                  src_frame_id.c_str(),
                  dst_frame_id.c_str(),
                  translation.tX,
                  translation.tY,
                  translation.tZ,
                  rotation.x(),
                  rotation.y(),
                  rotation.z(),
                  rotation.w());
    return std::string(buff);
}
