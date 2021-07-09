/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/sig/Vector.h>

#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Port.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Time.h>

#include <algorithm>

#include <yarp/gsl/impl/gsl_structs.h>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os::impl;
using namespace yarp::os;
using namespace yarp::sig;


TEST_CASE("sig::VectorOfTest", "[yarp::sig]")
{
    NetworkBase::setLocalMode(true);

    SECTION("Check send and receive integers")
    {
        INFO("check VectorO<int> send receive");

        {
            Port portIn;
            Port portOut;

            portOut.open("/harness_sig/vtest/o");
            portIn.open("/harness_sig/vtest/i");

            Network::connect("/harness_sig/vtest/o", "/harness_sig/vtest/i");

            portOut.enableBackgroundWrite(true);


            VectorOf<int> vector;
            vector.resize(10);
            for (unsigned int k = 0; k < vector.size(); k++)
            {
                vector[k] = k;
            }

            bool success = true;
            portOut.write(vector);

            VectorOf<int> tmp;
            portIn.read(tmp);

            //compare vector and tmp
            if (tmp.size() != vector.size())
            {
                success = false;
            }
            else
            {
                for (unsigned int k = 0; k < vector.size(); k++)
                {
                    if (tmp[k] != vector[k])
                        success = false;
                }
            }

            CHECK(success); // VectorOf<int> was sent and received correctly
            portOut.interrupt();
            portOut.close();
            portIn.interrupt();
            portIn.close();
        }

        INFO("check VectorOf<int> bottle compatibility");
        {
            //write the same vector again and receive it as a bottle
            Port portIn;
            Port portOut;
            bool success = true;

            portOut.open("/harness_sig/vtest/o");
            portIn.open("/harness_sig/vtest/i");

            Network::connect("/harness_sig/vtest/o", "/harness_sig/vtest/i");

            portOut.enableBackgroundWrite(true);


            VectorOf<int> vector;
            vector.resize(10);
            for (unsigned int k = 0; k < vector.size(); k++)
            {
                vector[k] = k;
            }
            portOut.write(vector);
            Bottle tmp2;
            success = portIn.read(tmp2);
            CHECK(success); // correctly read from the port

            //compare vector and tmp
            success = true;
            if (tmp2.size() != vector.size())
            {
                success = false;
            }
            else
            {
                for (unsigned int k = 0; k < vector.size(); k++)
                {
                    if (tmp2.get(k).asInt32() != vector[k])
                        success = false;
                }
            }

            CHECK(success); // VectorOf<int> was received correctly in a Bottle
            portOut.interrupt();
            portOut.close();
            portIn.interrupt();
            portIn.close();
        }
    }

    SECTION("check toString")
    {
        {
            INFO("testing toString int");
            bool ok = true;
            VectorOf<int> vec;
            std::string strToCheck = "0 1 2 3 4 5 6 7 8 9";
            for (size_t i=0; i<10; i++)
            {
                vec.push_back(i);
            }

            ok = vec.toString() == strToCheck;
            CHECK(ok); // string correctly formatted
        }

        {
            INFO("testing toString double");
            bool ok = true;
            VectorOf<double> vec;
            std::string strToCheck = " 0.000000\t 1.000000\t 2.000000\t 3.000000\t 4.000000\t"
                                     " 5.000000\t 6.000000\t 7.000000\t 8.000000\t 9.000000";
            for (size_t i=0; i<10; i++)
            {
                vec.push_back(i);
            }

            ok = vec.toString() == strToCheck;
            CHECK(ok); // string correctly formatted
        }

    }

    SECTION("Checking the functionalities of the initializer list constructor")
    {
        VectorOf<int> v{1, 2, 3};
        CHECK(v.size() == (size_t) 3); // Checking size

        CHECK(v[0] == 1); // Checking data consistency
        CHECK(v[1] == 2); // Checking data consistency
        CHECK(v[2] == 3); // Checking data consistency
    }

    SECTION("Checking the the for range based")
    {
        VectorOf<int> v{0,1,2,3,4};
        int i = 0;
        for(const auto& el:v) {
            CHECK(el == i); // Checking data consistency
            i++;
        }
        INFO("Checking the std::transform");
        std::transform(v.begin(), v.end(), v.begin(), [](int i) { return i*2; });
        // Checking data consistency
        CHECK(v[0] == 0);
        CHECK(v[1] == 2);
        CHECK(v[2] == 4);
        CHECK(v[3] == 6);
        CHECK(v[4] == 8);
    }

    SECTION("Checking reserve()")
    {
        VectorOf<int> v(0);
        CHECK(v.size() == (size_t) 0); // Checking size() after constructor
        CHECK(v.capacity() == (size_t) 0); // Checking memory allocated after constructor
        v.push_back(1);
        CHECK(v[0] == 1); // Checking data consistency
        CHECK(v.size() == (size_t) 1); // Checking size() after push_back
        CHECK(v.capacity() == (size_t) 1); // Checking capacity() after push_back
        v.reserve(10);
        CHECK(v[0] == 1); // Checking data consistency
        CHECK(v.size() == (size_t) 1); // The memory has been allocated but the vector is empty
        CHECK(v.capacity() >= (size_t) 10); // Checking memory allocated
        v.push_back(2);
        CHECK(v[0] == 1); // Checking data consistency
        CHECK(v[1] == 2); // Checking data consistency
        CHECK(v.size() == (size_t) 2); // Checking size() after push_back
        CHECK(v.capacity() >= (size_t) 10); // Checking capacity() after push_back
        v.resize(11);
        CHECK(v[0] == 1); // Checking data consistency
        CHECK(v[1] == 2); // Checking data consistency
        CHECK(v.size() == (size_t) 11); // Checking size() after resize()
        CHECK(v.capacity() >= (size_t) 11); // Checking size() after resize()
        v.resize(1);
        CHECK(v[0] == 1); // Checking data consistency
        CHECK(v.size() == (size_t) 1); // Checking size() after push_back
        CHECK(v.capacity() >= (size_t) 11); // Checking capacity() after push_back
    }

    NetworkBase::setLocalMode(false);
}
