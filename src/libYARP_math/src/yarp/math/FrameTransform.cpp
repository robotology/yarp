/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/math/FrameTransform.h>

#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/math/Math.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <cstdio>
#include <cmath>

namespace {
YARP_LOG_COMPONENT(FRAMETRANSFORM, "yarp.math.FrameTransform")
}

yarp::math::FrameTransform::FrameTransform()
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
    //yCDebug(FRAMETRANSFORM) << rotM.toString();
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
        yCError(FRAMETRANSFORM, "FrameTransform::fromMatrix() failed, matrix should be = 4x4");
        yCAssert(FRAMETRANSFORM, mat.cols() == 4 && mat.rows() == 4);
        return false;
    }

    yarp::sig::Vector q;

    translation.tX = mat[0][3];
    translation.tY = mat[1][3];
    translation.tZ = mat[2][3];
    rotation.fromRotationMatrix(mat);
    return true;
}



std::string yarp::math::FrameTransform::toString(display_transform_mode_t format) const
{
    char buff[1024];

    if (format == rotation_as_quaternion)
    {
        sprintf(buff, "%s -> %s \n tran: %f %f %f \n rot quaternion: %f %f %f %f\n\n",
        src_frame_id.c_str(),
        dst_frame_id.c_str(),
        translation.tX,
        translation.tY,
        translation.tZ,
        rotation.x(),
        rotation.y(),
        rotation.z(),
        rotation.w());
        /*
        Quaternion normrotation= rotation;
        normrotation.normalize();
        sprintf(buff, "%s -> %s \n tran: %f %f %f \n rot norm quaternion: %f %f %f %f\n\n",
        src_frame_id.c_str(),
        dst_frame_id.c_str(),
        translation.tX,
        translation.tY,
        translation.tZ,
        normrotation.x(),
        normrotation.y(),
        normrotation.z(),
        normrotation.w());
        */
    }
    else if (format == rotation_as_matrix)
    {
        yarp::sig::Matrix rotM;
        rotM = rotation.toRotationMatrix4x4();
        rotM[0][3] = translation.tX;
        rotM[1][3] = translation.tY;
        rotM[2][3] = translation.tZ;
        std::string s_rotm =rotM.toString();
        sprintf(buff, "%s -> %s \n transformation matrix:\n %s \n\n",
        src_frame_id.c_str(),
        dst_frame_id.c_str(),
        s_rotm.c_str());
    }
    else if (format == rotation_as_rpy)
    {
        yarp::sig::Vector rotVrad;
        yarp::sig::Matrix rotM;
        rotM = rotation.toRotationMatrix3x3();
        //yCDebug(FRAMETRANSFORM)<< rotM.toString();
        rotVrad = dcm2rpy(rotM);
        yarp::sig::Vector rotVdeg = rotVrad*180/M_PI;
        std::string s_rotmr = rotVrad.toString();
        std::string s_rotmd = rotVdeg.toString();
        sprintf(buff, "%s -> %s \n tran: %f %f %f \n rotation rpy: %s (deg %s)\n\n",
        src_frame_id.c_str(),
        dst_frame_id.c_str(),
        translation.tX,
        translation.tY,
        translation.tZ,
        s_rotmr.c_str(),
        s_rotmd.c_str());
    }
    return std::string(buff);
}

bool yarp::math::FrameTransform::read(yarp::os::ConnectionReader& connection)
{
    // auto-convert text mode interaction
    connection.convertTextMode();

    connection.expectInt32();
    connection.expectInt32();

    connection.expectInt32();
    src_frame_id = connection.expectString();
    connection.expectInt32();
    dst_frame_id = connection.expectString();
    connection.expectInt32();
    timestamp = connection.expectFloat64();
    connection.expectInt32();
    isStatic = (connection.expectInt8()==1);

    connection.expectInt32();
    translation.tX = connection.expectFloat64();
    connection.expectInt32();
    translation.tY = connection.expectFloat64();
    connection.expectInt32();
    translation.tZ = connection.expectFloat64();

    connection.expectInt32();
    rotation.x() = connection.expectFloat64();
    connection.expectInt32();
    rotation.y() = connection.expectFloat64();
    connection.expectInt32();
    rotation.z() = connection.expectFloat64();
    connection.expectInt32();
    rotation.w() = connection.expectFloat64();

    return !connection.isError();
}

bool yarp::math::FrameTransform::write(yarp::os::ConnectionWriter& connection) const
{
    connection.appendInt32(BOTTLE_TAG_LIST);
    connection.appendInt32(4+3+4);

    connection.appendInt32(BOTTLE_TAG_STRING);
    connection.appendString(src_frame_id);
    connection.appendInt32(BOTTLE_TAG_STRING);
    connection.appendString(dst_frame_id);
    connection.appendInt32(BOTTLE_TAG_FLOAT64);
    connection.appendFloat64(timestamp);
    connection.appendInt32(BOTTLE_TAG_INT8);
    connection.appendInt8(int8_t(isStatic));

    connection.appendInt32(BOTTLE_TAG_FLOAT64);
    connection.appendFloat64(translation.tX);
    connection.appendInt32(BOTTLE_TAG_FLOAT64);
    connection.appendFloat64(translation.tY);
    connection.appendInt32(BOTTLE_TAG_FLOAT64);
    connection.appendFloat64(translation.tZ);

    connection.appendInt32(BOTTLE_TAG_FLOAT64);
    connection.appendFloat64(rotation.x());
    connection.appendInt32(BOTTLE_TAG_FLOAT64);
    connection.appendFloat64(rotation.y());
    connection.appendInt32(BOTTLE_TAG_FLOAT64);
    connection.appendFloat64(rotation.z());
    connection.appendInt32(BOTTLE_TAG_FLOAT64);
    connection.appendFloat64(rotation.w());

    connection.convertTextMode();

    return !connection.isError();
}
