/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include <yarp/dev/ControlBoardPid.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;

TEST_CASE("dev::Pid", "[yarp::dev]")
{
    SECTION("test PID serialization")
    {
        yarp::dev::Pid input(1,2,3,4,5,6,7,8,9);

        yarp::dev::Pid output = input;
        CHECK(output == input);

        yarp::dev::Pid test;
        bool b = yarp::os::Portable::copyPortable(input,test);
        CHECK(b == true);
        CHECK(test==input);
    }

    SECTION("test PID serialization 2")
    {
        yarp::dev::Pid input(1,2,3,4,5,6,7,8,9);
        yarp::dev::Pid output;

        yarp::os::Bottle bot;
        bool b = yarp::os::Portable::copyPortable(input,bot);
        CHECK(b == true);
        CHECK(bot.size() == 10);
        std::string sss = bot.toString();
        b = yarp::os::Portable::copyPortable(bot, output);
        CHECK(b == true);
        CHECK(output == input);
    }

    SECTION("test PID serialization 3")
    {
        yarp::dev::Pid test(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0);

        yarp::os::Bottle bot;
        bool b = yarp::os::Portable::copyPortable(test,bot);
        CHECK(b == true);
        std::string ttt;
        ttt = bot.toString();
        size_t sz = bot.size();
        CHECK(sz == 10);

        yarp::dev::Pid output;
        b = yarp::os::Portable::copyPortable(bot,output);
        CHECK(b == true);
        CHECK(output == test);

        yarp::os::Bottle bot2;
        bot2.fromString(ttt);
        yarp::dev::Pid output2;
        b = yarp::os::Portable::copyPortable(bot2,output2);
        CHECK(b == true);
        CHECK(output2 == test);
    }

    SECTION("test PIDWithExtraInfo")
    {
        yarp::dev::Pid inpid {1,2,3,4,5,6};

        yarp::dev::PidWithExtraInfo pidWithInfo;
        pidWithInfo.pidExtraInfo.description.input_data_description = "a";
        pidWithInfo.pidExtraInfo.description.output_data_description = "b";
        pidWithInfo.pidExtraInfo.description.pid_description = "c";
        pidWithInfo.pidExtraInfo.units.units_kp = "1";
        pidWithInfo.pidExtraInfo.units.units_kd = "2";
        pidWithInfo.pidExtraInfo.units.units_ki = "3";
        pidWithInfo.pidExtraInfo.units.units_max_int = "4";
        pidWithInfo.pidExtraInfo.units.units_max_output = "5";
        pidWithInfo.pidExtraInfo.units.units_kff = "6";
        pidWithInfo.pidExtraInfo.units.units_offset = "7";
        pidWithInfo.pidExtraInfo.units.units_scale = "8";
        pidWithInfo.pidExtraInfo.units.units_stiction_up_val = "9";
        pidWithInfo.pidExtraInfo.units.units_stiction_down_val = "0";
        pidWithInfo = inpid;
        yarp::dev::Pid outpid = pidWithInfo.pid;

        CHECK(outpid.kp == 1);
        CHECK(outpid.kd == 2);
        CHECK(outpid.ki == 3);
        CHECK(outpid.max_int == 4);
        CHECK(outpid.scale == 5);
        CHECK(outpid.max_output == 6);

        CHECK(pidWithInfo.pid.kp == 1);
        CHECK(pidWithInfo.pid.kd == 2);
        CHECK(pidWithInfo.pid.ki == 3);
        CHECK(pidWithInfo.pid.max_int == 4);
        CHECK(pidWithInfo.pid.scale == 5);
        CHECK(pidWithInfo.pid.max_output == 6);

        CHECK(pidWithInfo.pidExtraInfo.description.input_data_description == "a");
        CHECK(pidWithInfo.pidExtraInfo.description.output_data_description == "b");
        CHECK(pidWithInfo.pidExtraInfo.description.pid_description == "c");
        CHECK(pidWithInfo.pidExtraInfo.units.units_kp == "1");
        CHECK(pidWithInfo.pidExtraInfo.units.units_kd == "2");
        CHECK(pidWithInfo.pidExtraInfo.units.units_ki == "3");
        CHECK(pidWithInfo.pidExtraInfo.units.units_max_int == "4");
        CHECK(pidWithInfo.pidExtraInfo.units.units_max_output == "5");
        CHECK(pidWithInfo.pidExtraInfo.units.units_kff == "6");
        CHECK(pidWithInfo.pidExtraInfo.units.units_offset == "7");
        CHECK(pidWithInfo.pidExtraInfo.units.units_scale == "8");
        CHECK(pidWithInfo.pidExtraInfo.units.units_stiction_up_val == "9");
        CHECK(pidWithInfo.pidExtraInfo.units.units_stiction_down_val == "0");
    }

    SECTION("test PIDWithExtraInfo Serialization")
    {
        yarp::dev::PidExtraInfo in;
        yarp::dev::PidExtraInfo out;
        bool b = yarp::os::Portable::copyPortable(static_cast<yarp::os::PortWriter&>(in), static_cast<yarp::os::PortReader&>(out));
        CHECK(b == true);
      //  CHECK(out == in);
    }

    SECTION("test PID methods")
    {
        yarp::dev::Pid testpid;
        testpid.setKp(1); CHECK(testpid.kp ==1);
        testpid.setKd(2); CHECK(testpid.kd == 2);
        testpid.setKff(3); CHECK(testpid.kff == 3);
        testpid.setKi(4); CHECK(testpid.ki == 4);
        testpid.setMaxInt(5); CHECK(testpid.max_int == 5);
        testpid.setMaxOut(6); CHECK(testpid.max_output == 6);
        testpid.setOffset(7); CHECK(testpid.offset == 7);
        testpid.setScale(8); CHECK(testpid.scale == 8);
        testpid.setStictionValues(9,10);
        CHECK(testpid.stiction_up_val == 9);
        CHECK(testpid.stiction_down_val==10);

        yarp::dev::Pid testpid2 (testpid);
        CHECK(testpid2 == testpid);

        yarp::dev::Pid emptypid;
        testpid2.clear();
        CHECK(testpid2 == emptypid);

        yarp::dev::Pid testpid3 (1,2,3,4,5,6);
        CHECK(testpid3.kp == 1);
        CHECK(testpid3.kd == 2);
        CHECK(testpid3.ki == 3);
        CHECK(testpid3.max_int == 4);
        CHECK(testpid3.scale == 5);
        CHECK(testpid3.max_output == 6);

        yarp::dev::Pid testpid4 (1,2,3,4,5,6,7,8,9);
        CHECK(testpid4.kp == 1);
        CHECK(testpid4.kd == 2);
        CHECK(testpid4.ki == 3);
        CHECK(testpid4.max_int == 4);
        CHECK(testpid4.scale == 5);
        CHECK(testpid4.max_output == 6);
        CHECK(testpid4.stiction_up_val == 7);
        CHECK(testpid4.stiction_down_val == 8);
        CHECK(testpid4.kff == 9);
    }
}
