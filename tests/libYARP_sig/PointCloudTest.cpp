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
        report(0,"Testing the case in which we receive a bigger structure");
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

        checkFalse(inCloud.dataSizeBytes() == testPC.dataSizeBytes(), "Checking size, correctly wrong");

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
        report(0,"Testing the case in which we receive a smaller structure");
        BufferedPort< PointCloud<XYZ_DATA> > outPort;
        Port inPort;
        checkTrue(outPort.open("/test/pointcloud/out"),"Opening output port");
        checkTrue(inPort.open("/test/pointcloud/in"),"Opening input port");
        checkTrue(NetworkBase::connect(outPort.getName(), inPort.getName()),"Checking connection");
        PointCloud<XYZ_DATA>& testPC = outPort.prepare();
        int width  = 200;
        int height = 42;
        testPC.resize(width, height);

        for (int i=0; i<width*height; i++)
        {
            testPC.data[i].x = i;
            testPC.data[i].y = i + 1;
            testPC.data[i].z = i + 2;
        }

        yarp::os::Time::delay(0.3);

        outPort.write();

        PointCloud<XYZ_NORMAL_DATA> inCloud;
        inPort.read(inCloud);

        checkFalse(inCloud.dataSizeBytes() == testPC.dataSizeBytes(), "Checking size, correctly wrong");

        bool ok = true;
        for (int i=0; i<width*height; i++)
        {
            ok &= inCloud.data[i].x == i;
            ok &= inCloud.data[i].y == i + 1;
            ok &= inCloud.data[i].z == i + 2;
            ok &= inCloud.data[i].normal_x == 0;// The data sent not contains normals...
            ok &= inCloud.data[i].normal_y == 0;// The data sent not contains normals...
            ok &= inCloud.data[i].normal_z == 0;// The data sent not contains normals...
        }

        checkTrue(ok, "Checking data validity");

        outPort.close();
        inPort.close();

    }

    virtual void runTests() override
    {
        readWriteMatch();
        readWriteMisMatch1();
        readWriteMisMatch2();
    }
};


static PointCloudTest pointCloudTest_instance;

yarp::os::impl::UnitTest& getPointCloudTest() {
    return pointCloudTest_instance;
}

