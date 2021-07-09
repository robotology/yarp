/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * \infile Tests for Vec2D.
 */

#include <cstdio>
#include <yarp/os/Portable.h>

#include <yarp/math/Math.h>
#include <yarp/sig/Vector.h>
#include <yarp/math/Vec2D.h>

#include <yarp/os/Time.h>
#include <yarp/os/LogStream.h>

#if defined(_MSC_VER)
# define _USE_MATH_DEFINES
#endif
#include <cmath>
#include <string>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::os::impl;
using namespace yarp::sig;
using namespace yarp::math;
using namespace std;


TEST_CASE("math::Vec2DTest", "[yarp::math]")
{

    SECTION("Check Vec2D")
    {
        Vec2D<int> s;
        Vec2D<int> a(1, 2);
        Vec2D<int> b(3, 4);
        Vec2D<int> c1 = a + b;
        Vec2D<int> c2 = a + b + c1;
        Vec2D<int> eq_a;

        eq_a = a;
        CHECK((eq_a.x == 1 && eq_a.y == 2 && a.x == 1 && a.y == 2)); // assignement operator ok

        //some the following checks verify also that the original data have been not corrupted
        CHECK((a.x == 1 && a.y == 2 && b.x == 3 && b.y == 4)); // input data are ok
        //checks the result of the sum
        CHECK((c1.x == 4 && c1.y == 6)); // sum of Vec2D ok
        CHECK((c2.x == 8 && c2.y == 12)); // sum concatenation of Vec2D ok

        Vec2D<int> d = a - b;
        CHECK((a.x == 1 && a.y == 2 && b.x == 3 && b.y == 4)); // input data are ok
        CHECK((d.x == -2 && d.y == -2)); // sub of Vec2D ok

        Vec2D<int> e = a;
        CHECK((e.x == 1 && e.y == 2)); // assignement operator of Vec2D ok

        yarp::sig::Vector v = static_cast<yarp::sig::Vector>(a);
        CHECK((v[0] == 1 && v[1] == 2)); // method toVector() of Vec2D ok

        Vec2D<int> vv = v;
        CHECK((vv.x == 1 && vv.y == 2)); // creation of Vec2D from vector () of Vec2D ok

        int mod = b.norm();
        CHECK(mod == 5); // method mod() ok

        yarp::sig::Matrix mat;
        mat.resize(2, 2);
        mat[0][0] = 10; mat[0][1] = 20;
        mat[1][0] = 30; mat[1][1] = 40;
        Vec2D<int> vmat = mat*vv;
        CHECK((vmat.x == 50 && vmat.y == 110)); // product between Vec2D and yarp::sig::Matrix ok

        Bottle bot;
        Vec2D<int> vecbot1 (1,2);
        bool copy1 = yarp::os::Portable::copyPortable(vecbot1, bot);
        CHECK((copy1 && bot.size() == 2 && bot.get(0).asInt32() == vecbot1.x && bot.get(1).asInt32() == vecbot1.y)); //  copyPortable ok

        Vec2D<double> vecbot2 (1.1,2.2);
        bool copy2 = yarp::os::Portable::copyPortable(vecbot2, bot);
        CHECK((copy2 && bot.size() == 2 && bot.get(0).asInt32() == vecbot1.x && bot.get(1).asInt32() == vecbot1.y)); // copyPortable ok

        Bottle bot1; bot1.addInt32(7); bot1.addInt32(9);
        Vec2D<int> vecbot3;
        bool copy3 = yarp::os::Portable::copyPortable(bot1, vecbot3);
        CHECK((copy3 && vecbot3.x == 7 && vecbot3.y == 9)); // copyPortable ok

        Bottle bot2; bot2.addFloat64(7.1); bot2.addFloat64(9.1);
        Vec2D<double> vecbot4;
        bool copy4 = yarp::os::Portable::copyPortable(bot2, vecbot4);
        CHECK((copy4 && vecbot4.x == 7.1 && vecbot4.y == 9.1)); // copyPortable ok

        std::string str1 = vecbot1.toString();
        std::string str2 = vecbot2.toString();
        //yDebug() << str1 << str2;
        CHECK((str1.size() != 0 && str2.size() != 0)); // toString() method ok
    }
};
