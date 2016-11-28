/*
 * Author: Lorenzo Natale and Giorgio Metta
 * Copyright (C) 2006 The Robotcub consortium
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <yarp/sig/Vector.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Port.h>
#include <yarp/os/Time.h>

//#include <vector>

#include <yarp/gsl/Gsl.h>
#include <yarp/gsl_compatibility.h>

#include "TestList.h"

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

    bool threadInit()
    {
        success=false;
        return true;
    }

    void run()
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

    bool threadInit()
    {
        success=false;
        return true;
    }

    void run()
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

class VectorTest : public UnitTest {
    
    bool checkConsistency(Vector &a)
    {
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

public:
    virtual ConstString getName() { return "VectorTest"; }

    void checkGsl()
    {
        Vector a(5);
        Vector b;
        b=a;
        checkTrue(checkConsistency(a), "gsldata consistent after creation");
        checkTrue(checkConsistency(b), "gsldata consistent after copy");
        b.resize(100);
        checkTrue(checkConsistency(b), "gsldata consistent after resize");

        for(int k=0;k<100;k++)
            a.push_back(1.0);

        checkConsistency(a);
        checkTrue(checkConsistency(a), "gsldata consistent after push");
        Vector c=a;
        checkTrue(checkConsistency(c), "gsldata consistent after init");
    }

    void checkFormat() {
        Vector v(10);

        report(0,"check vector format conforms to network standard...");
        {
            for (size_t i=0; i<v.size(); i++ ){
                v[i] = (double)i;
            }
        }
        BufferedConnectionWriter writer;
        v.write(writer);
        ConstString s = writer.toString();
        Bottle bot;
        bot.fromBinary(s.c_str(),(int)s.length());
        checkEqual((int)bot.size(),(int)v.size(),"size matches");
        {
            for (int i=0; i<bot.size(); i++) {
                checkTrue(bot.get(i).asDouble()>i-0.25,"bounded below");
                checkTrue(bot.get(i).asDouble()<i+0.25,"bounded above");
            }
        }
    }

    void checkSendReceive()
    {
        report(0, "check Vector send receive");

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

        checkTrue(senderThread->success, "Send test");
        checkTrue(receiverThread->success, "Receive test");

        delete receiverThread;
        delete senderThread;
    }

    void checkCopyCtor()
    {
        report(0, "check creation from double*");
        double dV[5]={0,1,2,3,4};
        Vector v1(5,dV);
        checkEqual((int)v1.size(),5,"ok");

        report(0,"check vectors copy constructor works...");
        Vector v(4);
        v[0]=99;
        v[1]=99;
        v[2]=99;
        v[3]=99;

        checkEqual((int)v.size(),4,"size set ok");

        Vector v2(v);
        checkEqual((int)v.size(),(int)v2.size(),"size matches");

        bool ok=true;
        for (int k=0; k<4; k++)
            ok=ok&&(v2[k]==v[k]);

        checkTrue(ok,"elements match");

        report(0, "check construction from empty vector");
        Vector empty1;
        Vector empty2(empty1);
    }


    void checkCopy() {
        report(0,"check vectors copy works...");
        Vector v(4);
        v[0]=99;
        v[1]=99;
        v[2]=99;
        v[3]=99;

        Vector v2;
        v2 = v;
        checkEqual((int)v.size(),(int)v2.size(),"size matches");

        bool ok=true;
        for (int k=0; k<4; k++)
            ok=ok&&(v2[k]==v[k]);

        checkTrue(ok,"elements match");

        report(0,"check bug #1601862...");
        Vector v3(10);
        Vector v4 = v3;

        report(0, "check copy of uninitialized vector");
        Vector empty1;
        Vector empty2;
        empty2=empty1;
    }

    void checkOperators()
    {
        report(0,"checking operator ==");

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

        checkTrue(ok, "operator== for vectors works");
    }

    void checkResize()
    {
        Vector ones;
        ones.resize(10, 1.1);

        bool ok=true;
        for(unsigned int k=0; k<ones.size(); k++)
            ok=ok&&(ones[k]==1.1);

        checkTrue(ok, "resize(int, double) works");

        ones.resize(15);
        // fill new values
        for(unsigned int k=10; k<ones.size(); k++)
            ones[k]=1.1;

        ok=true;
        for(unsigned int k=0; k<ones.size(); k++)
            ok=ok&&(ones[k]==1.1);

        checkTrue(ok, "resize(int) works");

        ones.resize(9);
        ok=true;
        for(unsigned int k=0; k<ones.size(); k++)
            ok=ok&&(ones[k]==1.1);

        checkTrue(ok, "resizing to smaller vector");

        ones.resize(10, 42.42);

        ok=true;
        for(unsigned int k=0; k<ones.size(); k++)
            ok=ok&&(ones[k]==42.42);

        checkTrue(ok, "resize(int, double) works");
    }

    void checkEmpty()
    {
        Vector v;
        v.resize(0);
        checkTrue(v.data()==NULL, "size 0 => null data()");
        v.resize(1);
        checkTrue(v.data()!=NULL, "size 1 => non-null data()");
        v.resize(2);
        checkTrue(v.data()!=NULL, "size 2 => non-null data()");
    }

    virtual void runTests() {
        Network::setLocalMode(true);
        checkFormat();
        checkCopyCtor();
        checkCopy();
        checkSendReceive();
        checkOperators();
        checkGsl();
        checkResize();
        checkEmpty();
        Network::setLocalMode(false);
    }
};

static VectorTest theVectorTest;

UnitTest& getVectorTest() {
    return theVectorTest;
}
