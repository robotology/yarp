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

float acceptedDiff = 1e-6;

class PointCloudTest : public yarp::os::impl::UnitTest
{
public:
    virtual ConstString getName() override
    { return "PointCloudTest"; }

    void readWriteMatchTest()
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
            testPC(i).x = i;
            testPC(i).y = i + 1;
            testPC(i).z = i + 2;
            testPC(i).r = '1';
            testPC(i).g = '2';
            testPC(i).b = '3';
            testPC(i).a = '4';
        }

        yarp::os::Time::delay(0.3);

        outPort.write();

        PointCloud<XYZ_RGBA_DATA> inCloud;
        inPort.read(inCloud);

        checkTrue(inCloud.dataSizeBytes() == testPC.dataSizeBytes(), "Checking size consistency");

        bool ok = true;
        for (int i=0; i<width*height; i++)
        {
            ok &= inCloud(i).x == i;
            ok &= inCloud(i).y == i + 1;
            ok &= inCloud(i).z == i + 2;
            ok &= inCloud(i).r == '1';
            ok &= inCloud(i).g == '2';
            ok &= inCloud(i).b == '3';
            ok &= inCloud(i).a == '4';
        }

        checkTrue(ok, "Checking data validity");

        outPort.close();
        inPort.close();

        // TODO : check other data types..
        //        once implemented check toString()

    }

    virtual void readWriteMisMatch1Test()
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
            testPC(i).x = i;
            testPC(i).y = i + 1;
            testPC(i).z = i + 2;
            testPC(i).r = '1';
            testPC(i).g = '2';
            testPC(i).b = '3';
            testPC(i).a = '4';
        }

        yarp::os::Time::delay(0.3);

        outPort.write();

        PointCloud<XYZ_DATA> inCloud;
        inPort.read(inCloud);

        checkFalse(inCloud.dataSizeBytes() == testPC.dataSizeBytes(), "Checking size, correctly different");

        bool ok = true;
        for (int i=0; i<width*height; i++)
        {
            ok &= inCloud(i).x == i;
            ok &= inCloud(i).y == i + 1;
            ok &= inCloud(i).z == i + 2;
        }

        checkTrue(ok, "Checking data validity");

        outPort.close();
        inPort.close();


    }
    virtual void readWriteMisMatch2Test()
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
            testPC(i).x = i;
            testPC(i).y = i + 1;
            testPC(i).z = i + 2;
            testPC(i).normal_x = i * 2;
            testPC(i).normal_y = i * 3;
            testPC(i).normal_z = i * 4;
            testPC(i).curvature = i * 5;
        }

        yarp::os::Time::delay(0.3);

        outPort.write();

        PointCloud<XYZ_NORMAL_RGBA_DATA> inCloud;
        inPort.read(inCloud);

        checkTrue(inCloud.dataSizeBytes() == testPC.dataSizeBytes(), "Checking size, equals for the padding");

        bool ok = true;
        for (int i=0; i<width*height; i++)
        {
            ok &= inCloud(i).x == i;
            ok &= inCloud(i).y == i + 1;
            ok &= inCloud(i).z == i + 2;
            ok &= inCloud(i).normal_x == i * 2;
            ok &= inCloud(i).normal_y == i * 3;
            ok &= inCloud(i).normal_z == i * 4;
            ok &= inCloud(i).curvature ==i * 5;
            ok &= inCloud(i).r == 0;
            ok &= inCloud(i).g == 0;
            ok &= inCloud(i).b == 0;
            ok &= inCloud(i).a == 0;
        }

        checkTrue(ok, "Checking data validity");

        outPort.close();
        inPort.close();

    }

    void copyAndAssignmentTest()
    {
        report(0,"Testing the copy constructor with PC of the same type");
        PointCloud<XYZ_RGBA_DATA> testPC;
        int width  = 5;
        int height = 5;
        testPC.resize(width, height);

        for (int i=0; i<width*height; i++)
        {
            testPC(i).x = i;
            testPC(i).y = i + 1;
            testPC(i).z = i + 2;
            testPC(i).r = '1';
            testPC(i).g = '2';
            testPC(i).b = '3';
            testPC(i).a = '4';
        }

        PointCloud<XYZ_RGBA_DATA> testPC2(testPC);

        checkTrue(testPC2.dataSizeBytes() == testPC.dataSizeBytes(), "Checking size");

        bool ok = true;
        for (int i=0; i<width*height; i++)
        {
            ok &= testPC2(i).x == i;
            ok &= testPC2(i).y == i + 1;
            ok &= testPC2(i).z == i + 2;
            ok &= testPC2(i).r == '1';
            ok &= testPC2(i).g == '2';
            ok &= testPC2(i).b == '3';
            ok &= testPC2(i).a == '4';
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
            ok &= testPC3(i).x == i;
            ok &= testPC3(i).y == i + 1;
            ok &= testPC3(i).z == i + 2;
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
            ok &= testPC4(i).x == i;
            ok &= testPC4(i).y == i + 1;
            ok &= testPC4(i).z == i + 2;
            ok &= testPC4(i).normal_x == 0.0;
            ok &= testPC4(i).normal_y == 0.0;
            ok &= testPC4(i).normal_z == 0.0;
            ok &= testPC4(i).curvature ==0.0;
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
            ok &= testPC5(i).x == i;
            ok &= testPC5(i).y == i + 1;
            ok &= testPC5(i).z == i + 2;
            ok &= testPC5(i).normal_x == 0.0;
            ok &= testPC5(i).normal_y == 0.0;
            ok &= testPC5(i).normal_z == 0.0;
            ok &= testPC5(i).curvature ==0.0;
        }

        checkTrue(ok, "Checking data consistency");

        report(0,"Testing the copy constructor for the curvature case:");
        report(0,"Smaller structure built from bigger");

        PointCloud<XYZ_NORMAL_RGBA_DATA> testPC6;
        testPC6.resize(width, height);

        for (int i=0; i<width*height; i++)
        {
            testPC6(i).x = i;
            testPC6(i).y = i + 1;
            testPC6(i).z = i + 2;
            testPC6(i).r = '1';
            testPC6(i).g = '2';
            testPC6(i).b = '3';
            testPC6(i).a = '4';
            testPC6(i).normal_x = i*2;
            testPC6(i).normal_y = i*3;
            testPC6(i).normal_z = i*4;
            testPC6(i).curvature = i*5;
        }

        PointCloud<XYZ_NORMAL_DATA> testPC7(testPC6);
        ok = true;
        for (int i=0; i<width*height; i++)
        {
            ok &= testPC7(i).x == i;
            ok &= testPC7(i).y == i + 1;
            ok &= testPC7(i).z == i + 2;
            ok &= testPC7(i).normal_x == i*2;
            ok &= testPC7(i).normal_y == i*3;
            ok &= testPC7(i).normal_z == i*4;
            ok &= testPC7(i).curvature ==i*5;
        }

        checkTrue(ok, "Checking data consistency");


        report(0,"Testing the assignment operator with not matching types");
        report(0,"Smaller structure built from bigger");

        PointCloud<XYZ_DATA> testPC8 = testPC6;


        checkFalse(testPC8.dataSizeBytes() == testPC6.dataSizeBytes(), "Checking size, correctly different");
        checkTrue(testPC8.height() == testPC6.height(), "Checking height");
        checkTrue(testPC8.width() == testPC6.width(), "Checking width");

        ok = true;
        for (int i=0; i<width*height; i++)
        {
            ok &= testPC8(i).x == i;
            ok &= testPC8(i).y == i + 1;
            ok &= testPC8(i).z == i + 2;
        }

        checkTrue(ok, "Checking data consistency");

        report(0,"Testing the assignment operator with not matching types");
        report(0,"Bigger structure built from smaller");

        PointCloud<XYZ_NORMAL_RGBA_DATA> testPC9 = testPC7;


        checkTrue(testPC9.dataSizeBytes() == testPC7.dataSizeBytes(), "Checking size");
        checkTrue(testPC9.height() == testPC7.height(), "Checking height");
        checkTrue(testPC9.width() == testPC7.width(), "Checking width");

        ok = true;
        for (int i=0; i<width*height; i++)
        {
            ok &= testPC9(i).x == i;
            ok &= testPC9(i).y == i + 1;
            ok &= testPC9(i).z == i + 2;
            ok &= testPC9(i).normal_x == i*2;
            ok &= testPC9(i).normal_y == i*3;
            ok &= testPC9(i).normal_z == i*4;
            ok &= testPC9(i).curvature ==i*5;
            ok &= testPC9(i).r == 0;
            ok &= testPC9(i).g == 0;
            ok &= testPC9(i).b == 0;
            ok &= testPC9(i).a == 0;
        }

        checkTrue(ok, "Checking data consistency");


    }

    void fromExternalTest()
    {
        report(0,"Testing the fromExternalPC with PC of the same type");
        PointCloud<XYZ_RGBA_DATA> testPC;
        int width  = 32;
        int height = 25;
        testPC.resize(width, height);

        for (int i=0; i<width*height; i++)
        {
            testPC(i).x = i;
            testPC(i).y = i + 1;
            testPC(i).z = i + 2;
            testPC(i).r = '1';
            testPC(i).g = '2';
            testPC(i).b = '3';
            testPC(i).a = '4';
        }

        PointCloud<XYZ_RGBA_DATA> testPC2;
        testPC2.fromExternalPC(testPC.getRawData(), PCL_POINT_XYZ_RGBA, width, height);

        checkTrue(testPC2.dataSizeBytes() == testPC.dataSizeBytes(), "Checking size");

        bool ok = true;
        for (int i=0; i<width*height; i++)
        {
            ok &= testPC2(i).x == i;
            ok &= testPC2(i).y == i + 1;
            ok &= testPC2(i).z == i + 2;
            ok &= testPC2(i).r == '1';
            ok &= testPC2(i).g == '2';
            ok &= testPC2(i).b == '3';
            ok &= testPC2(i).a == '4';
        }

        checkTrue(ok, "Checking data consistency");

        report(0,"Testing the fromExternalPC with PC of different types:");
        report(0,"Smaller structure built from bigger");
        PointCloud<XYZ_DATA> testPC3;

        testPC3.fromExternalPC(testPC2.getRawData(), PCL_POINT_XYZ_RGBA, width, height);

        checkFalse(testPC3.dataSizeBytes() == testPC2.dataSizeBytes(), "Checking size, correctly different");
        checkTrue(testPC3.height() == testPC2.height(), "Checking height");
        checkTrue(testPC3.width() == testPC2.width(), "Checking width");

        ok = true;
        for (int i=0; i<width*height; i++)
        {
            ok &= testPC3(i).x == i;
            ok &= testPC3(i).y == i + 1;
            ok &= testPC3(i).z == i + 2;
        }

        checkTrue(ok, "Checking data consistency");

        report(0,"Testing the fromExternalPC with PC of different types:");
        report(0,"Bigger structure built from smaller");

        PointCloud<XYZ_NORMAL_DATA> testPC4(testPC3);

        testPC4.fromExternalPC(testPC3.getRawData(), PCL_POINT_XYZ, width, height);

        checkFalse(testPC4.dataSizeBytes() == testPC3.dataSizeBytes(), "Checking size, correctly different");
        checkTrue(testPC4.height() == testPC3.height(), "Checking height");
        checkTrue(testPC4.width() == testPC3.width(), "Checking width");

        ok = true;
        for (int i=0; i<width*height; i++)
        {
            ok &= testPC4(i).x == i;
            ok &= testPC4(i).y == i + 1;
            ok &= testPC4(i).z == i + 2;
            ok &= testPC4(i).normal_x == 0.0;
            ok &= testPC4(i).normal_y == 0.0;
            ok &= testPC4(i).normal_z == 0.0;
            ok &= testPC4(i).curvature ==0.0;
        }

        checkTrue(ok, "Checking data consistency");
    }

    void concatenationTest()
    {
        report(0,"Testing the operator+ with PC of the same type");
        PointCloud<XYZ_NORMAL_RGBA_DATA> testPC;
        PointCloud<XYZ_NORMAL_RGBA_DATA> testPC2;
        int width  = 35;
        int height = 62;
        testPC.resize(width, height);
        testPC2.resize(width, height);

        for (int i=0; i<width*height; i++)
        {
            testPC(i).x = i;    testPC2(i).x = i;
            testPC(i).y = i + 1;testPC2(i).y = i*2;
            testPC(i).z = i + 2;testPC2(i).z = i*3;
            testPC(i).r = '1';  testPC2(i).r = 'r';
            testPC(i).g = '2';  testPC2(i).g = 'g';
            testPC(i).b = '3';  testPC2(i).b = 'b';
            testPC(i).a = '4';  testPC2(i).a = 'a';
        }

        PointCloud<XYZ_NORMAL_RGBA_DATA> sumPC;
        sumPC = testPC + testPC2;

        checkTrue(sumPC.size() == (size_t) (width*height*2), "Checking the size");

        bool ok = true;
        for (int i=0; i<width*height; i++)
        {
            ok &= sumPC(i).x == i;
            ok &= sumPC(i).y == i + 1;
            ok &= sumPC(i).z == i + 2;
            ok &= sumPC(i).r == '1';
            ok &= sumPC(i).g == '2';
            ok &= sumPC(i).b == '3';
            ok &= sumPC(i).a == '4';
        }


        checkTrue(ok,"Checking data consistency: part1");

        ok = true;
        for (int i=width*height; i<(2*(width*height)); i++)
        {
            ok &= sumPC(i).x == (i-(width*height));
            ok &= sumPC(i).y == (i-(width*height))*2;
            ok &= sumPC(i).z == (i-(width*height))*3;
            ok &= sumPC(i).r == 'r';
            ok &= sumPC(i).g == 'g';
            ok &= sumPC(i).b == 'b';
            ok &= sumPC(i).a == 'a';
        }

        checkTrue(ok,"Checking data consistency: part2");

        report(0,"Testing the operator+= with PC of the same type");


        testPC += testPC2;

        XYZ_NORMAL_RGBA_DATA point;
        point.x = 1.1; point.y = 1.2; point.z = 1.3;
        point.normal_x = 2.1; point.normal_y = 2.2; point.normal_z = 2.3;
        point.r = 'r'; point.g = 'g'; point.b = 'b'; point.a = 'a';

        testPC.push_back(point);

        checkTrue(testPC.size() == (size_t) (width*height*2) + 1, "Checking the size");

        ok = true;
        for (int i=0; i<width*height; i++)
        {
            ok &= testPC(i).x == i;
            ok &= testPC(i).y == i + 1;
            ok &= testPC(i).z == i + 2;
            ok &= testPC(i).r == '1';
            ok &= testPC(i).g == '2';
            ok &= testPC(i).b == '3';
            ok &= testPC(i).a == '4';
        }

        checkTrue(ok,"Checking data consistency: part1");

        ok = true;
        for (int i=width*height; i<(2*(width*height)); i++)
        {
            ok &= testPC(i).x == i-(width*height);
            ok &= testPC(i).y == (i-(width*height))*2;
            ok &= testPC(i).z == (i-(width*height))*3;
            ok &= testPC(i).r == 'r';
            ok &= testPC(i).g == 'g';
            ok &= testPC(i).b == 'b';
            ok &= testPC(i).a == 'a';
        }

        checkTrue(ok,"Checking data consistency: part2");

        report(0,"Testing the push_back");

        ok = true;

        ok &= testPC(testPC.size()-1).x - 1.1 < acceptedDiff;
        ok &= testPC(testPC.size()-1).y - 1.2 < acceptedDiff;
        ok &= testPC(testPC.size()-1).z - 1.3 < acceptedDiff;
        ok &= testPC(testPC.size()-1).normal_x - 2.1 < acceptedDiff;
        ok &= testPC(testPC.size()-1).normal_y - 2.2 < acceptedDiff;
        ok &= testPC(testPC.size()-1).normal_z - 2.3 < acceptedDiff;
        ok &= testPC(testPC.size()-1).r == 'r';
        ok &= testPC(testPC.size()-1).g == 'g';
        ok &= testPC(testPC.size()-1).b == 'b';
        ok &= testPC(testPC.size()-1).a == 'a';

        checkTrue(ok,"Checking data consistency");

    }

    virtual void runTests() override
    {
        readWriteMatchTest();
        readWriteMisMatch1Test();
        readWriteMisMatch2Test();
        copyAndAssignmentTest();
        fromExternalTest();
        concatenationTest();
    }
};


static PointCloudTest pointCloudTest_instance;

yarp::os::impl::UnitTest& getPointCloudTest() {
    return pointCloudTest_instance;
}

