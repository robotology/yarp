/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef ISERIALSTEST_H
#define ISERIALSTEST_H

#include <yarp/dev/ISerialDevice.h>
#include <catch2/catch_amalgamated.hpp>

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;

namespace yarp::dev::tests
{
    inline void exec_iSerial_test_1(ISerialDevice* iser)
    {
        {
            size_t flushed = 0;
            ReturnValue ret = iser->flush(flushed);
            CHECK(ret);
            CHECK(flushed == 3);
        }
        {
            size_t flushed = 0;
            ReturnValue ret = iser->flush();
            CHECK(ret);
        }
        {
            std::string str1 = "hello";
            ReturnValue ret = iser->sendString(str1);
            CHECK(ret);
            std::string str2 = "";
            ReturnValue ret2 = iser->sendString(str2);
            CHECK(!ret2);
        }
        {
            std::string str1;
            ReturnValue ret = iser->receiveString(str1);
            CHECK(ret);
            CHECK(str1==std::string("123456789"));
        }
        {
            std::vector<unsigned char> vec = {'A', 'B', 0x00, 'C', 'D'};
            ReturnValue ret = iser->sendBytes(vec);
            CHECK(ret);
        }
        {
            unsigned char mybyte = 'A';
            ReturnValue ret = iser->sendByte(mybyte);
            CHECK(ret);
        }
        {
            unsigned char mybyte = 0;
            ReturnValue ret = iser->receiveByte(mybyte);
            CHECK(ret);
            CHECK(mybyte=='R');
        }
        {
            std::vector<unsigned char> vec;
            ReturnValue ret = iser->receiveBytes(vec,3);
            CHECK(ret);
            CHECK(vec.size()==3);
            CHECK(vec[0] == '0');
            CHECK(vec[1] == '1');
            CHECK(vec[2] == '2');
        }
    }
}

#endif
