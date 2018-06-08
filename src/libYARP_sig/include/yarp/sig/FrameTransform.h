/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_SIG_TRANSFORM_H
#define YARP_SIG_TRANSFORM_H

#include <yarp/sig/Vector.h>
#include <yarp/sig/Matrix.h>
#include <yarp/sig/api.h>
#include <yarp/sig/Quaternion.h>

namespace yarp
{
    namespace sig
    {
        class YARP_sig_API FrameTransform
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

            void rotFromRPY(double R, double P, double Y);
            yarp::sig::Vector getRPYRot() const;
            yarp::sig::Matrix toMatrix() const;

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

            std::string toString();
        };
    }
}

#endif

