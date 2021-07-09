/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/math/Math.h>
#include <stdio.h>

using namespace yarp::sig;
using namespace yarp::math;

int main(int argc, const char **argv)
{
    Matrix m1;
    Matrix m2;
    m1=eye(5,5);    //build a 5x5 identity matrix
    m2=zeros(3,3);  //this is a 3x3 matrix of zeros

    //single element access
    for (int r=0;r<3;r++)
        for(int c=0;c<3;c++)
              m2(r,c)=r;

    Vector v;
    v=zeros(10);    //build a 10 element vector of zeros
    v=10;                   //set all elements to 10

    Matrix sm=m2.submatrix(1,2,3,3); //extract submatrix from 1,2 to 3,3
    Matrix mt=m2.transposed();      //transpose m2

    printf("Vector is: (%s)\n", v.toString().c_str());
    printf("M1 is: (%s)\n", m1.toString().c_str());
    printf("M2 is: (%s)\n", m2.toString().c_str());
    printf("M2T is: (%s)\n", mt.toString().c_str());
    printf("SM is: (%s)\n", sm.toString().c_str());

    return 0;
}
