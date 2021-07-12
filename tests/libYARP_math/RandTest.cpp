/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * \infile Tests for Rand.h/Rand.cpp
 */

#include <cstdio>

#include <yarp/math/Rand.h>
#include <yarp/math/NormRand.h>
#include <yarp/sig/Vector.h>
#include <yarp/sig/Matrix.h>

#include <cmath>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::os::impl;
using namespace yarp::sig;
using namespace yarp::math;

TEST_CASE("math::RandTest", "[yarp::math]")
{
    SECTION("checking random generation with normal distribution...")
    {

        const int N=10000;
        double u=1.0;
        double sigma=2;

        printf("Going to generated %d samples with u=%lf and sigma=%lf...", N, u, sigma);

        Vector uv(N);
        Vector sigmav(N);
        uv=1.0;
        sigmav=2;

        Vector rv=NormRand::vector(uv, sigmav);

        printf("done!\n");
        printf("Now performing some *basic* tests on the sequence\n");

        double average=0.0;
        double std=0.0;
        int k=0;
        for(k=0;k<N;k++)
        {
            average+=rv[k];
        }
        average/=N;

        for(k=0; k<N;k++)
        {
            std+=(rv[k]-average)*(rv[k]-average);
        }

        std/=N-1;
        std=sqrt(std);

        printf("Average: %lf\n", average);
        printf("Std: %lf\n", std);

        bool avOk=false;
        bool stdOk=false;

        if (fabs(average-u)<0.075)
            avOk=true;
        if (fabs(std-sigma)<0.075)
            stdOk=true;

        CHECK(avOk); // normal distribution average ~as requested
        CHECK(stdOk); // normal distribution std ~as requested
    }

    SECTION("checking random  matrix generation...")
    {
        int R=100;
        int C=100;
        Matrix rndM=Rand::matrix(R,C);
        double average=0.0;
        for(int r=0; r<R; r++)
            for(int c=0; c<C;c++)
            {
                average+=rndM[r][c];
            }
        average/=R*C;
        bool mGood=false;
        if(fabs(average-0.5)<0.01)
            mGood=true;

        CHECK(mGood); // random matrix
    }

    SECTION("checking random  number generation...")
    {
        //we check the impl class since all the others relay on that
        Rand::init(10);

        const int N=10000;

        printf("Going to generate %d numbers, normally distributed in the range [0-1]...", N);

        Vector rv=Rand::vector(N);

        printf("done!\n");
        printf("Now performing some *basic* tests on the sequence\n");
        //now we can perform clever tests to determine if the numbers we generated are good...
        double min=1e10;
        double max=0.0;
        double average=0.0;
        int k=0;
        for(k=0;k<N;k++)
        {
            if (rv[k]>max)
                max=rv[k];
            if (rv[k]<min)
                min=rv[k];

            average+=rv[k];
        }
        average/=N;

        //computing std
        double std=0.0;
        for(k=0;k<N;k++)
        {
            std+=(rv[k]-average)*(rv[k]-average);
        }
        std/=N-1;
        std=sqrt(std);

        printf("Maximum value was: %lf\n", max);
        printf("Minimum value was: %lf\n", min);
        printf("Average was: %lf, std was: %lf\n", average, std);

        CHECK(max<=1.0); // sequence is <= 1
        CHECK(max>=0.0); // sequence is >= 0

        bool avGood=false;
        bool stdGood=false;

        // 0.01 looks like a reasonable threhold, no particular reasons
        if (fabs(average-0.5)<0.01)
            avGood=true;
        if (fabs(std-0.28)<0.01)
            stdGood=true;

        CHECK(avGood); // average is ~0.5;
        CHECK(stdGood); // std is ~0.28;

        Rand::init(123);
        Vector v1=Rand::vector(N);
        Rand::init(123);
        Vector v2=Rand::vector(N);

        //check that v1 and v2 are equal
        bool tmp=(v1==v2);
        CHECK(tmp); // same seed produces identical sequences

        Rand::init(456);
        Vector v3=Rand::vector(N);
        tmp=(v1==v3);
        CHECK(!tmp); // different seeds generate different sequences

        Rand::init();
        Vector v4=Rand::vector(N);
        Rand::init();
        Vector v5=Rand::vector(N);
        tmp=(v1==v3);
        CHECK(!tmp); // default seed initialization for two sequences
    }
}
