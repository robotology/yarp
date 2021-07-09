/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <iostream>

#include <yarp/sig/Vector.h>
#include <yarp/sig/Matrix.h>
#include <yarp/math/Math.h>
#include <yarp/math/Rand.h>
#include <yarp/os/Time.h>

void fillMatrix(yarp::sig::Matrix& mat) {
    for(int r = 0; r < mat.rows(); r++) {
        for(int c = 0; c < r; c++) {
            mat(r, c) = mat(c, r) = yarp::math::Rand::scalar(-10, 10);
            if(r == c) mat(r, c) += 1;
        }
    }
}

void fillVector(yarp::sig::Vector& vec) {
    for(int i = 0; i < vec.size(); i++) {
        vec(i) = yarp::math::Rand::scalar(-10, 10);
    }
}

int main(int argc, char** argv) {
    std::cout << "YARP math test" << std::endl;

    int vec_times = 1000;
    int mat_times = 1;
    int vec_size = 1000000;
    int mat_size = 1000;
    double t1, t2;

    yarp::sig::Vector vec = yarp::sig::Vector(vec_size);
    yarp::sig::Matrix mat = yarp::sig::Matrix(mat_size, mat_size);
    yarp::sig::Vector matvec = yarp::sig::Vector(mat_size);

    t1 = yarp::os::Time::now();
    fillVector(vec);
    t2 = yarp::os::Time::now();
    std::cout << "Filling random vector(" << vec.size() << "): " << (t2 - t1) << std::endl;

    t1 = yarp::os::Time::now();
    fillMatrix(mat);
    t2 = yarp::os::Time::now();
    std::cout << "Filling random matrix(" << mat.rows() << "," << mat.cols() << "): " << (t2 - t1) << std::endl;
    fillVector(matvec);

    t1 = yarp::os::Time::now();
    for(int i = 0; i < vec_times; i++) {
        double t = yarp::math::dot(vec, vec);
    }
    t2 = yarp::os::Time::now();
    std::cout << "Vector dot product: " << (t2 - t1) << std::endl;

    t1 = yarp::os::Time::now();
    for(int i = 0; i < vec_times; i++) {
        yarp::sig::Vector t = yarp::math::operator*(mat, matvec);
    }
    t2 = yarp::os::Time::now();
    std::cout << "Matrix/Vector product: " << (t2 - t1) << std::endl;

    t1 = yarp::os::Time::now();
    for(int i = 0; i < mat_times; i++) {
        yarp::sig::Matrix t = yarp::math::operator*(mat, mat);
    }
    t2 = yarp::os::Time::now();
    std::cout << "Matrix/Matrix product: " << (t2 - t1) << std::endl;

    t1 = yarp::os::Time::now();
    for(int i = 0; i < mat_times; i++) {
        yarp::sig::Matrix t = yarp::math::luinv(mat);
    }
    t2 = yarp::os::Time::now();
    std::cout << "Matrix inverse: " << (t2 - t1) << std::endl;

}
