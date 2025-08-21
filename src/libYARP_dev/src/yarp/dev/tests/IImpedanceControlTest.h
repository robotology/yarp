/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IIMPEDANCECONTROLTEST_H
#define IIMPEDANCECONTROLTEST_H

#include <yarp/dev/IImpedanceControl.h>
#include <catch2/catch_amalgamated.hpp>

#include <memory>
#include <numeric>
#include <vector>

using namespace yarp::dev;
using namespace yarp::os;

namespace yarp::dev::tests
{
    inline void exec_iImpedanceControl_test_1(IImpedanceControl* iimp)
    {
        REQUIRE(iimp != nullptr);

        int axis=0;
        double val = 0;
        bool b = true;

        b = iimp->getAxes(&axis);
        CHECK (b);
        CHECK (axis>0);

        double vstiff=1;
        double vdamp=2;
        double voff=3;

        {
            double stiff=0;
            double damp=0;
            b = iimp->getImpedance(0, &stiff, &damp);
            CHECK(b);
            CHECK(stiff==0);
            CHECK(damp==0);

            double off=0;
            b = iimp->getImpedanceOffset(0, &off);
            CHECK(b);
            CHECK(off==0);
        }

        {
            b = iimp->setImpedance(0, vstiff, vdamp);
            CHECK(b);
            b = iimp->setImpedanceOffset(0, voff);
            CHECK(b);

            double stiff=0;
            double damp=0;
            b = iimp->getImpedance(0, &stiff, &damp);
            CHECK(b);
            CHECK(stiff==1);
            CHECK(damp==2);

            double off=0;
            b = iimp->getImpedanceOffset(0, &off);
            CHECK(b);
            CHECK(off==3);
        }

        {
            double mins=0;
            double maxs=0;
            double mind=0;
            double maxd=0;
            b = iimp->getCurrentImpedanceLimit(0, &mins,&maxs, &mind, &maxd);
            CHECK(b);
            CHECK(mins==1.0);
            CHECK(maxs==10.0);
            CHECK(mind==2.0);
            CHECK(maxd==20.0);
        }

    }
}

#endif
