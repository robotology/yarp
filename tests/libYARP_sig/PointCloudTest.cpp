/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/sig/PointCloud.h>
#include <yarp/sig/PointCloudUtils.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/NetType.h>
#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/PortReaderBuffer.h>
#include <yarp/os/Time.h>
#include <yarp/sig/Image.h>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::sig;
using namespace yarp::os;

float acceptedDiff = 1e-6f;

TEST_CASE("sig::PointCloudTest", "[yarp::sig]")
{
    Network::setLocalMode(true);

    SECTION("check read/write mismatch.")
    {
        INFO( "Checking DataXYZRGBA sending - Type match");
        BufferedPort< PointCloud<DataXYZRGBA> > outPort;
        Port inPort;
        CHECK(outPort.open("/test/pointcloud/out")); // Opening output port
        CHECK(inPort.open("/test/pointcloud/in")); // Opening input port
        CHECK(NetworkBase::connect(outPort.getName(), inPort.getName())); // "Checking connection"
        PointCloud<DataXYZRGBA>& testPC = outPort.prepare();
        int width  = 100;
        int height = 20;
        testPC.resize(width, height);

        for (int i=0; i<width*height; i++)
        {
            testPC(i).x = static_cast<float>(i);
            testPC(i).y = static_cast<float>(i + 1);
            testPC(i).z = static_cast<float>(i + 2);
            testPC(i).r = '1';
            testPC(i).g = '2';
            testPC(i).b = '3';
            testPC(i).a = '4';
        }

        yarp::os::Time::delay(0.3);

        outPort.write();

        PointCloud<DataXYZRGBA> inCloud;
        inPort.read(inCloud);

        CHECK(inCloud.dataSizeBytes() == testPC.dataSizeBytes()); // Checking size consistency

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

        CHECK(ok); // Checking data validity

        outPort.close();
        inPort.close();

        // TODO : check other data types..
        //        once implemented check toString()

    }

    SECTION("check read/write mismatch 1.")
    {
        INFO("Testing the case in which we receive a structure bigger than the one we expect");
        BufferedPort< PointCloud<DataXYZRGBA> > outPort;
        Port inPort;
        CHECK(outPort.open("/test/pointcloud/out")); // Opening output port
        CHECK(inPort.open("/test/pointcloud/in")); // Opening input port
        CHECK(NetworkBase::connect(outPort.getName(), inPort.getName())); // "Checking connection"
        PointCloud<DataXYZRGBA>& testPC = outPort.prepare();
        int width  = 200;
        int height = 20;
        testPC.resize(width, height);

        for (int i=0; i<width*height; i++)
        {
            testPC(i).x = static_cast<float>(i);
            testPC(i).y = static_cast<float>(i + 1);
            testPC(i).z = static_cast<float>(i + 2);
            testPC(i).r = '1';
            testPC(i).g = '2';
            testPC(i).b = '3';
            testPC(i).a = '4';
        }

        yarp::os::Time::delay(0.3);

        outPort.write();

        PointCloud<DataXYZ> inCloud;
        inPort.read(inCloud);

        CHECK(inCloud.dataSizeBytes() != testPC.dataSizeBytes()); // "Checking size, correctly different"

        bool ok = true;
        for (int i=0; i<width*height; i++)
        {
            ok &= inCloud(i).x == i;
            ok &= inCloud(i).y == i + 1;
            ok &= inCloud(i).z == i + 2;
        }

        CHECK(ok); // Checking data validity

        outPort.close();
        inPort.close();


    }

    SECTION("check read/write mismatch 2.")
    {
        INFO("Testing the case in which we receive a structure smaller than the one we expect");
        BufferedPort< PointCloud<DataXYZNormal> > outPort;
        Port inPort;
        CHECK(outPort.open("/test/pointcloud/out")); // Opening output port
        CHECK(inPort.open("/test/pointcloud/in")); // Opening input port
        CHECK(NetworkBase::connect(outPort.getName(), inPort.getName())); // "Checking connection"
        PointCloud<DataXYZNormal>& testPC = outPort.prepare();
        int width  = 200;
        int height = 42;
        testPC.resize(width, height);

        for (int i=0; i<width*height; i++)
        {
            testPC(i).x = static_cast<float>(i);
            testPC(i).y = static_cast<float>(i + 1);
            testPC(i).z = static_cast<float>(i + 2);
            testPC(i).normal_x = static_cast<float>(i * 2);
            testPC(i).normal_y = static_cast<float>(i * 3);
            testPC(i).normal_z = static_cast<float>(i * 4);
            testPC(i).curvature = static_cast<float>(i * 5);
        }

        yarp::os::Time::delay(0.3);

        outPort.write();

        PointCloud<DataXYZNormalRGBA> inCloud;
        inPort.read(inCloud);

        CHECK(inCloud.dataSizeBytes() == testPC.dataSizeBytes()); // Checking size, equals for the padding

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

        CHECK(ok); // Checking data validity

        outPort.close();
        inPort.close();

    }

    SECTION("check copy and assignment.")
    {
        INFO("Testing the copy constructor with PC of the same type");
        PointCloud<DataXYZRGBA> testPC;
        int width  = 5;
        int height = 5;
        testPC.resize(width, height);

        for (int i=0; i<width*height; i++)
        {
            testPC(i).x = static_cast<float>(i);
            testPC(i).y = static_cast<float>(i + 1);
            testPC(i).z = static_cast<float>(i + 2);
            testPC(i).r = '1';
            testPC(i).g = '2';
            testPC(i).b = '3';
            testPC(i).a = '4';
        }

        PointCloud<DataXYZRGBA> testPC2(testPC);

        CHECK(testPC2.dataSizeBytes() == testPC.dataSizeBytes()); // Checking size

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

        CHECK(ok); // Checking data consistency

        INFO("Testing the copy constructor with PC of different types:");
        INFO("Smaller structure built from bigger");
        PointCloud<DataXYZ> testPC3(testPC);

        CHECK(testPC3.dataSizeBytes() != testPC.dataSizeBytes()); // Checking size, correctly different
        CHECK(testPC3.height() == testPC.height()); // Checking height
        CHECK(testPC3.width() == testPC.width()); // Checking width

        ok = true;
        for (int i=0; i<width*height; i++)
        {
            ok &= testPC3(i).x == i;
            ok &= testPC3(i).y == i + 1;
            ok &= testPC3(i).z == i + 2;
        }

        CHECK(ok); // Checking data consistency

        INFO("Testing the copy constructor with PC of different types:");
        INFO("Bigger structure built from smaller");

        PointCloud<DataXYZNormal> testPC4(testPC3);

        CHECK(testPC4.dataSizeBytes() != testPC3.dataSizeBytes()); // "Checking size, correctly different"
        CHECK(testPC4.height() == testPC3.height()); // Checking height
        CHECK(testPC4.width() == testPC3.width()); // Checking width

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

        CHECK(ok); // Checking data consistency

        INFO("Testing the assignment operator with matching types");

        PointCloud<DataXYZNormal> testPC5 = testPC4;


        CHECK(testPC5.dataSizeBytes() == testPC4.dataSizeBytes()); // Checking size
        CHECK(testPC5.height() == testPC4.height()); // Checking height
        CHECK(testPC5.width() == testPC4.width()); // Checking width

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

        CHECK(ok); // Checking data consistency

        INFO("Testing the copy constructor for the curvature case:");
        INFO("Smaller structure built from bigger");

        PointCloud<DataXYZNormalRGBA> testPC6;
        testPC6.resize(width, height);

        for (int i=0; i<width*height; i++)
        {
            testPC6(i).x = static_cast<float>(i);
            testPC6(i).y = static_cast<float>(i + 1);
            testPC6(i).z = static_cast<float>(i + 2);
            testPC6(i).r = '1';
            testPC6(i).g = '2';
            testPC6(i).b = '3';
            testPC6(i).a = '4';
            testPC6(i).normal_x = static_cast<float>(i*2);
            testPC6(i).normal_y = static_cast<float>(i*3);
            testPC6(i).normal_z = static_cast<float>(i*4);
            testPC6(i).curvature = static_cast<float>(i*5);
        }

        PointCloud<DataXYZNormal> testPC7(testPC6);
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

        CHECK(ok); // Checking data consistency


        INFO("Testing the assignment operator with not matching types");
        INFO("Smaller structure built from bigger");

        PointCloud<DataXYZ> testPC8 = testPC6;


        CHECK(testPC8.dataSizeBytes() != testPC6.dataSizeBytes()); // Checking size, correctly different
        CHECK(testPC8.height() == testPC6.height()); // Checking height
        CHECK(testPC8.width() == testPC6.width()); // Checking width

        ok = true;
        for (int i=0; i<width*height; i++)
        {
            ok &= testPC8(i).x == i;
            ok &= testPC8(i).y == i + 1;
            ok &= testPC8(i).z == i + 2;
        }

        CHECK(ok); // Checking data consistency

        INFO("Testing the assignment operator with not matching types");
        INFO("Bigger structure built from smaller");

        PointCloud<DataXYZNormalRGBA> testPC9 = testPC7;


        CHECK(testPC9.dataSizeBytes() == testPC7.dataSizeBytes()); // Checking size
        CHECK(testPC9.height() == testPC7.height()); // Checking height
        CHECK(testPC9.width() == testPC7.width()); // Checking width

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

        CHECK(ok); // Checking data consistency


    }

    SECTION("Testing the fromExternalPC with PC of the same type")
    {
        PointCloud<DataXYZRGBA> testPC;
        int width  = 32;
        int height = 25;
        testPC.resize(width, height);

        for (int i=0; i<width*height; i++)
        {
            testPC(i).x = static_cast<float>(i);
            testPC(i).y = static_cast<float>(i + 1);
            testPC(i).z = static_cast<float>(i + 2);
            testPC(i).r = '1';
            testPC(i).g = '2';
            testPC(i).b = '3';
            testPC(i).a = '4';
        }

        PointCloud<DataXYZRGBA> testPC2;
        testPC2.fromExternalPC(testPC.getRawData(), PCL_POINT_XYZ_RGBA, width, height);

        CHECK(testPC2.dataSizeBytes() == testPC.dataSizeBytes()); // Checking size

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

        CHECK(ok); // Checking data consistency

        INFO("Testing the fromExternalPC with PC of different types:");
        INFO("Smaller structure built from bigger");
        PointCloud<DataXYZ> testPC3;

        testPC3.fromExternalPC(testPC2.getRawData(), PCL_POINT_XYZ_RGBA, width, height);

        CHECK(testPC3.dataSizeBytes() != testPC2.dataSizeBytes()); // Checking size, correctly different
        CHECK(testPC3.height() == testPC2.height()); // Checking height
        CHECK(testPC3.width() == testPC2.width()); // Checking width

        ok = true;
        for (int i=0; i<width*height; i++)
        {
            ok &= testPC3(i).x == i;
            ok &= testPC3(i).y == i + 1;
            ok &= testPC3(i).z == i + 2;
        }

        CHECK(ok); // Checking data consistency

        INFO("Testing the fromExternalPC with PC of different types:");
        INFO("Bigger structure built from smaller");

        PointCloud<DataXYZNormal> testPC4(testPC3);

        testPC4.fromExternalPC(testPC3.getRawData(), PCL_POINT_XYZ, width, height);

        CHECK(testPC4.dataSizeBytes() != testPC3.dataSizeBytes()); // Checking size, correctly different
        CHECK(testPC4.height() == testPC3.height()); // Checking height
        CHECK(testPC4.width() == testPC3.width()); // Checking width

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

        CHECK(ok); // Checking data consistency
    }

    SECTION("Testing the operator+ with PC of the same type")
    {
        PointCloud<DataXYZNormalRGBA> testPC;
        PointCloud<DataXYZNormalRGBA> testPC2;
        int width  = 35;
        int height = 62;
        testPC.resize(width, height);
        testPC2.resize(width, height);

        for (int i=0; i<width*height; i++)
        {
            testPC(i).x = static_cast<float>(i);     testPC2(i).x = static_cast<float>(i);
            testPC(i).y = static_cast<float>(i + 1); testPC2(i).y = static_cast<float>(i*2);
            testPC(i).z = static_cast<float>(i + 2); testPC2(i).z = static_cast<float>(i*3);
            testPC(i).r = '1';                       testPC2(i).r = 'r';
            testPC(i).g = '2';                       testPC2(i).g = 'g';
            testPC(i).b = '3';                       testPC2(i).b = 'b';
            testPC(i).a = '4';                       testPC2(i).a = 'a';
        }

        PointCloud<DataXYZNormalRGBA> sumPC;
        sumPC = testPC + testPC2;

        CHECK(sumPC.size() == (size_t) (width*height*2)); // Checking the size

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


        CHECK(ok); // Checking data consistency: part1

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

        CHECK(ok); // Checking data consistency: part2

        INFO("Testing the operator+= with PC of the same type");


        testPC += testPC2;

        DataXYZNormalRGBA point;
        point.x = 1.1f; point.y = 1.2f; point.z = 1.3f;
        point.normal_x = 2.1f; point.normal_y = 2.2f; point.normal_z = 2.3f;
        point.r = 'r'; point.g = 'g'; point.b = 'b'; point.a = 'a';

        testPC.push_back(point);

        CHECK(testPC.size() == (size_t) (width*height*2) + 1); // Checking the size

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

        CHECK(ok); // Checking data consistency: part1

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

        CHECK(ok); // Checking data consistency: part2

        INFO("Testing the push_back");

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

        CHECK(ok); // Checking data consistency

    }

    SECTION("check to/from bottle")
    {
       {
            INFO("Testing fromBottle(toBottle) XYZ_NORMAL_RGBA");
            PointCloud<DataXYZNormalRGBA> testPC;
            size_t width = 21; size_t height = 32;
            testPC.resize(width, height);
            for (size_t i=0; i<width*height; i++)
            {
                testPC(i).x = static_cast<float>(i);
                testPC(i).y = static_cast<float>(i + 1);
                testPC(i).z = static_cast<float>(i + 2);
                testPC(i).normal_x = static_cast<float>(i*2);
                testPC(i).normal_y = static_cast<float>(i*3);
                testPC(i).normal_z = static_cast<float>(i*4);
                testPC(i).curvature = static_cast<float>(i*5);
                testPC(i).r = 'r';
                testPC(i).g = 'g';
                testPC(i).b = 'b';
                testPC(i).a = 'a';
            }
            PointCloud<DataXYZNormalRGBA> testPC2;
            Bottle bt = testPC.toBottle();
            testPC2.fromBottle(bt);
            CHECK(testPC.width() ==  testPC2.width()); // Checking width
            CHECK(testPC.height() ==  testPC2.height()); // Checking height

            bool ok = true;

            for (size_t i=0; i<width*height; i++)
            {
                ok &= testPC2(i).x == i;
                ok &= testPC2(i).y == i + 1;
                ok &= testPC2(i).z == i + 2;
                ok &= testPC2(i).normal_x == i*2;
                ok &= testPC2(i).normal_y == i*3;
                ok &= testPC2(i).normal_z == i*4;
                ok &= testPC2(i).curvature ==i*5;
                ok &= testPC2(i).r == 'r';
                ok &= testPC2(i).g == 'g';
                ok &= testPC2(i).b == 'b';
                ok &= testPC2(i).a == 'a';
            }
            CHECK(ok); // Checking data consistency

            PointCloud<DataXYZNormal> testPCfail;
            CHECK(!testPCfail.fromBottle(bt)); // from bottle correctly failing... type mismatch
        }

        {
            INFO("Testing fromBottle(toBottle) XYZ_NORMAL");
            PointCloud<DataXYZNormal> testPC;
            size_t width = 21; size_t height = 32;
            testPC.resize(width, height);
            for (size_t i=0; i<width*height; i++)
            {
                testPC(i).x = static_cast<float>(i);
                testPC(i).y = static_cast<float>(i + 1);
                testPC(i).z = static_cast<float>(i + 2);
                testPC(i).normal_x = static_cast<float>(i*2);
                testPC(i).normal_y = static_cast<float>(i*3);
                testPC(i).normal_z = static_cast<float>(i*4);
                testPC(i).curvature = static_cast<float>(i*5);
            }
            PointCloud<DataXYZNormal> testPC2;
            Bottle bt = testPC.toBottle();
            testPC2.fromBottle(bt);
            CHECK(testPC.width() ==  testPC2.width()); // Checking width
            CHECK(testPC.height() ==  testPC2.height()); // Checking height

            bool ok = true;

            for (size_t i=0; i<width*height; i++)
            {
                ok &= testPC2(i).x == i;
                ok &= testPC2(i).y == i + 1;
                ok &= testPC2(i).z == i + 2;
                ok &= testPC2(i).normal_x == i*2;
                ok &= testPC2(i).normal_y == i*3;
                ok &= testPC2(i).normal_z == i*4;
                ok &= testPC2(i).curvature ==i*5;
            }
            CHECK(ok); // Checking data consistency
        }

        {
            INFO("Testing fromBottle(toBottle) XYZ_RGBA");
            PointCloud<DataXYZRGBA> testPC;
            size_t width = 21; size_t height = 32;
            testPC.resize(width, height);
            for (size_t i=0; i<width*height; i++)
            {
                testPC(i).x = static_cast<float>(i);
                testPC(i).y = static_cast<float>(i + 1);
                testPC(i).z = static_cast<float>(i + 2);
                testPC(i).r = 'r';
                testPC(i).g = 'g';
                testPC(i).b = 'b';
                testPC(i).a = 'a';
            }
            PointCloud<DataXYZRGBA> testPC2;
            Bottle bt = testPC.toBottle();
            testPC2.fromBottle(bt);
            CHECK(testPC.width() ==  testPC2.width()); // Checking width
            CHECK(testPC.height() ==  testPC2.height()); // Checking height

            bool ok = true;

            for (size_t i=0; i<width*height; i++)
            {
                ok &= testPC2(i).x == i;
                ok &= testPC2(i).y == i + 1;
                ok &= testPC2(i).z == i + 2;
                ok &= testPC2(i).r == 'r';
                ok &= testPC2(i).g == 'g';
                ok &= testPC2(i).b == 'b';
                ok &= testPC2(i).a == 'a';
            }
            CHECK(ok); // Checking data consistency
        }

        {
            INFO("Testing fromBottle(toBottle) XYZ");
            PointCloud<DataXYZ> testPC;
            size_t width = 21; size_t height = 32;
            testPC.resize(width, height);
            for (size_t i=0; i<width*height; i++)
            {
                testPC(i).x = static_cast<float>(i);
                testPC(i).y = static_cast<float>(i + 1);
                testPC(i).z = static_cast<float>(i + 2);
            }
            PointCloud<DataXYZ> testPC2;
            Bottle bt = testPC.toBottle();
            testPC2.fromBottle(bt);
            CHECK(testPC.width() ==  testPC2.width()); // Checking width
            CHECK(testPC.height() ==  testPC2.height()); // Checking height

            bool ok = true;

            for (size_t i=0; i<width*height; i++)
            {
                ok &= testPC2(i).x == i;
                ok &= testPC2(i).y == i + 1;
                ok &= testPC2(i).z == i + 2;
            }
            CHECK(ok); // Checking data consistency
        }

        {
            INFO("Testing fromBottle(toBottle) NORMAL");
            PointCloud<DataNormal> testPC;
            size_t width = 3;
            size_t height = 3;
            testPC.resize(width, height);
            for (size_t i=0; i<width*height; i++)
            {
                testPC(i).normal_x = static_cast<float>(i*2);
                testPC(i).normal_y = static_cast<float>(i*3);
                testPC(i).normal_z = static_cast<float>(i*4);
                testPC(i).curvature = static_cast<float>(i*5);
            }
            PointCloud<DataNormal> testPC2;
            Bottle bt = testPC.toBottle();
            testPC2.fromBottle(bt);
            CHECK(testPC.width() ==  testPC2.width()); // Checking width
            CHECK(testPC.height() ==  testPC2.height()); // Checking height

            bool ok = true;

            for (size_t i=0; i<width*height; i++)
            {
                ok &= testPC2(i).normal_x == i*2;
                ok &= testPC2(i).normal_y == i*3;
                ok &= testPC2(i).normal_z == i*4;
                ok &= testPC2(i).curvature ==i*5;
            }
            CHECK(ok); // Checking data consistency
        }

    }

    SECTION("check read/write from bottle")
    {
        {
            INFO("Testing readWriteFromBottle(toBottle) XYZ_NORMAL_RGBA");
            PointCloud<DataXYZNormalRGBA> testPC;
            Port outPort;
            Port inPort;
            CHECK(outPort.open("/test/pointcloud/out")); // Opening output port
            CHECK(inPort.open("/test/pointcloud/in")); // Opening input port
            CHECK(NetworkBase::connect(outPort.getName(), inPort.getName())); // "Checking connection"
            size_t width = 21; size_t height = 32;
            testPC.resize(width, height);
            for (size_t i=0; i<width*height; i++)
            {
                testPC(i).x = static_cast<float>(i);
                testPC(i).y = static_cast<float>(i + 1);
                testPC(i).z = static_cast<float>(i + 2);
                testPC(i).normal_x = static_cast<float>(i*2);
                testPC(i).normal_y = static_cast<float>(i*3);
                testPC(i).normal_z = static_cast<float>(i*4);
                testPC(i).curvature = static_cast<float>(i*5);
                testPC(i).r = 'r';
                testPC(i).g = 'g';
                testPC(i).b = 'b';
                testPC(i).a = 'a';
            }

            Bottle outBt = testPC.toBottle();
            outPort.enableBackgroundWrite(true);
            CHECK(outPort.write(outBt)); // Checking write
            yarp::os::Time::delay(0.2);
            Bottle inBt;
            CHECK(inPort.read(inBt)); // Checking read
            PointCloud<DataXYZNormalRGBA> testPC2;
            testPC2.fromBottle(inBt);
            CHECK(testPC.width() ==  testPC2.width()); // Checking width
            CHECK(testPC.height() ==  testPC2.height()); // Checking height

            bool ok = true;

            for (size_t i=0; i<width*height; i++)
            {
                ok &= testPC2(i).x == i;
                ok &= testPC2(i).y == i + 1;
                ok &= testPC2(i).z == i + 2;
                ok &= testPC2(i).normal_x == i*2;
                ok &= testPC2(i).normal_y == i*3;
                ok &= testPC2(i).normal_z == i*4;
                ok &= testPC2(i).curvature ==i*5;
                ok &= testPC2(i).r == 'r';
                ok &= testPC2(i).g == 'g';
                ok &= testPC2(i).b == 'b';
                ok &= testPC2(i).a == 'a';
            }
            CHECK(ok); // Checking data consistency

            PointCloud<DataXYZNormal> testPCfail;
            CHECK(!testPCfail.fromBottle(inBt)); // from bottle correctly failing... type mismatch
        }
        {
            INFO("Testing readWriteFromBottle(toBottle) XYZ_NORMAL");
            PointCloud<DataXYZNormal> testPC;
            Port outPort;
            Port inPort;
            CHECK(outPort.open("/test/pointcloud/out")); // Opening output port
            CHECK(inPort.open("/test/pointcloud/in")); // Opening input port
            CHECK(NetworkBase::connect(outPort.getName(), inPort.getName())); // "Checking connection"
            size_t width = 21; size_t height = 32;
            testPC.resize(width, height);
            for (size_t i=0; i<width*height; i++)
            {
                testPC(i).x = static_cast<float>(i);
                testPC(i).y = static_cast<float>(i + 1);
                testPC(i).z = static_cast<float>(i + 2);
                testPC(i).normal_x = static_cast<float>(i*2);
                testPC(i).normal_y = static_cast<float>(i*3);
                testPC(i).normal_z = static_cast<float>(i*4);
                testPC(i).curvature = static_cast<float>(i*5);
            }

            Bottle outBt = testPC.toBottle();
            outPort.enableBackgroundWrite(true);
            CHECK(outPort.write(outBt)); // Checking write
            yarp::os::Time::delay(0.2);
            Bottle inBt;
            CHECK(inPort.read(inBt)); // Checking read
            PointCloud<DataXYZNormal> testPC2;
            testPC2.fromBottle(inBt);
            CHECK(testPC.width() ==  testPC2.width()); // Checking width
            CHECK(testPC.height() ==  testPC2.height()); // Checking height

            bool ok = true;

            for (size_t i=0; i<width*height; i++)
            {
                ok &= testPC2(i).x == i;
                ok &= testPC2(i).y == i + 1;
                ok &= testPC2(i).z == i + 2;
                ok &= testPC2(i).normal_x == i*2;
                ok &= testPC2(i).normal_y == i*3;
                ok &= testPC2(i).normal_z == i*4;
                ok &= testPC2(i).curvature ==i*5;
            }
            CHECK(ok); // Checking data consistency

            PointCloud<DataXYZ> testPCfail;
            CHECK(!(testPCfail.fromBottle(inBt))); //  from bottle correctly failing... type mismatch
        }


        {
            INFO("Testing readWriteFromBottle(toBottle) XYZ");
            PointCloud<DataXYZ> testPC;
            Port outPort;
            Port inPort;
            CHECK(outPort.open("/test/pointcloud/out")); // Opening output port
            CHECK(inPort.open("/test/pointcloud/in")); // Opening input port
            CHECK(NetworkBase::connect(outPort.getName(), inPort.getName())); // "Checking connection"
            size_t width = 21; size_t height = 32;
            testPC.resize(width, height);
            for (size_t i=0; i<width*height; i++)
            {
                testPC(i).x = static_cast<float>(i);
                testPC(i).y = static_cast<float>(i + 1);
                testPC(i).z = static_cast<float>(i + 2);
            }

            Bottle outBt = testPC.toBottle();
            outPort.enableBackgroundWrite(true);
            CHECK(outPort.write(outBt)); // Checking write
            yarp::os::Time::delay(0.2);
            Bottle inBt;
            CHECK(inPort.read(inBt)); // Checking read
            PointCloud<DataXYZ> testPC2;
            testPC2.fromBottle(inBt);
            CHECK(testPC.width() ==  testPC2.width()); // Checking width
            CHECK(testPC.height() ==  testPC2.height()); // Checking height

            bool ok = true;

            for (size_t i=0; i<width*height; i++)
            {
                ok &= testPC2(i).x == i;
                ok &= testPC2(i).y == i + 1;
                ok &= testPC2(i).z == i + 2;
            }
            CHECK(ok); // Checking data consistency

            PointCloud<DataNormal> testPCfail;
            CHECK(!(testPCfail.fromBottle(inBt))); // from bottle correctly failing... type mismatch
        }
    }

    SECTION("Testing depthToPC BGRA")
    {
        ImageOf<PixelFloat> depth;
        size_t width{320};
        size_t height{240};
        depth.resize(width, height);
        IntrinsicParams intp;

        auto pc = utils::depthToPC(depth, intp);
        CHECK(pc.width() == depth.width()); // Checking PC width
        CHECK(pc.height() == depth.height()); // Checking PC height

        ImageOf<PixelBgra> color;
        color.resize(width, height);
        auto pcCol = utils::depthRgbToPC<DataXYZRGBA, PixelBgra>(depth, color, intp);
        CHECK(pcCol.width() == depth.width()); // Checking PC width
        CHECK(pcCol.height() == depth.height()); // Checking PC height
    }

    SECTION("Testing depthToPC BGR")
    {
        ImageOf<PixelFloat> depth;
        size_t width{320};
        size_t height{240};
        depth.resize(width, height);
        IntrinsicParams intp;

        auto pc = utils::depthToPC(depth, intp);
        CHECK(pc.width() == depth.width()); // Checking PC width
        CHECK(pc.height() == depth.height()); // Checking PC height

        ImageOf<PixelBgr> color;
        color.resize(width, height);
        auto pcCol = utils::depthRgbToPC<DataXYZRGBA, PixelBgr>(depth, color, intp);
        CHECK(pcCol.width() == depth.width()); // Checking PC width
        CHECK(pcCol.height() == depth.height()); // Checking PC height
    }

    SECTION("Testing depthToPC with ROI")
    {
        ImageOf<PixelFloat> depth;
        size_t width{320};
        size_t height{240};
        depth.resize(width, height);
        IntrinsicParams intp;
        utils::PCL_ROI roi {100, 300, 150, 200}; // {min_x, max_x, min_y, max_y}
        size_t step_x = 2; // (300 - 100) / 2 = 100 pixels wide
        size_t step_y = 5; // (200 - 150) / 5 = 10 pixels high

        auto pc = utils::depthToPC(depth, intp, roi, step_x, step_y);
        CHECK(pc.width() == (roi.max_x - roi.min_x) / step_x); // Checking PC width
        CHECK(pc.height() == (roi.max_y - roi.min_y) / step_y); // Checking PC height
    }

    SECTION("Testing move semantics")
    {
        INFO("Testing the copy constructor with PC of the same type");
        PointCloud<DataXYZRGBA> testPC;
        int width  = 5;
        int height = 5;
        testPC.resize(width, height);

        for (int i=0; i<width*height; i++)
        {
            testPC(i).x = static_cast<float>(i);
            testPC(i).y = static_cast<float>(i + 1);
            testPC(i).z = static_cast<float>(i + 2);
            testPC(i).r = '1';
            testPC(i).g = '2';
            testPC(i).b = '3';
            testPC(i).a = '4';
        }

        auto size_bytes = testPC.dataSizeBytes();

        PointCloud<DataXYZRGBA> testPC2(std::move(testPC));

        CHECK(testPC2.dataSizeBytes() == size_bytes); // Checking size

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

        CHECK(ok); // Checking data consistency

        PointCloud<DataXYZRGBA> testPC3 = std::move(testPC2);
        CHECK(testPC3.dataSizeBytes() == size_bytes); // Checking size

        ok = true;
        for (int i=0; i<width*height; i++)
        {
            ok &= testPC3(i).x == i;
            ok &= testPC3(i).y == i + 1;
            ok &= testPC3(i).z == i + 2;
            ok &= testPC3(i).r == '1';
            ok &= testPC3(i).g == '2';
            ok &= testPC3(i).b == '3';
            ok &= testPC3(i).a == '4';
        }

        CHECK(ok); // Checking data consistency
    }
    Network::setLocalMode(false);
}
