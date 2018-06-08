/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/sig/Quaternion.h>
#include <cmath>
#include <cstdio>

using namespace yarp::sig;

YARP_BEGIN_PACK
class QuaternionPortContentHeader
{
public:
    yarp::os::NetInt32 listTag;
    yarp::os::NetInt32 listLen;
    QuaternionPortContentHeader() : listTag(0), listLen(0) {}
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

Quaternion::Quaternion(const Quaternion &l)
{
    internal_data[0] = l.w();
    internal_data[1] = l.x();
    internal_data[2] = l.y();
    internal_data[3] = l.z();
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

bool Quaternion::write(yarp::os::ConnectionWriter& connection)
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
