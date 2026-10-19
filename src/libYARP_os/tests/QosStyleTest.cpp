/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/QosStyle.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::os;

TEST_CASE("os::QosStyleTest", "[yarp::os]")
{
    SECTION("setPacketPriority with a LEVEL")
    {
        QosStyle style;
        CHECK(style.setPacketPriority("LEVEL:LOW"));
        CHECK(style.getPacketPriorityAsTOS() == 40);
        CHECK(style.setPacketPriority("LEVEL:NORM"));
        CHECK(style.getPacketPriorityAsTOS() == 0);
        CHECK(style.setPacketPriority("LEVEL:HIGH"));
        CHECK(style.getPacketPriorityAsTOS() == 144);
        CHECK(style.getPacketPriorityAsLevel() == QosStyle::PacketPriorityHigh);
        CHECK(style.setPacketPriority("LEVEL:CRIT"));
        CHECK(style.getPacketPriorityAsTOS() == 176);
    }

    SECTION("setPacketPriority with a DSCP class")
    {
        QosStyle style;
        CHECK(style.setPacketPriority("DSCP:AF12"));
        CHECK(style.getPacketPriorityAsTOS() == 48);
        CHECK(style.getPacketPriorityAsDSCP() == QosStyle::DSCP_AF12);
        CHECK(style.setPacketPriority("DSCP:EF"));
        CHECK(style.getPacketPriorityAsTOS() == 184);
    }

    SECTION("setPacketPriority with a TOS value")
    {
        QosStyle style;
        CHECK(style.setPacketPriority("TOS:0"));
        CHECK(style.getPacketPriorityAsTOS() == 0);
        CHECK(style.setPacketPriority("TOS:48"));
        CHECK(style.getPacketPriorityAsTOS() == 48);
        CHECK(style.getPacketPriorityAsDSCP() == QosStyle::DSCP_AF12);
        CHECK(style.setPacketPriority("TOS:255"));
        CHECK(style.getPacketPriorityAsTOS() == 255);
    }

    SECTION("setPacketPriority with an invalid string")
    {
        for (const char* priority : {"", "HIGH", "LEVEL:", "LEVEL:FOO", "DSCP:FOO", "FOO:1",
                                     "TOS:", "TOS:abc", "TOS:12abc", "TOS:-1", "TOS:256"}) {
            QosStyle style;
            CHECK_FALSE(style.setPacketPriority(priority));
        }
    }
}
