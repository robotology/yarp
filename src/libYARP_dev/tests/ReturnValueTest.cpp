/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include <yarp/dev/ReturnValue.h>

#include <catch2/catch_amalgamated.hpp>
#include <harness.h>

using namespace yarp::dev;

ReturnValue test_method1()
{
    return YARP_METHOD_NOT_YET_IMPLEMENTED();
}

ReturnValue test_method2()
{
    return YARP_METHOD_DEPRECATED();
}

TEST_CASE("dev::ReturnValue", "[yarp::dev]")
{
#if !YARP_RETURNVALUE_DISABLE_BOOL_INPUT
    SECTION("test block 1")
    {
        ReturnValue val_f1(false);
        ReturnValue val_f2(false);

        ReturnValue val_t1(true);
        ReturnValue val_t2(true);

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
        ReturnValue val1;
        CHECK(val1 == ReturnValue::return_code::return_value_uninitialized);

        std::string s;
        val1 = ReturnValue::return_code::return_value_ok;
        s = val1.toString();
        CHECK(val1);
        CHECK(s!="unknown");

        val1 = ReturnValue::return_code::return_value_error_generic;
        s = val1.toString();
        CHECK(!val1);
        CHECK(s != "unknown");

        val1 = ReturnValue::return_code::return_value_error_method_failed;
        s = val1.toString();
        CHECK(!val1);
        CHECK(s != "unknown");

        val1 = ReturnValue::return_code::return_value_error_not_implemented_by_device;
        s = val1.toString();
        CHECK(!val1);
        CHECK(s != "unknown");

        val1 = ReturnValue::return_code::return_value_error_nws_nwc_communication_error;
        s = val1.toString();
        CHECK(!val1);
        CHECK(s != "unknown");

        val1 = ReturnValue::return_code::return_value_uninitialized;
        s = val1.toString();
        CHECK(!val1);
        CHECK(s != "unknown");
    }

    SECTION("test block 3")
    {
        ReturnValue val1;
        val1 = ReturnValue::return_code::return_value_ok;
        ReturnValue val2(val1);
        CHECK(val2);
        CHECK(val2 == ReturnValue::return_code::return_value_ok);

        val1 = ReturnValue::return_code::return_value_error_method_failed;
        ReturnValue val3 = val1;
        CHECK(!val3);
        CHECK(val3 == ReturnValue::return_code::return_value_error_method_failed);
    }

#ifndef YARP_RETURNVALUE_DISABLE_BOOL_INPUT
    SECTION("test block 4a")
    {
        ReturnValue val_f1(false);
        ReturnValue val_t1(true);
        bool bool_f1 = val_f1;
        bool bool_t1 = val_t1;
        CHECK (bool_f1 == false);
        CHECK (bool_t1 == true);
    }
#endif

    SECTION("test block 4b")
    {
        ReturnValue val_f2(ReturnValue::return_code::return_value_error_method_failed);
        ReturnValue val_t2(ReturnValue::return_code::return_value_ok);
        bool bool_f2 = val_f2;
        bool bool_t2 = val_t2;
        CHECK(bool_f2 == false);
        CHECK(bool_t2 == true);
    }

#ifndef YARP_RETURNVALUE_DISABLE_BOOL_INPUT
    SECTION("test block 4c")
    {
        ReturnValue val_f1 = false;
        ReturnValue val_t1 = true;
        bool bool_f1 = val_f1;
        bool bool_t1 = val_t1;
        CHECK (bool_f1 == false);
        CHECK (bool_t1 == true);
    }
#endif

    SECTION("test block 5")
    {
        ReturnValue val_f(ReturnValue::return_code::return_value_error_method_failed);
        ReturnValue val_t(ReturnValue::return_code::return_value_ok);
        {
            ReturnValue ret(ReturnValue::return_code::return_value_ok);
            ret&= val_t;
            CHECK(ret == ReturnValue::return_code::return_value_ok);
        }
        {
            ReturnValue ret(ReturnValue::return_code::return_value_ok);
            ret &= val_f;
            CHECK(ret == ReturnValue::return_code::return_value_error_generic);
        }
        {
            ReturnValue ret(ReturnValue::return_code::return_value_error_not_implemented_by_device);
            ret &= val_t;
            CHECK(ret == ReturnValue::return_code::return_value_error_generic);
        }
        {
            ReturnValue ret(ReturnValue::return_code::return_value_error_not_implemented_by_device);
            ret &= val_f;
            CHECK(ret == ReturnValue::return_code::return_value_error_generic);
        }
    }

    SECTION("test block 5b")
    {
        ReturnValue val_f1(ReturnValue::return_code::return_value_error_method_failed);
        ReturnValue val_f2(ReturnValue::return_code::return_value_error_not_implemented_by_device);
        ReturnValue val_t(ReturnValue::return_code::return_value_ok);
        {
            ReturnValue ret;
            ret = val_t && val_t;
            CHECK(ret == ReturnValue::return_code::return_value_ok);
        }
        {
            ReturnValue ret;
            ret = val_f1 && val_f1;
            CHECK(ret == ReturnValue::return_code::return_value_error_method_failed);
        }
        {
            ReturnValue ret;
            ret = val_f2 && val_f2;
            CHECK(ret == ReturnValue::return_code::return_value_error_not_implemented_by_device);
        }
        {
            ReturnValue ret(ReturnValue::return_code::return_value_ok);
            ret &= val_f1 && val_f2;
            CHECK(ret == ReturnValue::return_code::return_value_error_generic);
        }
        {
            ReturnValue ret(ReturnValue::return_code::return_value_ok);
            ret &= val_t && val_f1 ;
            CHECK(ret == ReturnValue::return_code::return_value_error_generic);
        }
        {
            ReturnValue ret(ReturnValue::return_code::return_value_ok);
            ret &= val_f1 && val_f2;
            CHECK(ret == ReturnValue::return_code::return_value_error_generic);
        }
    }

    SECTION("test block 6")
    {
        bool ok;
        ReturnValue val_fi(ReturnValue::return_code::return_value_error_method_failed);
        ReturnValue val_ti(ReturnValue::return_code::return_value_ok);
        ReturnValue val_fo;
        ReturnValue val_to;

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
        CHECK(ret1 == ReturnValue::return_code::return_value_error_not_implemented_by_device);
        CHECK(!(ret1 == ReturnValue::return_code::return_value_ok));

        auto ret2 = test_method2();
        CHECK(ret2 == ReturnValue::return_code::return_value_error_deprecated);
        CHECK(!(ret2 == ReturnValue::return_code::return_value_ok));
    }
}
