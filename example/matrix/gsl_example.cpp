/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <stdio.h>

//#define GSL_DLL

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_matrix_double.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>

//#include <gsl/gsl_math.h>
//#include <gsl/gsl_chebyshev.h>

#include <yarp/sig/Matrix.h>
#include <yarp/sig/Vector.h>
#include <yarp/gsl/Gsl.h>

using namespace yarp::sig;
using namespace yarp::gsl;

int main(int argc, const char **argv)
{
    Vector v1(5);
    Vector v2(5);
    v1=10;
    v2=1;

    double r;
    gsl_blas_ddot((gsl_vector *)GslVector(v1).getGslVector(),
                  (gsl_vector *)GslVector(v2).getGslVector(), &r);

    printf("Res vector: %lf\n", r);

    Matrix m1;
    Matrix m2;
    Matrix m3;
    m1.resize(2,3);
    m2.resize(3,2);
    m1=3;
    m2=3;

    m3.resize(2,2);
    m3.zero();

    //gsl_matrix_view A1 = gsl_matrix_view_array(m1.data(), 2, 3);
    //gsl_matrix_view A2 = gsl_matrix_view_array(m2.data(), 3, 2);
    //gsl_matrix_view A3 = gsl_matrix_view_array(m3.data(), 2, 2);

    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans,
        1.0,
        (const gsl_matrix *) GslMatrix(m1).getGslMatrix(),
        (const gsl_matrix *) GslMatrix(m2).getGslMatrix(),
        0.0,
        (gsl_matrix *)GslMatrix(m3).getGslMatrix());

    printf("Result: (%s)", m3.toString().c_str());

    return 0;
}
