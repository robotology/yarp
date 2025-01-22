/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include <yarp/dev/ReturnValue.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;

yarp_ret_value test_method1()
{
    return YARP_METHOD_NOT_YET_IMPLEMENTED();
}

yarp_ret_value test_method2()
{
    return YARP_METHOD_DEPRECATED();
}

TEST_CASE("dev::ReturnValue", "[yarp::dev]")
{
#ifndef DISABLE_BOOL_INPUT
    SECTION("test block 1")
    {
        yarp_ret_value val_f1(false);
        yarp_ret_value val_f2(false);

        yarp_ret_value val_t1(true);
        yarp_ret_value val_t2(true);

        CHECK(val_f1 == val_f2);
        CHECK(val_t1 == val_t2);
        CHECK(val_f1 != val_t1);
        CHECK(val_t1);
        CHECK(!val_f1);

        std::string sf = val_f1.toString();
        std::string st = val_t1.toString();
    }
#endif

    SECTION("test block 2")
    {
        yarp_ret_value val1;
        CHECK(val1 == yarp_ret_value::return_code::return_value_unitialized);

        std::string s;
        val1 = yarp_ret_value::return_code::return_value_ok;
        s = val1.toString();
        CHECK(val1);
        CHECK(s!="unknown");

        val1 = yarp_ret_value::return_code::return_value_error_generic;
        s = val1.toString();
        CHECK(!val1);
        CHECK(s != "unknown");

        val1 = yarp_ret_value::return_code::return_value_error_method_failed;
        s = val1.toString();
        CHECK(!val1);
        CHECK(s != "unknown");

        val1 = yarp_ret_value::return_code::return_value_error_not_implemented_by_device;
        s = val1.toString();
        CHECK(!val1);
        CHECK(s != "unknown");

        val1 = yarp_ret_value::return_code::return_value_error_nws_nwc_communication_error;
        s = val1.toString();
        CHECK(!val1);
        CHECK(s != "unknown");

        val1 = yarp_ret_value::return_code::return_value_unitialized;
        s = val1.toString();
        CHECK(!val1);
        CHECK(s != "unknown");
    }

    SECTION("test block 3")
    {
        yarp_ret_value val1;
        val1 = yarp_ret_value::return_code::return_value_ok;
        yarp_ret_value val2(val1);
        CHECK(val2);
        CHECK(val2 == yarp_ret_value::return_code::return_value_ok);

        val1 = yarp_ret_value::return_code::return_value_error_method_failed;
        yarp_ret_value val3 = val1;
        CHECK(!val3);
        CHECK(val3 == yarp_ret_value::return_code::return_value_error_method_failed);
    }

#ifndef DISABLE_BOOL_INPUT
    SECTION("test block 4a")
    {
        yarp_ret_value val_f1(false);
        yarp_ret_value val_t1(true);
        bool bool_f1 = val_f1;
        bool bool_t1 = val_t1;
        CHECK (bool_f1 == false);
        CHECK (bool_t1 == true);
    }
#endif

    SECTION("test block 4b")
    {
        yarp_ret_value val_f2(yarp_ret_value::return_code::return_value_error_method_failed);
        yarp_ret_value val_t2(yarp_ret_value::return_code::return_value_ok);
        bool bool_f2 = val_f2;
        bool bool_t2 = val_t2;
        CHECK(bool_f2 == false);
        CHECK(bool_t2 == true);
    }

    SECTION("test block 5")
    {
        yarp_ret_value val_f(yarp_ret_value::return_code::return_value_error_method_failed);
        yarp_ret_value val_t(yarp_ret_value::return_code::return_value_ok);
        {
            yarp_ret_value ret(yarp_ret_value::return_code::return_value_ok);
            ret&= val_t;
            CHECK(ret == yarp_ret_value::return_code::return_value_ok);
        }
        {
            yarp_ret_value ret(yarp_ret_value::return_code::return_value_ok);
            ret &= val_f;
            CHECK(ret == yarp_ret_value::return_code::return_value_error_generic);
        }
        {
            yarp_ret_value ret(yarp_ret_value::return_code::return_value_error_not_implemented_by_device);
            ret &= val_t;
            CHECK(ret == yarp_ret_value::return_code::return_value_error_generic);
        }
        {
            yarp_ret_value ret(yarp_ret_value::return_code::return_value_error_not_implemented_by_device);
            ret &= val_f;
            CHECK(ret == yarp_ret_value::return_code::return_value_error_generic);
        }
    }

    SECTION("test block 5b")
    {
        yarp_ret_value val_f1(yarp_ret_value::return_code::return_value_error_method_failed);
        yarp_ret_value val_f2(yarp_ret_value::return_code::return_value_error_not_implemented_by_device);
        yarp_ret_value val_t(yarp_ret_value::return_code::return_value_ok);
        {
            yarp_ret_value ret;
            ret = val_t && val_t;
            CHECK(ret == yarp_ret_value::return_code::return_value_ok);
        }
        {
            yarp_ret_value ret;
            ret = val_f1 && val_f1;
            CHECK(ret == yarp_ret_value::return_code::return_value_error_method_failed);
        }
        {
            yarp_ret_value ret;
            ret = val_f2 && val_f2;
            CHECK(ret == yarp_ret_value::return_code::return_value_error_not_implemented_by_device);
        }
        {
            yarp_ret_value ret(yarp_ret_value::return_code::return_value_ok);
            ret &= val_f1 && val_f2;
            CHECK(ret == yarp_ret_value::return_code::return_value_error_generic);
        }
        {
            yarp_ret_value ret(yarp_ret_value::return_code::return_value_ok);
            ret &= val_t && val_f1 ;
            CHECK(ret == yarp_ret_value::return_code::return_value_error_generic);
        }
        {
            yarp_ret_value ret(yarp_ret_value::return_code::return_value_ok);
            ret &= val_f1 && val_f2;
            CHECK(ret == yarp_ret_value::return_code::return_value_error_generic);
        }
    }

    SECTION("test block 6")
    {
        bool ok;
        yarp_ret_value val_fi(yarp_ret_value::return_code::return_value_error_method_failed);
        yarp_ret_value val_ti(yarp_ret_value::return_code::return_value_ok);
        yarp_ret_value val_fo;
        yarp_ret_value val_to;

        ok = yarp::os::Portable::copyPortable(val_fi,val_fo);
        CHECK(ok);
        CHECK(val_fi == val_fo);

        ok = yarp::os::Portable::copyPortable(val_ti, val_to);
        CHECK(ok);
        CHECK(val_ti == val_to);
    }

    SECTION("test block 7")
    {
        auto ret1 = test_method1();
        CHECK(ret1 == yarp_ret_value::return_code::return_value_error_not_implemented_by_device);
        CHECK(!(ret1 == yarp_ret_value::return_code::return_value_ok));

        auto ret2 = test_method2();
        CHECK(ret2 == yarp_ret_value::return_code::return_value_error_deprecated);
        CHECK(!(ret2 == yarp_ret_value::return_code::return_value_ok));
    }
}
