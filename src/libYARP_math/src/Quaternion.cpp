/*
* Author: Marco Randazzo, Silvio Traversaro
* Copyright (C) 2016 iCubFacility - Istituto Italiano di Tecnologia
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

#include <yarp/math/Quaternion.h>
#include <yarp/math/Math.h>

using namespace yarp::math;

YARP_BEGIN_PACK
class QuaternionPortContentHeader
{
public:
    yarp::os::NetInt32 listTag;
    yarp::os::NetInt32 listLen;
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

yarp::sig::Vector Quaternion::toVector()
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

    if (header.listLen == 4 &&  header.listTag == (BOTTLE_TAG_LIST | BOTTLE_TAG_DOUBLE))
    {
        this->internal_data[0] = connection.expectDouble();
        this->internal_data[1] = connection.expectDouble();
        this->internal_data[2] = connection.expectDouble();
        this->internal_data[3] = connection.expectDouble();
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

    header.listTag = (BOTTLE_TAG_LIST | BOTTLE_TAG_DOUBLE);
    header.listLen = 4;

    connection.appendBlock((char*)&header, sizeof(header));

    connection.appendDouble(this->internal_data[0]);
    connection.appendDouble(this->internal_data[1]);
    connection.appendDouble(this->internal_data[2]);
    connection.appendDouble(this->internal_data[3]);

    // if someone is foolish enough to connect in text mode,
    // let them see something readable.
    connection.convertTextMode();

    return !connection.isError();
}

void Quaternion::fromRotationMatrix(const yarp::sig::Matrix &R)
{

    if ((R.rows()<3) || (R.cols()<3))
    {
        yError("fromRotationMatrix() failed, matrix is not 3x3");
        yAssert(R.rows() == 3 && R.cols() == 3);
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

yarp::sig::Matrix Quaternion::toRotationMatrix()
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

double Quaternion::abs()
{
    return sqrt(internal_data[0] * internal_data[0] +
                internal_data[1] * internal_data[1] +
                internal_data[2] * internal_data[2] +
                internal_data[3] * internal_data[3]);
}

double Quaternion::arg()
{
    return atan2(sqrt(internal_data[1] * internal_data[1] +
                      internal_data[2] * internal_data[2] +
                      internal_data[3] * internal_data[3]),
                 internal_data[0]);
}
