/*
* Author: Marco Randazzo <marco.randazzo@iit.it>
* Copyright (C) 2017 The Robotcub consortium.
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

/**
 * \infile Tests for Vec2D.
 */

#include <cstdio>
#include <yarp/os/impl/UnitTest.h>
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

using namespace yarp::os;
using namespace yarp::os::impl;
using namespace yarp::sig;
using namespace yarp::math;
using namespace std;

const double TOL = 1e-8;

class Vec2DTest : public UnitTest {
public:
    virtual ConstString getName() { return "Vec2DTest"; }

    virtual void runTests()
    {
        Vec2D<int> s;
        Vec2D<int> a(1, 2);
        Vec2D<int> b(3, 4);
        Vec2D<int> c1 = a + b;
        Vec2D<int> c2 = a + b + c1;
        Vec2D<int> eq_a;

        eq_a = a;
        checkTrue(eq_a.x == 1 && eq_a.y == 2 && a.x == 1 && a.y == 2, "assignement operator ok");

        //some the following checks verify also that the original data have been not corrupted
        checkTrue(a.x == 1 && a.y == 2 && b.x == 3 && b.y == 4, "input data are ok");
        //checks the result of the sum
        checkTrue(c1.x == 4 && c1.y == 6, "sum of Vec2D ok");
        checkTrue(c2.x == 8 && c2.y == 12, "sum concatenation of Vec2D ok");

        Vec2D<int> d = a - b;
        checkTrue(a.x == 1 && a.y == 2 && b.x == 3 && b.y == 4, "input data are ok");
        checkTrue(d.x == -2 && d.y == -2, "sub of Vec2D ok");

        Vec2D<int> e = a;
        checkTrue(e.x == 1 && e.y == 2, "assignement operator of Vec2D ok");

        yarp::sig::Vector v = static_cast<yarp::sig::Vector>(a);
        checkTrue(v[0] == 1 && v[1] == 2, "method toVector() of Vec2D ok");

        Vec2D<int> vv = v;
        checkTrue(vv.x == 1 && vv.y == 2, "creation of Vec2D from vector () of Vec2D ok");

        int mod = b.norm();
        checkTrue(mod == 5, "method mod() ok");

        yarp::sig::Matrix mat;
        mat.resize(2, 2);
        mat[0][0] = 10; mat[0][1] = 20;
        mat[1][0] = 30; mat[1][1] = 40;
        Vec2D<int> vmat = mat*vv;
        checkTrue(vmat.x == 50 && vmat.y == 110, "product between Vec2D and yarp::sig::Matrix ok");

        Bottle bot;
        Vec2D<int> vecbot1 (1,2);
        bool copy1 = yarp::os::Portable::copyPortable(vecbot1, bot);
        checkTrue(copy1 && bot.size() == 2 && bot.get(0).asInt() == vecbot1.x && bot.get(1).asInt() == vecbot1.y, "copyPortable ok");

        Vec2D<double> vecbot2 (1.1,2.2);
        bool copy2 = yarp::os::Portable::copyPortable(vecbot2, bot);
        checkTrue(copy2 && bot.size() == 2 && bot.get(0).asInt() == vecbot1.x && bot.get(1).asInt() == vecbot1.y, "copyPortable ok");

        Bottle bot1; bot1.addInt(7); bot1.addInt(9);
        Vec2D<int> vecbot3;
        bool copy3 = yarp::os::Portable::copyPortable(bot1, vecbot3);
        checkTrue(copy3 && vecbot3.x == 7 && vecbot3.y == 9, "copyPortable ok");

        Bottle bot2; bot2.addDouble(7.1); bot2.addDouble(9.1);
        Vec2D<double> vecbot4;
        bool copy4 = yarp::os::Portable::copyPortable(bot2, vecbot4);
        checkTrue(copy4 && vecbot4.x == 7.1 && vecbot4.y == 9.1, "copyPortable ok");

        std::string str1 = vecbot1.toString();
        std::string str2 = vecbot2.toString();
        //yDebug() << str1 << str2;
        checkTrue(str1.size() != 0 && str2.size() != 0, "toString() method ok");
    }
};

static Vec2DTest theVec2DTest;

UnitTest& getVec2DTest()
{
    return theVec2DTest;
}

