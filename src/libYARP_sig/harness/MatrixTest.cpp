// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Lorenzo Natale
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <yarp/sig/Matrix.h>
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

class MThread1:public Thread
{
public:
	MThread1(Port *p)
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
		Matrix m;

		int times=10;

		while(times--)
		{
			m.resize(4,4);
			int r=0;
			int c=0;
			for(r=0; r<4; r++)
			for (c=0; c<4; c++)
				m[r][c]=99;

			portOut->write(m);
			Time::delay(0.1);

			m.resize(2,4);
			for(r=0; r<2; r++)
			for (c=0; c<4; c++)
				m[r][c]=66;

			portOut->write(m);
		}

		success=true;
	}

	Port *portOut;
	bool success;
};

class MThread2:public Thread
{
public:
	MThread2(Port *p)
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
		Matrix m;

		int times=10;
		bool ok=true;
		while(times--)
		{
			portIn->read(m);
			if ( (m.rows()!=4)||(m.cols()!=4))
				ok=false;


			portIn->read(m);

			if ( (m.rows()!=2)||(m.cols()!=4))
				ok=false;
		}

		success=ok;
	}

	Port *portIn;
	bool success;
};

class MatrixTest : public UnitTest {
public:
    virtual String getName() { return "MatrixTest"; }

    void checkSendReceive()
	{
		Port portIn;
		Port portOut;

		MThread2 *receiverThread=new MThread2(&portIn);
		MThread1 *senderThread=new MThread1(&portOut);

		portOut.open("/harness_sig/mtest/o");
		portIn.open("/harness_sig/mtest/i");
	
		Network::connect("/harness_sig/mtest/o", "/harness_sig/mtest/i");

		receiverThread->start();
		senderThread->start();

		receiverThread->stop();
		senderThread->stop();

		portOut.close();
		portIn.close();

		checkTrue(senderThread->success, "Send matrix test");
		checkTrue(receiverThread->success, "Receive matrix test");

		delete receiverThread;
		delete senderThread;
	}

	void checkCopyCtor()
	{
	    report(0,"check matrix copy constructor works...");
        Matrix m(4,4);
		int r=0;
		int c=0;
		for(r=0; r<4; r++)
		{
			for (c=0; c<4; c++)
				m[r][c]=1333;
		}

        Matrix m2(m);
        checkEqual(m.rows(),m2.rows(),"rows matches");
		checkEqual(m.cols(),m2.cols(),"cols matches");

		bool ok=true;
		for(r=0; r<4; r++)
			for (c=0; c<4; c++)
				ok=ok && ((m[r])[c]==(m2[r])[c]);

		checkTrue(ok,"elements match");
	}

    void checkCopy() {
	    report(0,"check matrix copy constructor works...");
        Matrix m(4,4);
		int r=0;
		int c=0;
		for(r=0; r<4; r++)
			for (c=0; c<4; c++)
				m[r][c]=99;
        
		Matrix m2(m);
        checkEqual(m.rows(),m2.rows(),"rows matches");
		checkEqual(m.cols(),m2.cols(),"cols matches");

		bool ok=true;
		for(r=0; r<4; r++)
			for (c=0; c<4; c++)
				ok=ok && (m[r][c]==m2[r][c]);
		checkTrue(ok,"elements match");   
	}

	void checkMiscOperations() {
	    report(0,"check matrix misc operations...");
        Matrix m(4,4);

		m.zero();
        m=MatrixOps::eye(5,5);
        m=MatrixOps::eye(2,5);
        m=MatrixOps::eye(5, 2);

		Matrix mt=m.transposed();
	}

	virtual void runTests() {
        Network::setLocalMode(true);
		checkCopyCtor();
        checkCopy();
		checkSendReceive();
		checkMiscOperations();
        Network::setLocalMode(false);
    }
};

static MatrixTest theMatrixTest;

UnitTest& getMatrixTest() {
    return theMatrixTest;
}
