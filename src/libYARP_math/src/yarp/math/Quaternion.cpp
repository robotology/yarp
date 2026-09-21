/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/math/Quaternion.h>

#include <yarp/os/LogComponent.h>
#include <yarp/math/Math.h>
#include <cmath>
#include <cstdio>

using namespace yarp::math;

namespace {
YARP_LOG_COMPONENT(QUATERNION, "yarp.math.Quaternion")
}

Quaternion::Quaternion()
{
    q_w = 1;
    q_x = 0;
    q_y = 0;
    q_z = 0;
}

Quaternion::Quaternion(double _x, double _y, double _z, double _w)
{
    q_w = _w;
    q_x = _x;
    q_y = _y;
    q_z = _z;
}

bool Quaternion::isValid() const
{
    if (q_w == 0 &&
        q_x == 0 &&
        q_y == 0 &&
        q_z == 0) {return false;}
    return true;
}

yarp::sig::Vector Quaternion::toVector()  const
{
    yarp::sig::Vector v(4);
    v[0] = q_w;
    v[1] = q_x;
    v[2] = q_y;
    v[3] = q_z;
    return v;
}

double Quaternion::w() const
{
    return q_w;
}

double Quaternion::x() const
{
    return q_x;
}

double Quaternion::y() const
{
    return q_y;
}

double Quaternion::z() const
{
    return q_z;
}

double& Quaternion::w()
{
    return q_w;
}

double& Quaternion::x()
{
    return q_x;
}

double& Quaternion::y()
{
    return q_y;
}

double& Quaternion::z()
{
    return q_z;
}

void Quaternion::fromRotationMatrix(const yarp::sig::Matrix &R)
{
    if ((R.rows()<3) || (R.cols()<3))
    {
        yCError(QUATERNION, "fromRotationMatrix() failed, matrix should be >= 3x3");
        yCAssert(QUATERNION, R.rows() >= 3 && R.cols() >= 3);
    }

    double tr = R(0, 0) + R(1, 1) + R(2, 2);

    if (tr>0.0)
    {
        double sqtrp1 = sqrt(tr + 1.0);
        double sqtrp12 = 2.0*sqtrp1;
        q_w = 0.5*sqtrp1;
        q_x = (R(2, 1) - R(1, 2)) / sqtrp12;
        q_y = (R(0, 2) - R(2, 0)) / sqtrp12;
        q_z = (R(1, 0) - R(0, 1)) / sqtrp12;
    }
    else if ((R(1, 1)>R(0, 0)) && (R(1, 1)>R(2, 2)))
    {
        double sqdip1 = sqrt(R(1, 1) - R(0, 0) - R(2, 2) + 1.0);
        q_y = 0.5*sqdip1;

        if (sqdip1 > 0.0) {
            sqdip1 = 0.5 / sqdip1;
        }

        q_w = (R(0, 2) - R(2, 0))*sqdip1;
        q_x = (R(1, 0) + R(0, 1))*sqdip1;
        q_z = (R(2, 1) + R(1, 2))*sqdip1;
    }
    else if (R(2, 2)>R(0, 0))
    {
        double sqdip1 = sqrt(R(2, 2) - R(0, 0) - R(1, 1) + 1.0);
        q_z = 0.5*sqdip1;

        if (sqdip1 > 0.0) {
            sqdip1 = 0.5 / sqdip1;
        }

        q_w = (R(1, 0) - R(0, 1))*sqdip1;
        q_x = (R(0, 2) + R(2, 0))*sqdip1;
        q_y = (R(2, 1) + R(1, 2))*sqdip1;
    }
    else
    {
        double sqdip1 = sqrt(R(0, 0) - R(1, 1) - R(2, 2) + 1.0);
        q_x = 0.5*sqdip1;

        if (sqdip1 > 0.0) {
            sqdip1 = 0.5 / sqdip1;
        }

        q_w = (R(2, 1) - R(1, 2))*sqdip1;
        q_y = (R(1, 0) + R(0, 1))*sqdip1;
        q_z = (R(0, 2) + R(2, 0))*sqdip1;
    }
}

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

std::string Quaternion::toString(int precision, int width) const
{
    std::string ret;
    char tmp[350];
    if (width<0)
    {
        sprintf(tmp, "w=% .*lf\t", precision, q_w);   ret += tmp;
        sprintf(tmp, "x=% .*lf\t", precision, q_x);   ret += tmp;
        sprintf(tmp, "y=% .*lf\t", precision, q_y);   ret += tmp;
        sprintf(tmp, "z=% .*lf\t", precision, q_z);   ret += tmp;
    }
    else
    {
        sprintf(tmp, "w=% *.*lf ", width, precision, q_w);    ret += tmp;
        sprintf(tmp, "x=% *.*lf ", width, precision, q_x);    ret += tmp;
        sprintf(tmp, "y=% *.*lf ", width, precision, q_y);    ret += tmp;
        sprintf(tmp, "z=% *.*lf ", width, precision, q_z);    ret += tmp;
    }

    return ret.substr(0, ret.length() - 1);
}

void Quaternion::fromAxisAngle(const yarp::sig::Vector &v)
{
    yarp::sig::Matrix m = axis2dcm(v);
    Quaternion q;
    q.fromRotationMatrix(m);
    this->q_w = q.q_w;
    this->q_x = q.q_x;
    this->q_y = q.q_y;
    this->q_z = q.q_z;
}

void Quaternion::fromAxisAngle(const yarp::sig::Vector& axis, const double& angle)
{
    yarp::sig::Vector v = axis;
    v.resize(4); v[4] = angle;
    yarp::sig::Matrix m = axis2dcm(v);
    Quaternion q;
    q.fromRotationMatrix(m);
    this->q_w = q.q_w;
    this->q_x = q.q_x;
    this->q_y = q.q_y;
    this->q_z = q.q_z;
}

yarp::sig::Vector Quaternion::toAxisAngle()
{
    yarp::sig::Matrix m=this->toRotationMatrix4x4();
    yarp::sig::Vector v = dcm2axis(m);
    return v;
}

double Quaternion::abs()
{
    return sqrt(q_w * q_w +
                q_x * q_x +
                q_y * q_y +
                q_z * q_z);
}

void Quaternion::normalize()
{
    double length = abs();
    q_w /= length;
    q_x /= length;
    q_y /= length;
    q_z /= length;
    return;
}

double Quaternion::arg()
{
    return atan2(sqrt(q_x * q_x +
                      q_y * q_y +
                      q_z * q_z),  q_w);
}

Quaternion Quaternion::inverse() const
{
    return Quaternion(q_w, -q_x, -q_y, -q_z);
}
