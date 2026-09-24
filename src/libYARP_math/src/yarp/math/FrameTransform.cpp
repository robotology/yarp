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
                                            double             int_x,
                                            double             int_y,
                                            double             int_z,
                                            double             inRX,
                                            double             inRY,
                                            double             inRZ,
                                            double             inRW)
{
    src_frame_id = parent;
    dst_frame_id = child;
    translation = Translation (int_x, int_y, int_z);
    rotation = Quaternion (inRX, inRY, inRZ, inRW);
}

bool yarp::math::FrameTransform::isValid() const
{
    //if (isStatic==false && timestamp < 0 ) { return false; }
    //if (std::isnan(timestamp)) { return false; }
    if (rotation.isValid()==false) { return false;}
    return true;
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
    t_mat[0][3] = translation.t_x;
    t_mat[1][3] = translation.t_y;
    t_mat[2][3] = translation.t_z;
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

    translation.t_x = mat[0][3];
    translation.t_y = mat[1][3];
    translation.t_z = mat[2][3];
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
        translation.t_x,
        translation.t_y,
        translation.t_z,
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
        translation.t_x,
        translation.t_y,
        translation.t_z,
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
        rotM[0][3] = translation.t_x;
        rotM[1][3] = translation.t_y;
        rotM[2][3] = translation.t_z;
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
        translation.t_x,
        translation.t_y,
        translation.t_z,
        s_rotmr.c_str(),
        s_rotmd.c_str());
    }
    return std::string(buff);
}
