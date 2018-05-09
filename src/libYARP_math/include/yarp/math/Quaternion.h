/*
* Author: Marco Randazzo, Silvio Traversaro
* Copyright (C) 2016 Istituto Italiano di Tecnologia (IIT)
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

#ifndef YARP_QUATERNION
#define YARP_QUATERNION

#include <yarp/math/api.h>
#include <yarp/sig/Vector.h>
#include <yarp/sig/Matrix.h>
#include <yarp/os/Portable.h>

// network stuff
#include <yarp/os/NetInt32.h>

namespace yarp {
    namespace math {
        class Quaternion;
    }
}

class YARP_math_API yarp::math::Quaternion : public yarp::os::Portable
{
    double internal_data[4]; // stored as [w x y z]

public:
    Quaternion();
    Quaternion(const Quaternion &l);
    Quaternion(double x, double y, double z, double w);
    double* data();
    const double* data() const;
    double x() const;
    double y() const;
    double z() const;
    double w() const;
    double& x() ;
    double& y() ;
    double& z() ;
    double& w() ;

    std::string toString(int precision = -1, int width = -1);

    /**
    * Computes the modulus of the quaternion.
    */
    double abs();

    void normalize();

    /**
    * Computes the argument or phase of the quaternion in radians.
    */
    double arg();

    void fromAxisAngle(const yarp::sig::Vector &v);
    yarp::sig::Vector toAxisAngle();

    /**
    * Converts a rotation matrix to a quaternion.
    *
    * The returned quaternion is ordered in the following way:
    * - s = q_0 \in \mathbb{R} the real part of the quaternion
    * - r = \f$ \begin{bmatrix} q_1 \\ q_2 \\ q_3 \end{bmatrix} \in \mathbb{R}^3 \f$ the imaginary part of the quaternion
    *
    * The input rotation matrix and the output quaternion are related by the following formula:
    * \f[
    *   R(s,r) = I_{3\times3} + 2s r^{\wedge} + 2{r^\wedge}^2,
    * \f]
    * where \f$ r^{\wedge} \f$ is the skew-symmetric matrix such that:
    * \f[
    *   r \times v = r^\wedge v
    * \f]
    *
    * \note This method is compatible with the rotation-quaternion convention used in the ROS tf2 library.
    *
    * @param R the input rotation matrix.
    * @return 4 by 1 vector for the quaternion representation in the
    *         form \f[ \mathbf{q}=q_0 + i \cdot q_1 + j \cdot q_2 +
    *         k \cdot q_3 \f]
    *
    */
    void fromRotationMatrix(const yarp::sig::Matrix &R);

    /**
    * Converts a quaternion to a rotation matrix.
    *
    * The quaternion is expected to be ordered in the following way:
    * - s = q_0 \in \mathbb{R} the real part of the quaterion
    * - r = \begin{bmatrix} q_1 \\ q_2 \\ q_3 \end{bmatrix} \in \mathbb{R}^3 the imaginary part of the quaternion
    *
    * The returned rotation matrix is given by the following formula:
    * \f[
    *   R(s,r) = I_{3\times3} + 2s r^{\wedge} + 2{r^\wedge}^2,
    * \f]
    * where \f$ r^{\wedge} \f$ is the skew-symmetric matrix such that:
    * \f[
    *   r \times v = r^\wedge v
    * \f]
    *
    * \note This method is compatible with the rotation-quaternion convention used in the ROS tf2 library.
    *
    * @param q the 4 by 1 input quaternion in the form \f[
    *         \mathbf{q}=q_0 + i \cdot q_1 + j \cdot q_2 + k \cdot
    *                q_3 \f]
    * @return 4 by 4 homogeneous matrix representing with the
    *         rotation components in the top left 3 by 3 submatrix.
    *
    */
    yarp::sig::Matrix toRotationMatrix() const;


    yarp::sig::Vector toVector() const;

    ///////// Serialization methods
    /*
    * Read vector from a connection.
    * return true iff a vector was read correctly
    */
    virtual bool read(yarp::os::ConnectionReader& connection) override;

    /**
    * Write vector to a connection.
    * return true iff a vector was written correctly
    */
    virtual bool write(yarp::os::ConnectionWriter& connection) override;

    virtual yarp::os::Type getType() override
    {
        return yarp::os::Type::byName("yarp/quaternion");
    }
};

#endif
