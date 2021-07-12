/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/sig/Vector.h>

#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Port.h>
#include <yarp/os/Time.h>

#include <algorithm>

#include <yarp/gsl/Gsl.h>
#include <yarp/gsl/impl/gsl_structs.h>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os::impl;
using namespace yarp::os;
using namespace yarp::sig;

class Thread1:public Thread
{
public:
    Thread1(Port *p)
    {
        portOut=p;
    }

    bool threadInit() override
    {
        success=false;
        return true;
    }

    void run() override
    {
        Vector v;

        int times=10;

        while(times--)
        {
            v.clear();
            int k=0;
            for(k=0;k<10;k++)
                v.push_back(3.14159265);

            portOut->write(v);
            Time::delay(0.1);
            v.clear();

            for(k=0;k<5;k++)
                v.push_back(k);

            portOut->write(v);
        }

        success=true;
    }

    Port *portOut;
    bool success;
};

class Thread2:public Thread
{
public:
    Thread2(Port *p)
    {
        portIn=p;
    }

    bool threadInit() override
    {
        success=false;
        return true;
    }

    void run() override
    {
        Vector v;

        int times=10;
        bool ok=true;
        while(times--)
        {
            portIn->read(v);
            //check if I received 42 42s

            int k;
            int s=(int)v.size();
            if (s!=10)
                ok=false;
            for(k=0;k<s;k++)
            {
                if (v[k]!=3.14159265)
                    ok=false;
            }

            portIn->read(v);
            s=(int)v.size();
            if (s!=5)
                ok=false;
            for(k=0;k<s;k++)
            {
                if (v[k]!=k)
                    ok=false;
            }

        }

        success=ok;
    }

    Port *portIn;
    bool success;
};

bool checkConsistency(Vector &a) {
    gsl_vector *tmp;
    yarp::gsl::GslVector tmpGSL(a);
    tmp = (gsl_vector *)(tmpGSL.getGslVector());

    bool ret=true;
    if ((int)tmp->size!=(int)a.size())
        ret=false;

    if (tmp->data!=a.data())
        ret=false;

    if (tmp->block->data!=a.data())
        ret=false;

    return ret;
}

Vector functionThatReturnsCopyOfReference(const Vector&a) {
    Vector ret_a = a;
    if (false) { return Vector(0); }
    return ret_a;
}

TEST_CASE("sig::VectorTest", "[yarp::sig]")
{
    NetworkBase::setLocalMode(true);

    SECTION("check gsl")
    {
        Vector a(5);
        Vector b;
        b=a;
        CHECK(checkConsistency(a)); // gsldata consistent after creation
        CHECK(checkConsistency(b)); // gsldata consistent after copy
        b.resize(100);
        CHECK(checkConsistency(b)); // gsldata consistent after resize

        for(int k=0;k<100;k++)
            a.push_back(1.0);

        checkConsistency(a);
        CHECK(checkConsistency(a)); // gsldata consistent after push
        Vector c=a;
        CHECK(checkConsistency(c)); // gsldata consistent after init
    }

    SECTION("check format")
    {
        Vector v(10);

        INFO("check vector format conforms to network standard...");
        {
            for (size_t i=0; i<v.size(); i++ ){
                v[i] = (double)i;
            }
        }
        BufferedConnectionWriter writer;
        v.write(writer);
        std::string s = writer.toString();
        Bottle bot;
        bot.fromBinary(s.c_str(), s.length());
        CHECK(bot.size() ==  v.size()); // size matches
        {
            for (size_t i=0; i<bot.size(); i++) {
                CHECK(bot.get(i).asFloat64()>i-0.25); // bounded below
                CHECK(bot.get(i).asFloat64()<i+0.25); // bounded above
            }
        }
    }

    SECTION("check Vector send receive")
    {

        Port portIn;
        Port portOut;

        Thread2 *receiverThread=new Thread2(&portIn);
        Thread1 *senderThread=new Thread1(&portOut);

        portOut.open("/harness_sig/vtest/o");
        portIn.open("/harness_sig/vtest/i");

        Network::connect("/harness_sig/vtest/o", "/harness_sig/vtest/i");

        receiverThread->start();
        senderThread->start();

        receiverThread->stop();
        senderThread->stop();

        portOut.close();
        portIn.close();

        CHECK(senderThread->success); // Send test
        CHECK(receiverThread->success); // Receive test

        delete receiverThread;
        delete senderThread;
    }

    SECTION("check creation from double*")
    {
        double dV[5]={0,1,2,3,4};
        Vector v1(5,dV);
        CHECK((int)v1.size() == 5); // ok

        INFO("check vectors copy constructor works...");
        Vector v(4);
        v[0]=99;
        v[1]=99;
        v[2]=99;
        v[3]=99;

        CHECK((int)v.size() == 4); // size set ok

        Vector v2(v);
        CHECK((int)v.size() == (int)v2.size()); // size matches

        bool ok=true;
        for (int k=0; k<4; k++)
            ok=ok&&(v2[k]==v[k]);

        CHECK(ok); // elements match

        INFO( "check construction from empty vector");
        Vector empty1;
        Vector empty2(empty1);
    }


    SECTION("check vectors copy works...")
    {
        Vector v(4);
        v[0]=99;
        v[1]=99;
        v[2]=99;
        v[3]=99;

        Vector v2;
        v2 = v;
        CHECK((int)v.size() == (int)v2.size()); // size matches

        bool ok=true;
        for (int k=0; k<4; k++)
            ok=ok&&(v2[k]==v[k]);

        CHECK(ok); // elements match

        INFO("check bug #1601862...");
        Vector v3(10);
        Vector v4 = v3;

        INFO( "check copy of uninitialized vector");
        Vector empty1;
        Vector empty2;
        empty2=empty1;
    }

    SECTION("checking operator ==")
    {

        Vector v1(5);
        Vector v2(5);

        v1=1;
        v2=1; //now we have to identical vectors

        bool ok=false;
        if (v1==v2)
            ok=true;

        v1=2;
        v2=1; //now vectors are different
        if (v1==v2)
            ok=false;

        CHECK(ok); // operator== for vectors works
    }

    SECTION("check resize")
    {
        Vector ones;
        ones.resize(10, 1.1);

        bool ok=true;
        for(unsigned int k=0; k<ones.size(); k++)
            ok=ok&&(ones[k]==1.1);

        CHECK(ok); // resize(int, double) works

        ones.resize(15);
        // fill new values
        for(unsigned int k=10; k<ones.size(); k++)
            ones[k]=1.1;

        ok=true;
        for(unsigned int k=0; k<ones.size(); k++)
            ok=ok&&(ones[k]==1.1);

        CHECK(ok); // resize(int) works

        ones.resize(9);
        ok=true;
        for(unsigned int k=0; k<ones.size(); k++)
            ok=ok&&(ones[k]==1.1);

        CHECK(ok); // resizing to smaller vector

        ones.resize(10, 42.42);

        ok=true;
        for(unsigned int k=0; k<ones.size(); k++)
            ok=ok&&(ones[k]==42.42);

        CHECK(ok); // resize(int, double) works
    }

    SECTION("check empty")
    {
        Vector v;
        v.resize(0);
        CHECK(v.data()==nullptr); // size 0 => null data()
        v.resize(1);
        CHECK(v.data()!=nullptr); // size 1 => non-null data()
        v.resize(2);
        CHECK(v.data()!=nullptr); // size 2 => non-null data()
    }

    SECTION("Checking the functionalities of the initializer list constructor")
    {
        Vector v{1.0, 2.0, 3.0};
        CHECK(v.size() == (size_t) 3); // Checking size

        CHECK(v[0] == 1.0); // Checking data consistency
        CHECK(v[1] == 2.0); // Checking data consistency
        CHECK(v[2] == 3.0); // Checking data consistency
    }

    SECTION("Checking the the for range based")
    {
        Vector v{0.0, 1.0, 2.0, 3.0, 4.0};
        double i = 0.0;
        for(const auto& el:v) {
            CHECK(el == i); // Checking data consistency
            i+=1.0;
        }

        INFO("Checking the std::transform");
        std::transform(v.begin(), v.end(), v.begin(), [](double d) { return d*2; });
        // Checking data consistency
        CHECK(v[0] == 0.0);
        CHECK(v[1] == 2.0);
        CHECK(v[2] == 4.0);
        CHECK(v[3] == 6.0);
        CHECK(v[4] == 8.0);
    }

    SECTION("Regression test for GitHub issue 2189")
    {
        Vector v{1.0, 2.0, 3.0};
        Vector vCopy = functionThatReturnsCopyOfReference(v);
        CHECK(vCopy.size() == v.size());
    }

    NetworkBase::setLocalMode(false);
}
