// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Lorenzo Natale and Giorgio Metta
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/sig/Vector.h>
#include <yarp/BufferedConnectionWriter.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Port.h>
#include <yarp/os/Time.h>

#include <vector>

#include "TestList.h"

using namespace yarp;
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
			int s=v.size();
			if (s!=10)
				ok=false;
			for(k=0;k<s;k++)
			{
				if (v[k]!=3.14159265)
					ok=false;
			}

			portIn->read(v);
			s=v.size();
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
public:
    virtual String getName() { return "VectorTest"; }

    void checkFormat() {
	    Vector v;

        report(0,"check vector format conforms to network standard...");
        {
            for (unsigned int i=0; i<v.size(); i++ ){
                v[i] = i;
            }
        }
        BufferedConnectionWriter writer;
        v.write(writer);
        String s = writer.toString();
        Bottle bot;
        bot.fromBinary(s.c_str(),s.length());
        checkEqual(bot.size(),v.size(),"size matches");
        {
            for (int i=0; i<bot.size(); i++) {
                checkTrue(bot.get(i).asDouble()>i-0.25,"bounded below");
                checkTrue(bot.get(i).asDouble()<i+0.25,"bounded above");
            }
        }
    }

	void checkSendReceive()
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

		checkTrue(senderThread->success, "Send test");
		checkTrue(receiverThread->success, "Receive test");

		delete receiverThread;
		delete senderThread;
	}

	void checkCopyCtor()
	{
	    report(0,"check vectors copy constructor works...");
        Vector v(4);
		v[0]=99;
		v[1]=99;
		v[2]=99;
		v[3]=99;

        Vector v2(v);
        checkEqual(v.size(),v2.size(),"size matches");

		bool ok=true;
		for (int k=0; k<4; k++)
			ok=ok&&(v2[k]==v[k]);

		checkTrue(ok,"elements match");
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
        checkEqual(v.size(),v2.size(),"size matches");

		bool ok=true;
		for (int k=0; k<4; k++)
			ok=ok&&(v2[k]==v[k]);

		checkTrue(ok,"elements match");

        report(0,"check bug #1601862...");
        std::vector<sig::Vector> myList; //using stl vector
        Vector v3(10);
        myList.push_back(v3);
        // this segfaults on linux
    }

	virtual void runTests() {
        checkFormat();
		checkCopyCtor();
        checkCopy();
		checkSendReceive();
    }
};

static VectorTest theVectorTest;

UnitTest& getVectorTest() {
    return theVectorTest;
}
