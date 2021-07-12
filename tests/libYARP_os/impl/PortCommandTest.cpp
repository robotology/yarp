/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/impl/PortCommand.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/StringInputStream.h>
#include <yarp/os/impl/StreamConnectionReader.h>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os;
using namespace yarp::os::impl;


static std::string humanize(const std::string& txt)
{
    std::string result("");
    for (unsigned int i=0; i<txt.length(); i++) {
        char ch = txt[i];
        if (ch == '\n') {
            result += "\\n";
        } else if (ch == '\r') {
            result += "\\r";
        } else if (ch == '\0') {
            result += "\\0";
        } else {
            result += ch;
        }
    }
    return result;
}

TEST_CASE("os::impl::PortCommandTest", "[yarp::os][yarp::os::impl]")
{

    SECTION("testing text-mode writing...")
    {

        PortCommand cmd1('d', "");;
        BufferedConnectionWriter bw(true);
        cmd1.write(bw);
        CHECK(humanize(bw.toString()) == "d\\r\\n"); // basic data command

        PortCommand cmd2('\0', "/bozo");;
        BufferedConnectionWriter bw2(true);
        cmd2.write(bw2);
        CHECK(humanize(bw2.toString()) == "/bozo\\r\\n"); // connect command
    }

    SECTION("testing text-mode reading...")
    {

        PortCommand cmd;
        StringInputStream sis;
        StreamConnectionReader br;
        sis.add("d\r\n");
        Route route;
        br.reset(sis, nullptr, route, sis.toString().length(), true);
        cmd.read(br);
        CHECK('d' == cmd.getKey()); // basic data command
    }

};
