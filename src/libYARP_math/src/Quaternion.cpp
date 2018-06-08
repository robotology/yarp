/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/sig/Quaternion.h>
#include <yarp/math/Math.h>
#include <cmath>
#include <cstdio>

using namespace yarp::sig;
using namespace yarp::math;
/*
void Quaternion::fromRotationMatrix(const yarp::sig::Matrix &R)
{
    if ((R.rows()<3) || (R.cols()<3))
    {
        yError("fromRotationMatrix() failed, matrix should be >= 3x3");
        yAssert(R.rows() >= 3 && R.cols() >= 3);
    }

    double tr = R(0, 0) + R(1, 1) + R(2, 2);

    if (tr>0.0)
    {
        double sqtrp1 = sqrt(tr + 1.0);
        double sqtrp12 = 2.0*sqtrp1;
        internal_data[0] = 0.5*sqtrp1;
        internal_data[1] = (R(2, 1) - R(1, 2)) / sqtrp12;
        internal_data[2] = (R(0, 2) - R(2, 0)) / sqtrp12;
        internal_data[3] = (R(1, 0) - R(0, 1)) / sqtrp12;
    }
    else if ((R(1, 1)>R(0, 0)) && (R(1, 1)>R(2, 2)))
    {
        double sqdip1 = sqrt(R(1, 1) - R(0, 0) - R(2, 2) + 1.0);
        internal_data[2] = 0.5*sqdip1;

        if (sqdip1>0.0)
            sqdip1 = 0.5 / sqdip1;

        internal_data[0] = (R(0, 2) - R(2, 0))*sqdip1;
        internal_data[1] = (R(1, 0) + R(0, 1))*sqdip1;
        internal_data[3] = (R(2, 1) + R(1, 2))*sqdip1;
    }
    else if (R(2, 2)>R(0, 0))
    {
        double sqdip1 = sqrt(R(2, 2) - R(0, 0) - R(1, 1) + 1.0);
        internal_data[3] = 0.5*sqdip1;

        if (sqdip1>0.0)
            sqdip1 = 0.5 / sqdip1;

        internal_data[0] = (R(1, 0) - R(0, 1))*sqdip1;
        internal_data[1] = (R(0, 2) + R(2, 0))*sqdip1;
        internal_data[2] = (R(2, 1) + R(1, 2))*sqdip1;
    }
    else
    {
        double sqdip1 = sqrt(R(0, 0) - R(1, 1) - R(2, 2) + 1.0);
        internal_data[1] = 0.5*sqdip1;

        if (sqdip1>0.0)
            sqdip1 = 0.5 / sqdip1;

        internal_data[0] = (R(2, 1) - R(1, 2))*sqdip1;
        internal_data[2] = (R(1, 0) + R(0, 1))*sqdip1;
        internal_data[3] = (R(0, 2) + R(2, 0))*sqdip1;
    }
}*/

yarp::sig::Matrix Quaternion::toRotationMatrix4x4() const
{
    yarp::sig::Vector q = this->toVector();
    yarp::sig::Vector qin = (1.0 / yarp::math::norm(q))*q;

    yarp::sig::Matrix R = yarp::math::eye(4, 4);
    R(0, 0) = qin[0] * qin[0] + qin[1] * qin[1] - qin[2] * qin[2] - qin[3] * qin[3];
    R(1, 0) = 2.0*(qin[1] * qin[2] + qin[0] * qin[3]);
    R(2, 0) = 2.0*(qin[1] * qin[3] - qin[0] * qin[2]);
    R(0, 1) = 2.0*(qin[1] * qin[2] - qin[0] * qin[3]);
    R(1, 1) = qin[0] * qin[0] - qin[1] * qin[1] + qin[2] * qin[2] - qin[3] * qin[3];
    R(2, 1) = 2.0*(qin[2] * qin[3] + qin[0] * qin[1]);
    R(0, 2) = 2.0*(qin[1] * qin[3] + qin[0] * qin[2]);
    R(1, 2) = 2.0*(qin[2] * qin[3] - qin[0] * qin[1]);
    R(2, 2) = qin[0] * qin[0] - qin[1] * qin[1] - qin[2] * qin[2] + qin[3] * qin[3];

    return R;
}

yarp::sig::Matrix Quaternion::toRotationMatrix3x3() const
{
    yarp::sig::Vector q = this->toVector();
    yarp::sig::Vector qin = (1.0 / yarp::math::norm(q))*q;

    yarp::sig::Matrix R = yarp::math::zeros(3,3);
    R(0, 0) = qin[0] * qin[0] + qin[1] * qin[1] - qin[2] * qin[2] - qin[3] * qin[3];
    R(1, 0) = 2.0*(qin[1] * qin[2] + qin[0] * qin[3]);
    R(2, 0) = 2.0*(qin[1] * qin[3] - qin[0] * qin[2]);
    R(0, 1) = 2.0*(qin[1] * qin[2] - qin[0] * qin[3]);
    R(1, 1) = qin[0] * qin[0] - qin[1] * qin[1] + qin[2] * qin[2] - qin[3] * qin[3];
    R(2, 1) = 2.0*(qin[2] * qin[3] + qin[0] * qin[1]);
    R(0, 2) = 2.0*(qin[1] * qin[3] + qin[0] * qin[2]);
    R(1, 2) = 2.0*(qin[2] * qin[3] - qin[0] * qin[1]);
    R(2, 2) = qin[0] * qin[0] - qin[1] * qin[1] - qin[2] * qin[2] + qin[3] * qin[3];

    return R;
}

std::string Quaternion::toString(int precision, int width)
{
    std::string ret = "";
    char tmp[350];
    if (width<0)
    {
        sprintf(tmp, "w=% .*lf\t", precision, internal_data[0]);   ret += tmp;
        sprintf(tmp, "x=% .*lf\t", precision, internal_data[1]);   ret += tmp;
        sprintf(tmp, "y=% .*lf\t", precision, internal_data[2]);   ret += tmp;
        sprintf(tmp, "z=% .*lf\t", precision, internal_data[3]);   ret += tmp;
    }
    else
    {
        sprintf(tmp, "w=% *.*lf ", width, precision, internal_data[0]);    ret += tmp;
        sprintf(tmp, "x=% *.*lf ", width, precision, internal_data[1]);    ret += tmp;
        sprintf(tmp, "y=% *.*lf ", width, precision, internal_data[2]);    ret += tmp;
        sprintf(tmp, "z=% *.*lf ", width, precision, internal_data[3]);    ret += tmp;
    }

    return ret.substr(0, ret.length() - 1);
}

void Quaternion::fromAxisAngle(const yarp::sig::Vector &v)
{
    yarp::sig::Matrix m = axis2dcm(v);
    Quaternion q;
    q.fromRotationMatrix(m);
    this->internal_data[0] = q.internal_data[0];
    this->internal_data[1] = q.internal_data[1];
    this->internal_data[2] = q.internal_data[2];
    this->internal_data[3] = q.internal_data[3];
}

void Quaternion::fromAxisAngle(const yarp::sig::Vector& axis, const double& angle)
{
    yarp::sig::Vector v = axis;
    v.resize(4); v[4] = angle;
    yarp::sig::Matrix m = axis2dcm(v);
    Quaternion q;
    q.fromRotationMatrix(m);
    this->internal_data[0] = q.internal_data[0];
    this->internal_data[1] = q.internal_data[1];
    this->internal_data[2] = q.internal_data[2];
    this->internal_data[3] = q.internal_data[3];
}

yarp::sig::Vector Quaternion::toAxisAngle()
{
    yarp::sig::Matrix m=this->toRotationMatrix4x4();
    yarp::sig::Vector v = dcm2axis(m);
    return v;
}

