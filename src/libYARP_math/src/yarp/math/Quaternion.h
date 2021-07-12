/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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

    std::string toString(int precision = -1, int width = -1) const;

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

#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0
    /**
     * Converts a quaternion to a rotation matrix.
     *
     * @deprecated since YARP 3.0.0. Use toRotationMatrix4x4 instead.
     */
    YARP_DEPRECATED_MSG("Use toRotationMatrix4x4 instead")
    yarp::sig::Matrix toRotationMatrix() const { return toRotationMatrix4x4(); }
#endif

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
    bool read(yarp::os::ConnectionReader& connection) override;

    /**
    * Write vector to a connection.
    * return true iff a vector was written correctly
    */
    bool write(yarp::os::ConnectionWriter& connection) const override;

    yarp::os::Type getType() const override
    {
        return yarp::os::Type::byName("yarp/quaternion");
    }


};

#endif
