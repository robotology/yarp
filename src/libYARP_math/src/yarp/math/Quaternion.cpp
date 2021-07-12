/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/math/Quaternion.h>

#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/LogComponent.h>
#include <yarp/math/Math.h>
#include <cmath>
#include <cstdio>

using namespace yarp::math;

namespace {
YARP_LOG_COMPONENT(QUATERNION, "yarp.math.Quaternion")
}

YARP_BEGIN_PACK
class QuaternionPortContentHeader
{
public:
    yarp::os::NetInt32 listTag{0};
    yarp::os::NetInt32 listLen{0};
    QuaternionPortContentHeader() = default;
};
YARP_END_PACK

Quaternion::Quaternion()
{
    internal_data[0] = 1;
    internal_data[1] = 0;
    internal_data[2] = 0;
    internal_data[3] = 0;
}

Quaternion::Quaternion(double x, double y, double z, double w)
{
    internal_data[0] = w;
    internal_data[1] = x;
    internal_data[2] = y;
    internal_data[3] = z;
}

const double* Quaternion::data() const
{
    return internal_data;
}

double* Quaternion::data()
{
    return internal_data;
}

yarp::sig::Vector Quaternion::toVector()  const
{
    yarp::sig::Vector v(4);
    v[0] = internal_data[0];
    v[1] = internal_data[1];
    v[2] = internal_data[2];
    v[3] = internal_data[3];
    return v;
}

double Quaternion::w() const
{
    return internal_data[0];
}

double Quaternion::x() const
{
    return internal_data[1];
}

double Quaternion::y() const
{
    return internal_data[2];
}

double Quaternion::z() const
{
    return internal_data[3];
}

double& Quaternion::w()
{
    return internal_data[0];
}

double& Quaternion::x()
{
    return internal_data[1];
}

double& Quaternion::y()
{
    return internal_data[2];
}

double& Quaternion::z()
{
    return internal_data[3];
}

bool Quaternion::read(yarp::os::ConnectionReader& connection)
{
    // auto-convert text mode interaction
    connection.convertTextMode();
    QuaternionPortContentHeader header;
    bool ok = connection.expectBlock((char*)&header, sizeof(header));
    if (!ok) return false;

    if (header.listLen == 4 &&  header.listTag == (BOTTLE_TAG_LIST | BOTTLE_TAG_FLOAT64))
    {
        this->internal_data[0] = connection.expectFloat64();
        this->internal_data[1] = connection.expectFloat64();
        this->internal_data[2] = connection.expectFloat64();
        this->internal_data[3] = connection.expectFloat64();
    }
    else
    {
        return false;
    }

    return !connection.isError();
}

bool Quaternion::write(yarp::os::ConnectionWriter& connection) const
{
    QuaternionPortContentHeader header;

    header.listTag = (BOTTLE_TAG_LIST | BOTTLE_TAG_FLOAT64);
    header.listLen = 4;

    connection.appendBlock((char*)&header, sizeof(header));

    connection.appendFloat64(this->internal_data[0]);
    connection.appendFloat64(this->internal_data[1]);
    connection.appendFloat64(this->internal_data[2]);
    connection.appendFloat64(this->internal_data[3]);

    // if someone is foolish enough to connect in text mode,
    // let them see something readable.
    connection.convertTextMode();

    return !connection.isError();
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

double Quaternion::abs()
{
    return sqrt(internal_data[0] * internal_data[0] +
                internal_data[1] * internal_data[1] +
                internal_data[2] * internal_data[2] +
                internal_data[3] * internal_data[3]);
}

void Quaternion::normalize()
{
    double length = abs();
    internal_data[0] /= length;
    internal_data[1] /= length;
    internal_data[2] /= length;
    internal_data[3] /= length;
    return;
}

double Quaternion::arg()
{
    return atan2(sqrt(internal_data[1] * internal_data[1] +
                      internal_data[2] * internal_data[2] +
                      internal_data[3] * internal_data[3]),
                 internal_data[0]);
}

Quaternion Quaternion::inverse() const
{
    //                     w                  x                 y                  z
    return Quaternion(internal_data[0], -internal_data[1], -internal_data[2], -internal_data[3]);
}
