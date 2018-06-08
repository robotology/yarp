/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARP_SIG_QUATERNION
#define YARP_SIG_QUATERNION

#include <yarp/sig/api.h>
#include <yarp/sig/Vector.h>
#include <yarp/sig/Matrix.h>
#include <yarp/os/Portable.h>

// network stuff
#include <yarp/os/NetInt32.h>

namespace yarp {
    namespace sig {
        class Quaternion;
    }
}

class YARP_sig_API yarp::sig::Quaternion : public yarp::os::Portable
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

    /**
    * Normalizes the quaternion elements.
    */
    void normalize();

    /**
    * Computes the inverse of the quaternion.
    */
    Quaternion inverse() const;

    /**
    * Computes the argument or phase of the quaternion in radians.
    */
    double arg();

    /**
    * Computes the quaternion from an axis-angle representation
    * @param v a 4D vector, where the first three elements represent the axis, while the fourth element represents the angle (in radians)
    */
    void fromAxisAngle(const yarp::sig::Vector &v);

    /**
    * Computes the quaternion from an axis-angle representation
    * @param axis a 3D vector representing the axis.
    * @angle the rotation angle (in radians)
    */
    void fromAxisAngle(const yarp::sig::Vector& axis, const double& angle);


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
    * - s = q_0 \in \mathbb{R} the real part of the quaternion
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
    yarp::sig::Matrix toRotationMatrix4x4() const;

    /**
    * Converts a quaternion to a rotation matrix.
    * @param q the quaternion
    * @return the corresponding 3 by 3 rotation matrix
    */
    yarp::sig::Matrix toRotationMatrix3x3() const;

    /**
    * Converts the quaternion to a vector of length 4.
    */
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

    virtual yarp::os::Type getType() const override
    {
        return yarp::os::Type::byName("yarp/quaternion");
    }


};

#endif
