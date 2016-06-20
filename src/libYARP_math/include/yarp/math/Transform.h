/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Authors: Marco Randazzo, Andrea Ruzzenenti
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_MATH_TRANSFORM_H
#define YARP_MATH_TRANSFORM_H

#include <yarp/sig/Vector.h>
#include <yarp/sig/Matrix.h>
#include <yarp/math/api.h>

namespace yarp
{
    namespace math 
    {
        class Transform_t
        {
            public:           
            std::string src_frame_id;
            std::string dst_frame_id;
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

            struct Rotation_t
            {
                double rX;
                double rY;
                double rZ;
                double rW;

                yarp::sig::Vector toQuaternion() const
                {
                    double q[4] = { rW, rX, rY, rZ };
                    return yarp::sig::Vector(4, q);
                }

                void fromQuaternion(const yarp::sig::Vector& q)
                {
                    rW = q[0];
                    rX = q[1];
                    rY = q[2];
                    rZ = q[3];
                }

                yarp::sig::Vector toRPY() const
                {
                    yarp::sig::Vector rotQ, rotV;
                    yarp::sig::Matrix rotM;
                    double rot[4] = { rW, rX, rY, rZ };
                    rotQ = yarp::sig::Vector(4, rot);
                    //to uncomment -- build yarp math
                    rotM = quat2dcm(rotQ);
                    rotV = dcm2rpy(rotM);
                    return rotV;
                }

                void set(double x, double y, double z, double w)
                {
                    rX = x;
                    rY = y;
                    rZ = z;
                    rW = w;
                }

                void set(double roll, double pitch, double yaw)
                {
                    double               rot[3] = { roll, pitch, yaw };
                    size_t               i;
                    yarp::sig::Vector    rotV, rotQ;
                    yarp::sig::Matrix    rotM;
                    i = 3;
                    rotV = yarp::sig::Vector(i, rot);
                    //to uncomment -- build yarp math
                    rotM = rpy2dcm(rotV);
                    rotQ = dcm2quat(rotM);
                    rW = rotQ[0];
                    rX = rotQ[1];
                    rY = rotQ[2];
                    rZ = rotQ[3];
                    return;
                }

            } rotation;

            Transform_t()
            {
                timestamp = 0;
                translation.set(0, 0, 0);
                rotation.set(0, 0, 0, 0);
            }

            Transform_t
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
                rotation.set(inRX, inRY, inRZ, inRW);
            }

            ~Transform_t(){};

            void transFromVec(double X, double Y, double Z)
            {
                translation.set(X, Y, Z);
            }

            void rotFromRPY(double R, double P, double Y)
            {
                rotation.set(R, P, Y);
            }

            yarp::sig::Vector getRPYRot() const
            {
                return rotation.toRPY();
            }

            yarp::sig::Matrix toMatrix() const
            {
                yarp::sig::Vector rotV;
                yarp::sig::Matrix mat;

                mat = quat2dcm(rotation.toQuaternion());
                mat[0][3] = translation.tX;
                mat[1][3] = translation.tY;
                mat[2][3] = translation.tZ;
                return mat;
            }

            bool fromMatrix(const yarp::sig::Matrix& mat)
            {
                if (mat.cols() != 4 || mat.rows() != 4)
                {
                    return false;
                }

                yarp::sig::Vector q;

                translation.tX = mat[0][3];
                translation.tY = mat[1][3];
                translation.tZ = mat[2][3];
                q = dcm2quat(mat);

                rotation.fromQuaternion(q);
                return true;
            }

            std::string toString()
            {
                char buff[1024];
                sprintf(buff,"%s -> %s \n tran: %f %f %f \n rot: %f %f %f %f \n\n", src_frame_id.c_str(), dst_frame_id.c_str(), translation.tX, translation.tY, translation.tZ, rotation.rX, rotation.rY, rotation.rZ, rotation.rW);
                return std::string(buff);
            }

        };
    }
}

#endif

