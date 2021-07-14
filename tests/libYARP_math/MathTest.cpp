/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define _USE_MATH_DEFINES

#include <yarp/math/Math.h>
#include <yarp/sig/Vector.h>
#include <yarp/math/Rand.h>
#include <yarp/math/SVD.h>
#include <yarp/os/Time.h>

#include <cmath>
#include <string>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::os::impl;
using namespace yarp::sig;
using namespace yarp::math;
using namespace std;

// Assert that 2 vectors are equal
void checkEqual(const Vector &a, const Vector &b)
{
    CHECK(a.size() == b.size());
    for(size_t i = 0; i<a.size(); i++){
        CHECK(a[i] == Approx(b[i]).margin(1e-8));
    }
}

// Assert that a vector (1 dim) is equal to a scalar
void checkEqual(const Vector &a, const double &b)
{
    checkEqual(a, Vector(1, b));
}

void checkEqual(const double &b, const Vector &a)
{
    checkEqual(a, Vector(1, b));
}

// Assert that 2 matrices are equal
void checkEqual(const Matrix &A, const Matrix &B)
{
    CHECK(A.cols() == B.cols());
    CHECK(A.rows() == B.rows());
    for(size_t r = 0; r<A.rows(); r++){
        for(size_t c = 0; c<A.cols(); c++){
            CHECK(A(r, c) == Approx(B(r, c)).margin(1e-8));
        }
    }
}

// Assert that a matrix and a vector are equal (the matrix has to have either one row or one column)
void checkEqual(const Matrix &A, const Vector &b)
{
    CHECK(((A.cols() == 1) || (A.rows() == 1)));
    if(A.cols() == 1){
        Vector a = A.getCol(0);
        checkEqual(a, b);
    } else if (A.rows() == 1) {
        checkEqual(A.getRow(0), b);
    }
}

void checkEqual(const Vector &b, const Matrix &A)
{
    checkEqual(A, b);
}

void checkEqual(const double &a, const double &b)
{
    CHECK(a == Approx(b).margin(1e-8));
}

#define CHECK_EQUAL(a, b) \
{ \
    INFO("CHECK_EQUAL( " << #a << ", " << #b << " ) at " << __FILE__ << ":" << __LINE__); \
    checkEqual(a, b); \
}

TEST_CASE("math::MathTest", "[yarp::math]")
{

    SECTION("check matrix misc operations")
    {
        Matrix mm(4, 4);
        mm.zero();
        mm = eye(5, 5);
        mm = eye(2, 5);
        mm = eye(5, 2);
        Matrix mt = mm.transposed();

        int n = 3, m = 2;
        Vector rangeMin(n, -100);
        Vector rangeMax(n, 100);
        double s = Rand::scalar(rangeMin[0], rangeMax[0]); // scalar
        double s2 = Rand::scalar(rangeMin[0], rangeMax[0]); // scalar
        Vector a1 = Rand::vector(rangeMin.subVector(0, 0), rangeMax.subVector(0, 0)); // 1 dim vector
        Vector b1 = Rand::vector(rangeMin.subVector(0, 0), rangeMax.subVector(0, 0)); // 1 dim vector
        Vector c1 = Rand::vector(rangeMin.subVector(0, 0), rangeMax.subVector(0, 0)); // 1 dim vector
        Vector an = Rand::vector(rangeMin, rangeMax); // n dim vector
        Vector bn = Rand::vector(rangeMin, rangeMax); // n dim vector
        Vector cn = Rand::vector(rangeMin, rangeMax); // n dim vector
        Matrix An = Rand::matrix(n, 1), Bn = Rand::matrix(n, 1);
        Matrix Ann = Rand::matrix(n, n), Amn = Rand::matrix(m, n), Bmn = Rand::matrix(m, n), Cmn = Rand::matrix(m, n);
        INFO("(x y z are scalar; a b c are vectors; A B C are matrices)");
        CHECK_EQUAL(an*(s+s2), an*s + an*s2); // a(x+y) = ax+ay;
        CHECK_EQUAL(an*(bn+cn), an*bn + an*cn); // a(b+c) = ab+ac;
        CHECK_EQUAL(an*(bn+s), an*bn + an*s); // a(b+x) = ab+ax;
        CHECK_EQUAL(an*(s+bn), an*s + an*bn); // a(x+b) = ax+ab;
        CHECK_EQUAL(Amn.transposed()*(Bmn+Cmn), Amn.transposed()*Bmn + Amn.transposed()*Cmn); // A(B+C) = AB+AC;
    }

    SECTION("check vector operators.")
    {
        Vector a(3);
        Vector b(3);
        Vector c;
        a = 1;
        b = 1;

        //test
        c = a+b;

        double acc = c[0]+c[1]+c[2];
        CHECK(acc == 6); // operator+ on vectors

        c = a-b;
        acc = c[0]+c[1]+c[2];
        CHECK(acc == 0); // operator- on vectors

        a = 2;
        b = 2;
        acc = dot(a, b);
        CHECK(acc == 12); // dot product on vectors

        //scalar mult
        a = 1;
        Vector m1 = a*3;
        Vector m2 = 3*a;

        acc = m1[0]+m1[1]+m1[2];
        CHECK(acc == 9); // element wise operator*
        acc = m2[0]+m2[1]+m2[2];
        CHECK(acc == 9); // element wise operator*

        //scalar division
        a = 9.0;
        m1 = a/3.0;

        acc = m1[0]+m1[1]+m1[2];
        CHECK(acc == 9); // element wise operator/
    }

    SECTION("check matrix and vector operations.")
    {

        Matrix m(3, 2);
        m = eye(3, 2);
        Vector v1(3);
        v1 = 1;
        Vector v2(2);
        v2 = 1;
        Vector a1;
        Vector a2;
        Vector ret1(2);
        Vector ret2(3);
        ret1(0) = 1;
        ret1(1) = 1;

        ret2(0) = 1;
        ret2(1) = 1;
        ret2(2) = 0;

        a1 = v1*m;
        CHECK(ret1 == a1); // Vector by Matrix multiplication
        a2 = m*v2;
        CHECK(ret2 == a2); // Matrix by Vector multiplication
    }

    SECTION("check matrix operations.")
    {
        Matrix A(3, 4);
        Matrix B(4, 2);
        A = 2;
        B = 3;
        Matrix C = A*B;

        Matrix exp(3, 2);
        exp = 24; //expected result

        CHECK(exp == C); // Matrix::operator*

        A = 9; //initialize all with 9
        Matrix res;
        res = A/3.0; //divide all by 3
        //sum elements
        double acc = 0.0;
        for (size_t r = 0; r < res.rows(); r++) {
            for (size_t c = 0; c < res.cols(); c++) {
                acc += res[r][c];
            }
        }

        double expected = res.rows()*res.cols()*3.0;

        CHECK(acc == expected); // Matrix element wise division

        A = 3.0; //initialize all with 3
        res = A*3.0; // multiply all by 3
        acc = 0.0;
        for (size_t r = 0; r < res.rows(); r++) {
            for(size_t c = 0; c<res.cols(); c++)
            {
                //INFO(res[r][c]);
                acc += res[r][c];
            }
        }

        expected = res.rows()*res.cols()*9;

        CHECK(acc == expected); // Matrix element wise multiplication
    }

    SECTION("check SVD.")
    {

        Matrix M(6, 5);
        M = 1;
        Matrix U, V;
        Vector s;
        Matrix S;
        S.resize(5, 5);

        U.resize(6, 5);
        s.resize(5);
        V.resize(5, 5);

        SVD(M, U, s, V);

        S.diagonal(s);

        Matrix T(6, 5);
        T = U*S*V.transposed();

        INFO(M.toString());
        INFO(T.toString());
    }

    SECTION("checking matrix inversions.")
    {
        Vector v(0);
        Matrix A(4, 4);
        int counter = 1;
        for(size_t r = 0; r < A.rows(); r++) {
            for(size_t c = 0; c < A.cols(); c++) {
                A(r, c) = counter++ + (r == c ? 1 : 0);
            }
        }
        Matrix Ainv = luinv(A);
        Matrix I = A * Ainv;

        bool invGood = true;
        Matrix ref = eye(4, 4);
        for (size_t r = 0; r < I.rows(); r++) {
            for(size_t c = 0; c<I.cols(); c++)
            {
                if (fabs(I[r][c] - ref[r][c]) > 0.0001) {
                    invGood = false;
                }
            }
        }

        CHECK(invGood); // luinv

        //INFO("luinv: " << I.toString());

        /*  [ 2 1 0 0 ]^-1   [ 1 -1  1 -1 ]
         *  [ 1 2 1 0 ]      [-1  2 -2  2 ]
         *  [ 0 1 2 1 ]    = [ 1 -2  3 -3 ]
         *  [ 0 0 1 1 ]      [-1  2 -3  4 ]
         */
        /* See note in math.cpp implementation of chinv
        Matrix B = zeros(4, 4);
        B(0, 0) = B(1, 1) = B(2, 2) = 2;
        B(0, 1) = B(1, 0) = B(1, 2) = B(2, 1) = B(2, 3) = B(3, 2) = B(3, 3) = 1;
        Matrix Binv = chinv(B);
        I = B * Binv;
        INFO("chinv: " << I.toString());
        */

    }

    SECTION("checking matrix determinant.")
    {
        Matrix A(4, 4);
        A(0, 0) = 2;
        A(0, 1) = 3;
        A(0, 2) = 5;
        A(0, 3) = 7;

        A(1, 0) = 4;
        A(1, 1) = 7;
        A(1, 2) = 2;
        A(1, 3) = 9;

        A(2, 0) = 5;
        A(2, 1) = 5;
        A(2, 2) = 6;
        A(2, 3) = 8;

        A(3, 0) = 1;
        A(3, 1) = 1;
        A(3, 2) = 4;
        A(3, 3) = 7;

        double val = det(A);
        bool ok = ((val - -163) < 1e-10 && (-163 - val) < 1e-10);
        CHECK(ok); // Matrix determinant
        //INFO("det: " << val);
    }

    SECTION("checking sum and subtraction operators.")
    {
        int n = 3, m = 2;
        Vector rangeMin(n, -100);
        Vector rangeMax(n, 100);
        double s = Rand::scalar(rangeMin[0], rangeMax[0]); // scalar
        Vector a1 = Rand::vector(rangeMin.subVector(0, 0), rangeMax.subVector(0, 0)); // 1 dim vector
        Vector b1 = Rand::vector(rangeMin.subVector(0, 0), rangeMax.subVector(0, 0)); // 1 dim vector
        Vector c1 = Rand::vector(rangeMin.subVector(0, 0), rangeMax.subVector(0, 0)); // 1 dim vector
        Vector an = Rand::vector(rangeMin, rangeMax); // n dim vector
        Vector bn = Rand::vector(rangeMin, rangeMax); // n dim vector
        Vector cn = Rand::vector(rangeMin, rangeMax); // n dim vector
        Matrix An = Rand::matrix(n, 1), Bn = Rand::matrix(n, 1);
        Matrix Ann = Rand::matrix(n, n), Amn = Rand::matrix(m, n), Bmn = Rand::matrix(m, n), Cmn = Rand::matrix(m, n);
        CHECK_EQUAL(a1+s, a1[0]+s); // vector + scalar
        CHECK_EQUAL(a1-s, a1[0]-s); // vector - scalar
        CHECK_EQUAL((an+s)-s, an); // vector + scalar - scalar
        CHECK_EQUAL(s+a1, a1[0]+s); // scalar + vector
        CHECK_EQUAL(s+an-s, an); // scalar + vector - scalar
        CHECK_EQUAL(s-a1, s-a1[0]); // scalar - vector
        CHECK_EQUAL(an+s-an, Vector(n, s)); // vector + scalar - vector
        // vector += scalar
        b1 = a1;
        b1 += s;
        CHECK_EQUAL(b1, a1[0]+s);
        // vector -= scalar
        b1 = a1;
        b1 -= s;
        CHECK_EQUAL(b1, a1[0]-s);
        // vector + vector
        CHECK_EQUAL(a1+b1, a1[0]+b1[0]);
        CHECK_EQUAL(an+zeros(n), an);
        // vector - vector
        CHECK_EQUAL(a1-b1, a1[0]-b1[0]);
        CHECK_EQUAL(an-zeros(n), an);
        // vector += vector
        cn = bn;
        cn += an;
        CHECK_EQUAL(cn, bn+an);
        bn = an;
        bn += zeros(n);
        CHECK_EQUAL(bn, an);
        // vector -= vector
        cn = bn;
        cn -= an;
        CHECK_EQUAL(cn, bn-an);
        bn = an;
        bn -= zeros(n);
        CHECK_EQUAL(bn, an);
        // matrix + matrix
        CHECK_EQUAL(Amn+zeros(m, n), Amn);
        CHECK_EQUAL(An+Bn, An.getCol(0)+Bn.getCol(0));
        // matrix - matrix
        CHECK_EQUAL(Amn-zeros(m, n), Amn);
        CHECK_EQUAL(An-Bn, An.getCol(0)-Bn.getCol(0));
        // matrix += matrix
        Amn = Bmn;
        Amn += zeros(m, n);
        CHECK_EQUAL(Amn, Bmn);
        Amn = Bmn;
        Amn += Cmn;
        CHECK_EQUAL(Amn, Bmn+Cmn);
        // matrix -= matrix
        Amn = Bmn;
        Amn -= zeros(m, n);
        CHECK_EQUAL(Amn, Bmn);
        Amn = Bmn;
        Amn -= Cmn;
        CHECK_EQUAL(Amn, Bmn-Cmn);
    }

    SECTION("checking product operator.")
    {
        int n = 3, m = 2;
        Vector rangeMin(n, -100);
        Vector rangeMax(n, 100);
        double s = Rand::scalar(rangeMin[0], rangeMax[0]); // scalar
        Vector a1 = Rand::vector(rangeMin.subVector(0, 0), rangeMax.subVector(0, 0)); // 1 dim vector
        Vector b1 = Rand::vector(rangeMin.subVector(0, 0), rangeMax.subVector(0, 0)); // 1 dim vector
        Vector c1 = Rand::vector(rangeMin.subVector(0, 0), rangeMax.subVector(0, 0)); // 1 dim vector
        Vector an = Rand::vector(rangeMin, rangeMax); // n dim vector
        Vector bn = Rand::vector(rangeMin, rangeMax); // n dim vector
        Vector cn = Rand::vector(rangeMin, rangeMax); // n dim vector
        Matrix An = Rand::matrix(n, 1), Bn = Rand::matrix(n, 1);
        Matrix Ann = Rand::matrix(n, n), Amn = Rand::matrix(m, n), Bmn = Rand::matrix(m, n), Cmn = Rand::matrix(m, n);
        // scalar * vector
        CHECK_EQUAL(0*an, zeros(n));
        CHECK_EQUAL(1*an, an);
        // vector * scalar
        CHECK_EQUAL(an*0, zeros(n));
        CHECK_EQUAL(an*1, an);
        CHECK_EQUAL(an*s, s*an);
        // vector *= scalar
        an = bn;
        an *= s;
        CHECK_EQUAL(an, bn*s);
        // vector * vector
        CHECK_EQUAL(an*zeros(n), zeros(n));
        CHECK_EQUAL(an*ones(n), an);
        // vector *= vector
        an = bn;
        an *= cn;
        CHECK_EQUAL(an, bn*cn);
        // scalar * matrix
        CHECK_EQUAL(0*Amn, zeros(m, n));
        CHECK_EQUAL(1*Amn, Amn);
        // matrix * scalar
        CHECK_EQUAL(Amn*0, zeros(m, n));
        CHECK_EQUAL(Amn*1, Amn);
        CHECK_EQUAL(Amn*s, s*Amn);
        // matrix *= scalar
        Amn = Bmn;
        Amn *= s;
        CHECK_EQUAL(Amn, Bmn*s);
        // vector * matrix
        CHECK_EQUAL(zeros(m)*Amn, zeros(n));
        Vector e1m(m, 0.0);
        e1m[0] = 1;
        CHECK_EQUAL(e1m*Amn, Amn.getRow(0));
        // matrix * vector
        CHECK_EQUAL(Amn*zeros(n), zeros(m));
        Vector e1n(n, 0.0);
        e1n[0] = 1;
        CHECK_EQUAL(Amn*e1n, Amn.getCol(0));
        // vector *= matrix
        an = bn;
        an *= Ann;
        CHECK_EQUAL(an, bn*Ann);
        // matrix * matrix
        CHECK_EQUAL(Amn*zeros(n, n), zeros(m, n));
        CHECK_EQUAL(zeros(m, m)*Amn, zeros(m, n));
        CHECK_EQUAL(Amn*eye(n, n), Amn);
        CHECK_EQUAL(eye(m, m)*Amn, Amn);
        CHECK_EQUAL(Amn*Bmn.transposed(), (Bmn*Amn.transposed()).transposed());
        // matrix *= matrix
        Amn = Bmn;
        Amn *= Ann;
        CHECK_EQUAL(Amn, Bmn*Ann);
    }

    SECTION("check division operator.")
    {
        int n = 3, m = 2;
        Vector rangeMin(n, -100);
        Vector rangeMax(n, 100);
        double s = Rand::scalar(rangeMin[0], rangeMax[0]); // scalar
        Vector a1 = Rand::vector(rangeMin.subVector(0, 0), rangeMax.subVector(0, 0)); // 1 dim vector
        Vector b1 = Rand::vector(rangeMin.subVector(0, 0), rangeMax.subVector(0, 0)); // 1 dim vector
        Vector an = Rand::vector(rangeMin, rangeMax); // n dim vector
        Vector bn = Rand::vector(rangeMin, rangeMax); // n dim vector
        Vector cn = Rand::vector(rangeMin, rangeMax); // n dim vector
        Matrix Amn = Rand::matrix(m, n), Bmn = Rand::matrix(m, n);
         // vector / scalar
        CHECK_EQUAL(an/1, an);
        CHECK_EQUAL(a1/s, a1[0]/s);
        // vector /= scalar
        an = bn;
        an /= s;
        CHECK_EQUAL(an, bn/s);
        // vector / vector
        CHECK_EQUAL(an/ones(n), an);
        CHECK_EQUAL(a1/b1, a1[0]/b1[0]);
        // vector /= vector
        an = bn;
        an /= cn;
        CHECK_EQUAL(an, bn/cn);
        // matrix / scalar
        CHECK_EQUAL(Amn/1, Amn);
        CHECK((Amn/s)(0, 0) == Approx(Amn(0, 0)/s));
        // matrix /= scalar
        Amn = Bmn;
        Amn /= s;
        CHECK_EQUAL(Amn, Bmn/s);
    }

    SECTION("check cross product.")
    {
        int n = 3;
        Vector rangeMin(n, -100);
        Vector rangeMax(n, 100);
        Vector an = Rand::vector(rangeMin, rangeMax); // 3 dim vector
        Vector bn = Rand::vector(rangeMin, rangeMax); // 3 dim vector
        Matrix A(3, 3);
        double ann = norm(an);
        double bnn = norm(bn);

        // a x a = 0
        CHECK_EQUAL(cross(an, an), zeros(n));
        // norm(a x b) = norm(a)*norm(b)*sin(theta)
        CHECK_EQUAL(norm(cross(an, bn)), ann*bnn*sqrt(1.0-pow(dot(an, bn)/(ann*bnn), 2)));
        // cross product = cross product matrix
        CHECK_EQUAL(cross(an, bn), crossProductMatrix(an)*bn);
        crossProductMatrix(an, A);
        CHECK_EQUAL(A, crossProductMatrix(an));
        // a x b = -b x a
        CHECK_EQUAL(cross(an, bn), -1.0*cross(bn, an));
    }

    SECTION("check conversions from euler angles to matrix.")
    {
        Vector euler;
        euler.resize(3, 0.0);
        Matrix R;
        R.resize(4, 4);
        R.eye();
        CHECK_EQUAL(euler2dcm(euler), R); // euler2dcm([0 0 0]) = [1 0 0 0; 0 1 0 0; 0 0 1 0; 0 0 0 1]
        euler[0] = 0;
        euler[1] = M_PI/2;
        euler[2] = M_PI;
        R(0, 0) = 0.0;
        R(0, 2) = 1.0;
        R(1, 1) = -1.0;
        R(2, 0) = 1.0;
        R(2, 2) = 0.0;
        CHECK_EQUAL(euler2dcm(euler), R.transposed()); // euler2dcm([pi, pi/2, -pi]) = [0 0 1 0; 0 -1 0 0; 1 0 0 0; 0 0 0 1]

        INFO("check conversions from matrix to euler angles.");
        CHECK_EQUAL(dcm2euler(R), euler); // dcm2euler(matrix-of-previous-test) = [0, pi/2, -pi] );

        INFO("check conversions from matrix to axis/angle.");
        R.eye();
        R(0, 0) = -1.0;
        R(0, 1) = 0.0;
        R(2, 2) = -1.0;
        Vector axis;
        axis.resize(4, 0.0);
        axis[0] = axis[2] = 0;
        axis[1] = 1;
        axis[3] = M_PI;
        yarp::sig::Vector v = dcm2axis(R);
        CHECK_EQUAL(v, axis); // dcm2axis([-1.0 0 0 0; 0 1 0 0; 0 0 -1 0; 0 0 O 1]) = [0 0 1 0; 0 -1 0 0; 1 0 0 0; 0 0 0 1] );

        yarp::sig::Matrix m = axis2dcm(v);
        CHECK_EQUAL(m, R); // axis2dcm
    }

    SECTION("check sign function.")
    {
        double a;
        a = -2.0;
        CHECK_EQUAL(sign(a), -1.0); // sign(double) = +/-1

        Vector b, c;
        b.resize(3, 0.0);
        c.resize(3, 0.0);
        b[0] = -0.8;
        b[1] = 1.3;
        b[2] = -2.4;
        c[0] = -1.0;
        c[1] = 1.0;
        c[2] = -1.0;
        CHECK_EQUAL(sign(b), c); // sign(vector) = vector cointaing signs of elements
    }

    SECTION("check eigenValues function.")
    {
        Matrix A;
        Vector real, img, real2, img2;
        real.resize(2, 0.0);
        img.resize(2, 0.0);
        real2.resize(2, 0.0);
        img2.resize(2, 0.0);
        A.resize(2, 2);
        A(0, 0) = 0;
        A(0, 1) = 1.0;
        A(1, 0) = -2.0;
        A(1, 1) = 2.0;
        eigenValues(A, real, img);
        real2[0] = 1.0;
        real2[1] = 1.0;
        img2[0] = 1.0;
        img2[1] = -1.0;
        CHECK_EQUAL(real, real2); // eigenValues(matrix) = real part of eigenValues
        CHECK_EQUAL(img, img2); // eigenValues(matrix) = img part of eigenValues
    }

    SECTION("check max and min element.")
    {
        Vector a;
        a.resize(3, 0.0);
        a[0] = 2.5;
        a[1] = 3.1;
        a[2] = 4.7;
        CHECK_EQUAL(findMax(a), 4.7); // findMax(vector) = max-elem
        CHECK_EQUAL(findMin(a), 2.5); // findMin(vector) = min-elem
    }

    SECTION("check Quaternion class, ")
    {
        Quaternion q1;
        Quaternion q2;
        Quaternion q3;
        Quaternion q4;
        Matrix m;
        Matrix m_check;
        Vector v_check;

        double vA[3] = {0.7 * M_PI, 3 * M_PI, 1/6 * M_PI};
        double vB[3] = {1.2, 2.4 * M_PI, 0.5 * M_PI};
        Matrix mA = euler2dcm(Vector(3, vA));
        Matrix mB = euler2dcm(Vector(3, vB));
        q3.fromRotationMatrix(mA);
        q4.fromRotationMatrix(mB);
        CHECK_EQUAL(mA*mB, (q3 * q4).toRotationMatrix4x4()); // check quaternion multiplication
        q3 = Quaternion(2.0, 3.0, 4.0, 5.0);
        q3.normalize();
        CHECK_EQUAL(q3.w()*q3.w() + q3.x()*q3.x() + q3.y()*q3.y() + q3.z()*q3.z(), 1); // check quaternion normalization

        Vector vx(4);
        Vector vy(4);
        Vector vz(4);
        vx[0] = 1; vx[1] = 0; vx[2] = 0; vx[3] = M_PI / 3;
        vy[0] = 0; vy[1] = 1; vy[2] = 0; vy[3] = M_PI / 4;
        vz[0] = 0; vz[1] = 0; vz[2] = 1; vz[3] = M_PI / 5;
        Matrix mx = axis2dcm(vx);
        Matrix my = axis2dcm(vy);
        Matrix mz = axis2dcm(vz);
        //mx =
        //  1.00000   0.00000   0.00000
        //  0.00000   0.50000  -0.86603
        //  0.00000   0.86603   0.50000

        // my =
        //  0.70711   0.00000   0.70711
        //  0.00000   1.00000   0.00000
        // -0.70711   0.00000   0.70711

        // mz =
        // 0.80902   -0.58779   0.00000
        // 0.58779    0.80902   0.00000
        // 0.00000    0.00000   1.00000

        m = mx*my*mz;
        m_check.resize(4, 4);
        m_check[0][0] = 0.5720614028176844;    m_check[0][1] = -0.4156269377774535;   m_check[0][2] = 0.7071067811865475;   m_check[0][3] = 0;
        m_check[1][0] = 0.7893123335109140;    m_check[1][1] = 0.0445650105750650;    m_check[1][2] = -0.6123724356957946;  m_check[1][3] = 0;
        m_check[2][0] = 0.2230062590462850;    m_check[2][1] = 0.9084427381107635;    m_check[2][2] = 0.3535533905932738;   m_check[2][3] = 0;
        m_check[3][0] = 0;                     m_check[3][1] = 0;                     m_check[3][2] = 0;                    m_check[3][3] = 1.0;
        CHECK_EQUAL(m, m_check); // check m computation);

        // q1
        //0.8201 -0.3369i -0.4579j -0.06527k
        v_check.resize(4, 0.0);
        v_check[0] = 0.70181546790912619;
        v_check[1] = 0.54174325137682744;
        v_check[2] = 0.17244580102463122;
        v_check[3] = 0.42922225513145423;

        q1.fromRotationMatrix(m);
        CHECK_EQUAL(q1.w(), v_check[0]); // check w value method fromRotationMatrix
        CHECK_EQUAL(q1.x(), v_check[1]); // check x value method fromRotationMatrix
        CHECK_EQUAL(q1.y(), v_check[2]); // check y value method fromRotationMatrix
        CHECK_EQUAL(q1.z(), v_check[3]); // check z value method fromRotationMatrix

        q2 = q1;

        m = q2.toRotationMatrix4x4();
        CHECK_EQUAL(m, m_check); // check method toRotationMatrix4x4

        Vector v = q2.toVector();
        CHECK_EQUAL(v, v_check); // check method toVector

        double quat_mod = q2.abs();
        CHECK_EQUAL(quat_mod, 1.0); // check quaternion modulus

        q3.fromAxisAngle(vz);
        Vector vz_out = q3.toAxisAngle();

        CHECK_EQUAL(q3.w(), 0.951056516295154); // check w value method fromAxisAngle
        CHECK_EQUAL(q3.x(), 0);                 // check x value method fromAxisAngle
        CHECK_EQUAL(q3.y(), 0);                 // check y value method fromAxisAngle
        CHECK_EQUAL(q3.z(), 0.309016994374947); // check z value method fromAxisAngle

        CHECK_EQUAL(vz_out, vz); // check toAxisAngle

        INFO( string("check toString() method: ") + q1.toString());
    }

    SECTION("check Matrix concatenations")
    {
        Matrix a, b, c;
        b.resize(2, 1);
        c.resize(3, 1);
        a.resize(5, 1);
        b(0, 0) = b(1, 0) = 1;
        c(0, 0) = c(1, 0) = c(2, 0) = 2;
        a(0, 0) = a(1, 0) = 1;
        a(2, 0) = a(3, 0) = a(4, 0) = 2;
        CHECK_EQUAL(pile(b, c), a); // pile(matrix1, matrix2) = [matrix1; matrix2]

        Vector d;
        d.resize(1, 0.0);
        d[0] = 4;
        a.resize(3, 1);
        a(0, 0) = a(0, 1) = 1;
        a(0, 2) = 4;
        CHECK_EQUAL(pile(b, d), a); // pile(matrix, vector) = [matrix; vector]
        a(0, 0) = 4;
        a(0, 2) = 1;
        CHECK_EQUAL(pile(d, b), a); // pile(vector, matrix) = [vector; matrix]

        Vector e, f;
        e.resize(1, 0.0);
        e[0] = -3;
        f.resize(2, 0.0);
        f[0] = 4;
        f[1] = -3;
        CHECK_EQUAL(pile(d, e), f); // pile(vector, vector) = [vector; vector]
        a.resize(1, 5);
        a(0, 0) = a(0, 1) = 1;
        a(0, 2) = a(0, 3) = a(0, 4) = 2;
        CHECK_EQUAL(cat(b.transposed(), c.transposed()), a); // cat(matrix1, matrix2) = [matrix1, matrix2]
        a.resize(1, 3);
        a(0, 2) = 4;
        CHECK_EQUAL(cat(b.transposed(), d), a); // cat(matrix, v) = [matrix, vector]
        a(0, 0) = 4;
        a(0, 2) = 1;
        CHECK_EQUAL(cat(d, b.transposed()), a); // cat(v, matrix) = [vector, matrix]
        f[1] = 2.0;
        CHECK_EQUAL(cat(d, 2.0), f); // cat(vector, double) = [vector, double]
        f[0] = 2.0;
        f[1] = 4.0;
        CHECK_EQUAL(cat(2.0, d), f); // cat(double, vector) = [double, vector]
        f.resize(5, 0.0);
        f[0] = 1.0;
        f[1] = 2.0;
        f[2] = 3.0;
        f[3] = 4.0;
        f[4] = 5.0;
        CHECK_EQUAL(cat(1.0, 2.0, 3.0, 4.0, 5.0), f); // cat(n1, n2, n3, n4, n5) = [n1, n2, n3, n4, n5]
    }
}
