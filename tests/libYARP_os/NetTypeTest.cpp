/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/NetType.h>
#include <yarp/os/NetInt16.h>
#include <yarp/os/NetInt32.h>
#include <yarp/os/NetFloat64.h>

#include <cstdlib>
#include <cstring>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;

TEST_CASE("os::NetTypeTest", "[yarp::os]")
{

    SECTION("checking cyclic redundancy check is plausible")
    {

        char buf[] = { 1, 2, 3, 4, 5 };
        char buf2[] = { 1, 2, 3, 4, 5 };
        int len = 5;

        unsigned long ct1 = NetType::getCrc(buf, len);
        unsigned long ct2 = NetType::getCrc(buf2, len);
        CHECK(ct1 == ct2); // two identical sequences

        buf[0] = 4;
        ct1 = NetType::getCrc(buf, len);
        CHECK(ct1!=ct2); // two different sequences

        buf2[0] = 4;
        ct2 = NetType::getCrc(buf2, len);
        CHECK(ct1==ct2); // two identical sequences again
    }

    SECTION("checking integer representation")
    {
        union {
            std::int32_t i;
            unsigned char c[sizeof(std::int32_t)];
        } val;
        NetInt32 i = 258;
        memcpy((char*)(&val.i), (char*)&i, sizeof(std::int32_t));
        CHECK(val.c[0] == 2); // first byte ok
        CHECK(val.c[1] == 1); // second byte ok
        CHECK(val.c[2] == 0); // third byte ok
        CHECK(val.c[3] == 0); // fourth byte ok
    }

    SECTION("checking 16-bit integer representation")
    {
        union {
            std::int16_t i;
            unsigned char c[sizeof(std::int16_t)];
        } val;
        NetInt16 i = 258;
        memcpy((char*)(&val.i), (char*)&i, sizeof(std::int16_t));
        CHECK(val.c[0] == 2); // first byte ok
        CHECK(val.c[1] == 1); // second byte ok
    }

    SECTION("checking floating point representation")
    {
        NetFloat64 d = 99;
        unsigned char rpi[8] = {
            110, 134, 27, 240, 249, 33, 9, 64
        };
        for (int i=0; i<8; i++) {
            ((unsigned char*)(&d))[i] = rpi[i];
        }
        CHECK(((double)(d)>3.14)); // pi lower bound
        CHECK(((double)(d)<3.15)); // pi upper bound
    }

}
