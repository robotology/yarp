
// by Arjan Gijsberts

#include <iostream>

#include <time.h>

#include <yarp/sig/Vector.h>
#include <yarp/sig/Matrix.h>
#include <yarp/math/Math.h>
#include <yarp/math/Rand.h>

static double _time() {
    struct timespec time;
    clock_gettime(CLOCK_REALTIME, &time);
    return (double) (time.tv_sec + ((double) time.tv_nsec / 1.0E9));
}


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
    std::cout << "YARP Atlas Blas test" << std::endl;
    int times = 1;
    int vec_size = 1000000;
    int mat_size = 1000;
    double t1, t2;

    yarp::sig::Vector vec = yarp::sig::Vector(vec_size);
    yarp::sig::Matrix mat = yarp::sig::Matrix(mat_size, mat_size);
    yarp::sig::Vector matvec = yarp::sig::Vector(mat_size);

    t1 = _time();
    fillVector(vec);
    t2 = _time();
    std::cout << "Filling random vector(" << vec.size() << "): " << (t2 - t1) << std::endl;

    t1 = _time();
    fillMatrix(mat);
    t2 = _time();
    std::cout << "Filling random matrix(" << mat.rows() << "," << mat.cols() << "): " << (t2 - t1) << std::endl;
    fillVector(matvec);

    t1 = _time();
    for(int i = 0; i < times; i++) {
        double t = yarp::math::dot(vec, vec);
    }
    t2 = _time();
    std::cout << "Vector dot product: " << (t2 - t1) << std::endl;

    t1 = _time();
    for(int i = 0; i < times; i++) {
        yarp::sig::Vector t = yarp::math::operator*(mat, matvec);
    }
    t2 = _time();
    std::cout << "Matrix/Vector product: " << (t2 - t1) << std::endl;

    t1 = _time();
    for(int i = 0; i < times; i++) {
        yarp::sig::Matrix t = yarp::math::operator*(mat, mat);
    }
    t2 = _time();
    std::cout << "Matrix/Matrix product: " << (t2 - t1) << std::endl;

    t1 = _time();
    for(int i = 0; i < times; i++) {
        yarp::sig::Matrix t = yarp::math::luinv(mat);
    }
    t2 = _time();
    std::cout << "Matrix inverse: " << (t2 - t1) << std::endl;

}
