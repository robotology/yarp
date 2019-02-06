/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_MATH_TRANSFORM_H
#define YARP_MATH_TRANSFORM_H

#include <yarp/sig/Vector.h>
#include <yarp/sig/Matrix.h>
#include <yarp/math/api.h>
#include <yarp/math/Math.h>
#include <yarp/math/Quaternion.h>

namespace yarp
{
    namespace math
    {
        class YARP_math_API FrameTransform
        {
            public:
            YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) src_frame_id;
            YARP_SUPPRESS_DLL_INTERFACE_WARNING_ARG(std::string) dst_frame_id;
            double      timestamp;

            struct Translation_t
            {
                double tX;
                double tY;
                double tZ;

                void set(double x, double y, double z)
                {
                    tX = x;
                    tY = y;
                    tZ = z;
                }
            } translation;

            Quaternion rotation;

            FrameTransform()
            {
                timestamp = 0;
                translation.set(0, 0, 0);
            }

            FrameTransform
                (
                const std::string& parent,
                const std::string& child,
                double             inTX,
                double             inTY,
                double             inTZ,
                double             inRX,
                double             inRY,
                double             inRZ,
                double             inRW
                )
            {
                src_frame_id = parent;
                dst_frame_id = child;
                translation.set(inTX, inTY, inTZ);
                rotation.w() = inRW;
                rotation.x() = inRX;
                rotation.y() = inRY;
                rotation.z() = inRZ;
            }

            ~FrameTransform(){};

            void transFromVec(double X, double Y, double Z)
            {
                translation.set(X, Y, Z);
            }

            void rotFromRPY(double R, double P, double Y)
            {
                double               rot[3] = { R, P, Y };
                size_t               i = 3;
                yarp::sig::Vector    rotV;
                yarp::sig::Matrix    rotM;
                rotV = yarp::sig::Vector(i, rot);
                rotM = rpy2dcm(rotV);
                rotation.fromRotationMatrix(rotM);
            }

            yarp::sig::Vector getRPYRot() const
            {
                yarp::sig::Vector rotV;
                yarp::sig::Matrix rotM;
                rotM = rotation.toRotationMatrix4x4();
                rotV = dcm2rpy(rotM);
                return rotV;
            }

            yarp::sig::Matrix toMatrix() const
            {
                yarp::sig::Vector rotV;
                yarp::sig::Matrix t_mat(4,4);
                t_mat = rotation.toRotationMatrix4x4();
                t_mat[0][3] = translation.tX;
                t_mat[1][3] = translation.tY;
                t_mat[2][3] = translation.tZ;
                return t_mat;
            }

            bool fromMatrix(const yarp::sig::Matrix& mat)
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

            std::string toString() const;
        };
    }
}

#endif

