/*
 * Copyright (C) 2017 RobotCub Consortium
 * Authors: Alberto Cardellino
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


/**
 *
 * Tests for point cloud type
 *
 */

#include <yarp/os/Port.h>
#include <yarp/os/Time.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Network.h>
#include <yarp/os/NetType.h>
#include <yarp/sig/PointCloud.hpp>
#include <yarp/os/PortReaderBuffer.h>

#include "TestList.h"

using namespace yarp::sig;
using namespace yarp::os;

class PointCloudTest : public yarp::os::impl::UnitTest
{
public:
    virtual ConstString getName() override
    { return "PointCloudTest"; }

    void readWriteMatch()
    {
        Network::setLocalMode(true);
        report(0, "Checking XYZ_RGBA_DATA sending - Type match");
        BufferedPort< PointCloud<XYZ_RGBA_DATA> > outPort;
        Port inPort;
        checkTrue(outPort.open("/test/pointcloud/out"),"Opening output port");
        checkTrue(inPort.open("/test/pointcloud/in"),"Opening input port");
        checkTrue(NetworkBase::connect(outPort.getName(), inPort.getName()),"Checking connection");
        PointCloud<XYZ_RGBA_DATA>& testPC = outPort.prepare();
        int width  = 100;
        int height = 20;
        testPC.resize(width, height);

        for (int i=0; i<width*height; i++)
        {
            testPC.data[i].x = i;
            testPC.data[i].y = i + 1;
            testPC.data[i].z = i + 2;
            testPC.data[i].r = '1';
            testPC.data[i].g = '2';
            testPC.data[i].b = '3';
            testPC.data[i].a = '4';
        }

        yarp::os::Time::delay(0.3);

        outPort.write();

        PointCloud<XYZ_RGBA_DATA> inCloud;
        inPort.read(inCloud);

        checkTrue(inCloud.dataSizeBytes() == testPC.dataSizeBytes(), "Checking size consistency");

        bool ok = true;
        for (int i=0; i<width*height; i++)
        {
            ok &= inCloud.data[i].x == i;
            ok &= inCloud.data[i].y == i + 1;
            ok &= inCloud.data[i].z == i + 2;
            ok &= inCloud.data[i].r == '1';
            ok &= inCloud.data[i].g == '2';
            ok &= inCloud.data[i].b == '3';
            ok &= inCloud.data[i].a == '4';
        }

        checkTrue(ok, "Checking data validity");

        outPort.close();
        inPort.close();

        // TODO : check other data types..
        //        once implemented check toString()

    }

    virtual void readWriteMisMatch1()
    {
        Network::setLocalMode(true);
        report(0,"Testing the case in which we receive a structure bigger than the one we expect");
        BufferedPort< PointCloud<XYZ_RGBA_DATA> > outPort;
        Port inPort;
        checkTrue(outPort.open("/test/pointcloud/out"),"Opening output port");
        checkTrue(inPort.open("/test/pointcloud/in"),"Opening input port");
        checkTrue(NetworkBase::connect(outPort.getName(), inPort.getName()),"Checking connection");
        PointCloud<XYZ_RGBA_DATA>& testPC = outPort.prepare();
        int width  = 200;
        int height = 20;
        testPC.resize(width, height);

        for (int i=0; i<width*height; i++)
        {
            testPC.data[i].x = i;
            testPC.data[i].y = i + 1;
            testPC.data[i].z = i + 2;
            testPC.data[i].r = '1';
            testPC.data[i].g = '2';
            testPC.data[i].b = '3';
            testPC.data[i].a = '4';
        }

        yarp::os::Time::delay(0.3);

        outPort.write();

        PointCloud<XYZ_DATA> inCloud;
        inPort.read(inCloud);

        checkFalse(inCloud.dataSizeBytes() == testPC.dataSizeBytes(), "Checking size, correctly different");

        bool ok = true;
        for (int i=0; i<width*height; i++)
        {
            ok &= inCloud.data[i].x == i;
            ok &= inCloud.data[i].y == i + 1;
            ok &= inCloud.data[i].z == i + 2;
        }

        checkTrue(ok, "Checking data validity");

        outPort.close();
        inPort.close();


    }
    virtual void readWriteMisMatch2()
    {
        Network::setLocalMode(true);
        report(0,"Testing the case in which we receive a structure smaller than the one we expect");
        BufferedPort< PointCloud<XYZ_NORMAL_DATA> > outPort;
        Port inPort;
        checkTrue(outPort.open("/test/pointcloud/out"),"Opening output port");
        checkTrue(inPort.open("/test/pointcloud/in"),"Opening input port");
        checkTrue(NetworkBase::connect(outPort.getName(), inPort.getName()),"Checking connection");
        PointCloud<XYZ_NORMAL_DATA>& testPC = outPort.prepare();
        int width  = 200;
        int height = 42;
        testPC.resize(width, height);

        for (int i=0; i<width*height; i++)
        {
            testPC.data[i].x = i;
            testPC.data[i].y = i + 1;
            testPC.data[i].z = i + 2;
            testPC.data[i].normal_x = i * 2;
            testPC.data[i].normal_y = i * 3;
            testPC.data[i].normal_z = i * 4;
            testPC.data[i].curvature = i * 5;
        }

        yarp::os::Time::delay(0.3);

        outPort.write();

        PointCloud<XYZ_NORMAL_RGBA_DATA> inCloud;
        inPort.read(inCloud);

        checkTrue(inCloud.dataSizeBytes() == testPC.dataSizeBytes(), "Checking size, equals for the padding");

        bool ok = true;
        for (int i=0; i<width*height; i++)
        {
            ok &= inCloud.data[i].x == i;
            ok &= inCloud.data[i].y == i + 1;
            ok &= inCloud.data[i].z == i + 2;
            ok &= inCloud.data[i].normal_x == i * 2;
            ok &= inCloud.data[i].normal_y == i * 3;
            ok &= inCloud.data[i].normal_z == i * 4;
            ok &= inCloud.data[i].curvature ==i * 5;
            ok &= inCloud.data[i].r == 0;
            ok &= inCloud.data[i].g == 0;
            ok &= inCloud.data[i].b == 0;
            ok &= inCloud.data[i].a == 0;
        }

        checkTrue(ok, "Checking data validity");

        outPort.close();
        inPort.close();

    }

    void copyAndAssignment()
    {
        report(0,"Testing the copy constructor with PC of the same type");
        PointCloud<XYZ_RGBA_DATA> testPC;
        int width  = 5;
        int height = 5;
        testPC.resize(width, height);

        for (int i=0; i<width*height; i++)
        {
            testPC.data[i].x = i;
            testPC.data[i].y = i + 1;
            testPC.data[i].z = i + 2;
            testPC.data[i].r = '1';
            testPC.data[i].g = '2';
            testPC.data[i].b = '3';
            testPC.data[i].a = '4';
        }

        PointCloud<XYZ_RGBA_DATA> testPC2(testPC);

        checkTrue(testPC2.dataSizeBytes() == testPC.dataSizeBytes(), "Checking size");

        bool ok = true;
        for (int i=0; i<width*height; i++)
        {
            ok &= testPC2.data[i].x == i;
            ok &= testPC2.data[i].y == i + 1;
            ok &= testPC2.data[i].z == i + 2;
            ok &= testPC2.data[i].r == '1';
            ok &= testPC2.data[i].g == '2';
            ok &= testPC2.data[i].b == '3';
            ok &= testPC2.data[i].a == '4';
        }

        checkTrue(ok, "Checking data consistency");

        report(0,"Testing the copy constructor with PC of different types:");
        report(0,"Smaller structure built from bigger");
        PointCloud<XYZ_DATA> testPC3(testPC);

        checkFalse(testPC3.dataSizeBytes() == testPC.dataSizeBytes(), "Checking size, correctly different");
        checkTrue(testPC3.height() == testPC.height(), "Checking height");
        checkTrue(testPC3.width() == testPC.width(), "Checking width");

        ok = true;
        for (int i=0; i<width*height; i++)
        {
            ok &= testPC3.data[i].x == i;
            ok &= testPC3.data[i].y == i + 1;
            ok &= testPC3.data[i].z == i + 2;
        }

        checkTrue(ok, "Checking data consistency");

        report(0,"Testing the copy constructor with PC of different types:");
        report(0,"Bigger structure built from smaller");

        PointCloud<XYZ_NORMAL_DATA> testPC4(testPC3);

        checkFalse(testPC4.dataSizeBytes() == testPC3.dataSizeBytes(), "Checking size, correctly different");
        checkTrue(testPC4.height() == testPC3.height(), "Checking height");
        checkTrue(testPC4.width() == testPC3.width(), "Checking width");

        ok = true;
        for (int i=0; i<width*height; i++)
        {
            ok &= testPC4.data[i].x == i;
            ok &= testPC4.data[i].y == i + 1;
            ok &= testPC4.data[i].z == i + 2;
            ok &= testPC4.data[i].normal_x == 0.0;
            ok &= testPC4.data[i].normal_y == 0.0;
            ok &= testPC4.data[i].normal_z == 0.0;
            ok &= testPC4.data[i].curvature ==0.0;
        }

        checkTrue(ok, "Checking data consistency");

        report(0,"Testing the assignment operator with matching types");

        PointCloud<XYZ_NORMAL_DATA> testPC5 = testPC4;


        checkTrue(testPC5.dataSizeBytes() == testPC4.dataSizeBytes(), "Checking size");
        checkTrue(testPC5.height() == testPC4.height(), "Checking height");
        checkTrue(testPC5.width() == testPC4.width(), "Checking width");

        ok = true;
        for (int i=0; i<width*height; i++)
        {
            ok &= testPC5.data[i].x == i;
            ok &= testPC5.data[i].y == i + 1;
            ok &= testPC5.data[i].z == i + 2;
            ok &= testPC5.data[i].normal_x == 0.0;
            ok &= testPC5.data[i].normal_y == 0.0;
            ok &= testPC5.data[i].normal_z == 0.0;
            ok &= testPC5.data[i].curvature ==0.0;
        }

        checkTrue(ok, "Checking data consistency");

        report(0,"Testing the copy constructor for the curvature case:");
        report(0,"Smaller structure built from bigger");

        PointCloud<XYZ_NORMAL_RGBA_DATA> testPC6;
        testPC6.resize(width, height);

        for (int i=0; i<width*height; i++)
        {
            testPC6.data[i].x = i;
            testPC6.data[i].y = i + 1;
            testPC6.data[i].z = i + 2;
            testPC6.data[i].r = '1';
            testPC6.data[i].g = '2';
            testPC6.data[i].b = '3';
            testPC6.data[i].a = '4';
            testPC6.data[i].normal_x = i*2;
            testPC6.data[i].normal_y = i*3;
            testPC6.data[i].normal_z = i*4;
            testPC6.data[i].curvature = i*5;
        }

        PointCloud<XYZ_NORMAL_DATA> testPC7(testPC6);
        ok = true;
        for (int i=0; i<width*height; i++)
        {
            ok &= testPC7.data[i].x == i;
            ok &= testPC7.data[i].y == i + 1;
            ok &= testPC7.data[i].z == i + 2;
            ok &= testPC7.data[i].normal_x == i*2;
            ok &= testPC7.data[i].normal_y == i*3;
            ok &= testPC7.data[i].normal_z == i*4;
            ok &= testPC7.data[i].curvature ==i*5;
        }

        checkTrue(ok, "Checking data consistency");


    }

    virtual void runTests() override
    {
        readWriteMatch();
        readWriteMisMatch1();
        readWriteMisMatch2();
        copyAndAssignment();
    }
};


static PointCloudTest pointCloudTest_instance;

yarp::os::impl::UnitTest& getPointCloudTest() {
    return pointCloudTest_instance;
}

