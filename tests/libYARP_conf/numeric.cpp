/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/conf/numeric.h>

#include <catch.hpp>
#include <harness.h>


namespace num = yarp::conf::numeric;
using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;
using std::int8_t;
using std::int16_t;
using std::int32_t;
using std::int64_t;
using yarp::conf::float32_t;
using yarp::conf::float64_t;
#if defined(YARP_HAS_FLOAT128_T)
using yarp::conf::float128_t;
#endif


TEST_CASE("conf::numeric", "[yarp::conf]")
{
    SECTION("Sanity check on floating point types size")
    {
        CHECK(sizeof(float32_t) == 4);
        CHECK(sizeof(float64_t) == 8);
#if defined(YARP_HAS_FLOAT128_T)
        CHECK(sizeof(float128_t) == 16);
#endif
    }


    SECTION("Test num::to_string<bool>")
    {
        CHECK(num::to_string<bool>(false) == "false");
        CHECK(num::to_string<bool>(true) == "true");
    }

    SECTION("Test num::to_string<uint8_t>")
    {
        CHECK(num::to_string<uint8_t>(0) == "0");
        CHECK(num::to_string<uint8_t>(1) == "1");
        CHECK(num::to_string<uint8_t>(2) == "2");
        CHECK(num::to_string<uint8_t>(std::numeric_limits<int8_t>::max()) == "127");
        CHECK(num::to_string<uint8_t>(std::numeric_limits<uint8_t>::max()) == "255");
    }

    SECTION("Test num::to_hex_string<uint8_t>")
    {
        CHECK(num::to_hex_string<uint8_t>(0) == "0");
        CHECK(num::to_hex_string<uint8_t>(1) == "1");
        CHECK(num::to_hex_string<uint8_t>(2) == "2");
        CHECK(num::to_hex_string<uint8_t>(std::numeric_limits<int8_t>::max()) == "7f");
        CHECK(num::to_hex_string<uint8_t>(std::numeric_limits<uint8_t>::max()) == "ff");
    }

    SECTION("Test num::to_string<int8_t>")
    {
        CHECK(num::to_string<int8_t>(std::numeric_limits<int8_t>::min()) == "-128");
        CHECK(num::to_string<int8_t>(-2) == "-2");
        CHECK(num::to_string<int8_t>(-1) == "-1");
        CHECK(num::to_string<int8_t>(0) == "0");
        CHECK(num::to_string<int8_t>(1) == "1");
        CHECK(num::to_string<int8_t>(2) == "2");
        CHECK(num::to_string<int8_t>(std::numeric_limits<int8_t>::max()) == "127");
    }

    SECTION("Test num::to_hex_string<int8_t>")
    {
        CHECK(num::to_hex_string<int8_t>(std::numeric_limits<int8_t>::min()) == "80");
        CHECK(num::to_hex_string<int8_t>(-2) == "fe");
        CHECK(num::to_hex_string<int8_t>(-1) == "ff");
        CHECK(num::to_hex_string<int8_t>(0) == "0");
        CHECK(num::to_hex_string<int8_t>(1) == "1");
        CHECK(num::to_hex_string<int8_t>(2) == "2");
        CHECK(num::to_hex_string<int8_t>(std::numeric_limits<int8_t>::max()) == "7f");
    }

    SECTION("Test num::to_string<uint16_t>")
    {
        CHECK(num::to_string<uint16_t>(0) == "0");
        CHECK(num::to_string<uint16_t>(1) == "1");
        CHECK(num::to_string<uint16_t>(2) == "2");
        CHECK(num::to_string<uint16_t>(std::numeric_limits<int8_t>::max()) == "127");
        CHECK(num::to_string<uint16_t>(std::numeric_limits<uint8_t>::max()) == "255");
        CHECK(num::to_string<uint16_t>(std::numeric_limits<int16_t>::max()) == "32767");
        CHECK(num::to_string<uint16_t>(std::numeric_limits<uint16_t>::max()) == "65535");
    }

    SECTION("Test num::to_hex_string<uint16_t>")
    {
        CHECK(num::to_hex_string<uint16_t>(0) == "0");
        CHECK(num::to_hex_string<uint16_t>(1) == "1");
        CHECK(num::to_hex_string<uint16_t>(2) == "2");
        CHECK(num::to_hex_string<uint16_t>(std::numeric_limits<int8_t>::max()) == "7f");
        CHECK(num::to_hex_string<uint16_t>(std::numeric_limits<uint8_t>::max()) == "ff");
        CHECK(num::to_hex_string<uint16_t>(std::numeric_limits<int16_t>::max()) == "7fff");
        CHECK(num::to_hex_string<uint16_t>(std::numeric_limits<uint16_t>::max()) == "ffff");
    }

    SECTION("Test num::to_string<int16_t>")
    {
        CHECK(num::to_string<int16_t>(std::numeric_limits<int16_t>::min()) == "-32768");
        CHECK(num::to_string<int16_t>(std::numeric_limits<int8_t>::min()) == "-128");
        CHECK(num::to_string<int16_t>(-2) == "-2");
        CHECK(num::to_string<int16_t>(-1) == "-1");
        CHECK(num::to_string<int16_t>(0) == "0");
        CHECK(num::to_string<int16_t>(1) == "1");
        CHECK(num::to_string<int16_t>(2) == "2");
        CHECK(num::to_string<int16_t>(std::numeric_limits<int8_t>::max()) == "127");
        CHECK(num::to_string<int16_t>(std::numeric_limits<uint8_t>::max()) == "255");
        CHECK(num::to_string<int16_t>(std::numeric_limits<int16_t>::max()) == "32767");
    }

    SECTION("Test num::to_hex_string<int16_t>")
    {
        CHECK(num::to_hex_string<int16_t>(std::numeric_limits<int16_t>::min()) == "8000");
        CHECK(num::to_hex_string<int16_t>(std::numeric_limits<int8_t>::min()) == "ff80");
        CHECK(num::to_hex_string<int16_t>(-2) == "fffe");
        CHECK(num::to_hex_string<int16_t>(-1) == "ffff");
        CHECK(num::to_hex_string<int16_t>(0) == "0");
        CHECK(num::to_hex_string<int16_t>(1) == "1");
        CHECK(num::to_hex_string<int16_t>(2) == "2");
        CHECK(num::to_hex_string<int16_t>(std::numeric_limits<int8_t>::max()) == "7f");
        CHECK(num::to_hex_string<int16_t>(std::numeric_limits<uint8_t>::max()) == "ff");
        CHECK(num::to_hex_string<int16_t>(std::numeric_limits<int16_t>::max()) == "7fff");
    }

    SECTION("Test num::to_string<uint32_t>")
    {
        CHECK(num::to_string<uint32_t>(0) == "0");
        CHECK(num::to_string<uint32_t>(1) == "1");
        CHECK(num::to_string<uint32_t>(2) == "2");
        CHECK(num::to_string<uint32_t>(std::numeric_limits<int8_t>::max()) == "127");
        CHECK(num::to_string<uint32_t>(std::numeric_limits<uint8_t>::max()) == "255");
        CHECK(num::to_string<uint32_t>(std::numeric_limits<int16_t>::max()) == "32767");
        CHECK(num::to_string<uint32_t>(std::numeric_limits<uint16_t>::max()) == "65535");
        CHECK(num::to_string<uint32_t>(std::numeric_limits<int32_t>::max()) == "2147483647");
        CHECK(num::to_string<uint32_t>(std::numeric_limits<uint32_t>::max()) == "4294967295");
    }

    SECTION("Test num::to_hex_string<uint32_t>")
    {
        CHECK(num::to_hex_string<uint32_t>(0) == "0");
        CHECK(num::to_hex_string<uint32_t>(1) == "1");
        CHECK(num::to_hex_string<uint32_t>(2) == "2");
        CHECK(num::to_hex_string<uint32_t>(std::numeric_limits<int8_t>::max()) == "7f");
        CHECK(num::to_hex_string<uint32_t>(std::numeric_limits<uint8_t>::max()) == "ff");
        CHECK(num::to_hex_string<uint32_t>(std::numeric_limits<int16_t>::max()) == "7fff");
        CHECK(num::to_hex_string<uint32_t>(std::numeric_limits<uint16_t>::max()) == "ffff");
        CHECK(num::to_hex_string<uint32_t>(std::numeric_limits<int32_t>::max()) == "7fffffff");
        CHECK(num::to_hex_string<uint32_t>(std::numeric_limits<uint32_t>::max()) == "ffffffff");
    }

    SECTION("Test num::to_string<int32_t>")
    {
        CHECK(num::to_string<int32_t>(std::numeric_limits<int32_t>::min()) == "-2147483648");
        CHECK(num::to_string<int32_t>(std::numeric_limits<int16_t>::min()) == "-32768");
        CHECK(num::to_string<int32_t>(std::numeric_limits<int8_t>::min()) == "-128");
        CHECK(num::to_string<int32_t>(-2) == "-2");
        CHECK(num::to_string<int32_t>(-1) == "-1");
        CHECK(num::to_string<int32_t>(0) == "0");
        CHECK(num::to_string<int32_t>(1) == "1");
        CHECK(num::to_string<int32_t>(2) == "2");
        CHECK(num::to_string<int32_t>(std::numeric_limits<int8_t>::max()) == "127");
        CHECK(num::to_string<int32_t>(std::numeric_limits<uint8_t>::max()) == "255");
        CHECK(num::to_string<int32_t>(std::numeric_limits<int16_t>::max()) == "32767");
        CHECK(num::to_string<int32_t>(std::numeric_limits<uint16_t>::max()) == "65535");
        CHECK(num::to_string<int32_t>(std::numeric_limits<int32_t>::max()) == "2147483647");
    }

    SECTION("Test num::to_hex_string<int32_t>")
    {
        CHECK(num::to_hex_string<int32_t>(std::numeric_limits<int32_t>::min()) == "80000000");
        CHECK(num::to_hex_string<int32_t>(std::numeric_limits<int16_t>::min()) == "ffff8000");
        CHECK(num::to_hex_string<int32_t>(std::numeric_limits<int8_t>::min()) == "ffffff80");
        CHECK(num::to_hex_string<int32_t>(-2) == "fffffffe");
        CHECK(num::to_hex_string<int32_t>(-1) == "ffffffff");
        CHECK(num::to_hex_string<int32_t>(0) == "0");
        CHECK(num::to_hex_string<int32_t>(1) == "1");
        CHECK(num::to_hex_string<int32_t>(2) == "2");
        CHECK(num::to_hex_string<int32_t>(std::numeric_limits<int8_t>::max()) == "7f");
        CHECK(num::to_hex_string<int32_t>(std::numeric_limits<uint8_t>::max()) == "ff");
        CHECK(num::to_hex_string<int32_t>(std::numeric_limits<int16_t>::max()) == "7fff");
        CHECK(num::to_hex_string<int32_t>(std::numeric_limits<uint16_t>::max()) == "ffff");
        CHECK(num::to_hex_string<int32_t>(std::numeric_limits<int32_t>::max()) == "7fffffff");
    }

    SECTION("Test num::to_string<uint64_t>")
    {
        CHECK(num::to_string<uint64_t>(0) == "0");
        CHECK(num::to_string<uint64_t>(1) == "1");
        CHECK(num::to_string<uint64_t>(2) == "2");
        CHECK(num::to_string<uint64_t>(std::numeric_limits<int8_t>::max()) == "127");
        CHECK(num::to_string<uint64_t>(std::numeric_limits<uint8_t>::max()) == "255");
        CHECK(num::to_string<uint64_t>(std::numeric_limits<int16_t>::max()) == "32767");
        CHECK(num::to_string<uint64_t>(std::numeric_limits<uint16_t>::max()) == "65535");
        CHECK(num::to_string<uint64_t>(std::numeric_limits<int32_t>::max()) == "2147483647");
        CHECK(num::to_string<uint64_t>(std::numeric_limits<uint32_t>::max()) == "4294967295");
        CHECK(num::to_string<uint64_t>(std::numeric_limits<int64_t>::max()) == "9223372036854775807");
        CHECK(num::to_string<uint64_t>(std::numeric_limits<uint64_t>::max()) == "18446744073709551615");
    }

    SECTION("Test num::to_hex_string<uint64_t>")
    {
        CHECK(num::to_hex_string<uint64_t>(0) == "0");
        CHECK(num::to_hex_string<uint64_t>(1) == "1");
        CHECK(num::to_hex_string<uint64_t>(2) == "2");
        CHECK(num::to_hex_string<uint64_t>(std::numeric_limits<int8_t>::max()) == "7f");
        CHECK(num::to_hex_string<uint64_t>(std::numeric_limits<uint8_t>::max()) == "ff");
        CHECK(num::to_hex_string<uint64_t>(std::numeric_limits<int16_t>::max()) == "7fff");
        CHECK(num::to_hex_string<uint64_t>(std::numeric_limits<uint16_t>::max()) == "ffff");
        CHECK(num::to_hex_string<uint64_t>(std::numeric_limits<int32_t>::max()) == "7fffffff");
        CHECK(num::to_hex_string<uint64_t>(std::numeric_limits<uint32_t>::max()) == "ffffffff");
        CHECK(num::to_hex_string<uint64_t>(std::numeric_limits<int64_t>::max()) == "7fffffffffffffff");
        CHECK(num::to_hex_string<uint64_t>(std::numeric_limits<uint64_t>::max()) == "ffffffffffffffff");
    }

    SECTION("Test num::to_string<int64_t>")
    {
        CHECK(num::to_string<int64_t>(std::numeric_limits<int64_t>::min()) == "-9223372036854775808");
        CHECK(num::to_string<int64_t>(std::numeric_limits<int32_t>::min()) == "-2147483648");
        CHECK(num::to_string<int64_t>(std::numeric_limits<int16_t>::min()) == "-32768");
        CHECK(num::to_string<int64_t>(std::numeric_limits<int8_t>::min()) == "-128");
        CHECK(num::to_string<int64_t>(-2) == "-2");
        CHECK(num::to_string<int64_t>(-1) == "-1");
        CHECK(num::to_string<int64_t>(0) == "0");
        CHECK(num::to_string<int64_t>(1) == "1");
        CHECK(num::to_string<int64_t>(2) == "2");
        CHECK(num::to_string<int64_t>(std::numeric_limits<int8_t>::max()) == "127");
        CHECK(num::to_string<int64_t>(std::numeric_limits<uint8_t>::max()) == "255");
        CHECK(num::to_string<int64_t>(std::numeric_limits<int16_t>::max()) == "32767");
        CHECK(num::to_string<int64_t>(std::numeric_limits<uint16_t>::max()) == "65535");
        CHECK(num::to_string<int64_t>(std::numeric_limits<int32_t>::max()) == "2147483647");
        CHECK(num::to_string<int64_t>(std::numeric_limits<uint32_t>::max()) == "4294967295");
        CHECK(num::to_string<int64_t>(std::numeric_limits<int64_t>::max()) == "9223372036854775807");
    }

    SECTION("Test num::to_hex_string<int64_t>")
    {
        CHECK(num::to_hex_string<int64_t>(std::numeric_limits<int64_t>::min()) == "8000000000000000");
        CHECK(num::to_hex_string<int64_t>(std::numeric_limits<int32_t>::min()) == "ffffffff80000000");
        CHECK(num::to_hex_string<int64_t>(std::numeric_limits<int16_t>::min()) == "ffffffffffff8000");
        CHECK(num::to_hex_string<int64_t>(std::numeric_limits<int8_t>::min()) == "ffffffffffffff80");
        CHECK(num::to_hex_string<int64_t>(-2) == "fffffffffffffffe");
        CHECK(num::to_hex_string<int64_t>(-1) == "ffffffffffffffff");
        CHECK(num::to_hex_string<int64_t>(0) == "0");
        CHECK(num::to_hex_string<int64_t>(1) == "1");
        CHECK(num::to_hex_string<int64_t>(2) == "2");
        CHECK(num::to_hex_string<int64_t>(std::numeric_limits<int8_t>::max()) == "7f");
        CHECK(num::to_hex_string<int64_t>(std::numeric_limits<uint8_t>::max()) == "ff");
        CHECK(num::to_hex_string<int64_t>(std::numeric_limits<int16_t>::max()) == "7fff");
        CHECK(num::to_hex_string<int64_t>(std::numeric_limits<uint16_t>::max()) == "ffff");
        CHECK(num::to_hex_string<int64_t>(std::numeric_limits<int32_t>::max()) == "7fffffff");
        CHECK(num::to_hex_string<int64_t>(std::numeric_limits<uint32_t>::max()) == "ffffffff");
        CHECK(num::to_hex_string<int64_t>(std::numeric_limits<int64_t>::max()) == "7fffffffffffffff");
    }

    SECTION("Test num::to_string<float32_t>")
    {
        // This is quite hard to test, because floating points numbers are not
        // exact. Therefore only some tests with a few values that should work
        // everywhere are actually done here.
        CHECK(num::to_string<float32_t>(0.0F) == "0.0");
        CHECK(num::to_string<float32_t>(1.0F) == "1.0");
        CHECK(num::to_string<float32_t>(-1.0e3F) == "-1000.0"); // NOLINT(readability-magic-numbers)
        CHECK(num::to_string<float32_t>(-1.0e10F) == "-10000000000.0"); // NOLINT(readability-magic-numbers)
    }

    SECTION("Test num::to_string<float64_t>")
    {
        // This is quite hard to test, because floating points numbers are not
        // exact. Therefore only some tests with a few values that should work
        // everywhere are actually done here.
        CHECK(num::to_string<float64_t>(0.0) == "0.0");
        CHECK(num::to_string<float64_t>(1.0) == "1.0");
        CHECK(num::to_string<float64_t>(-1.0e3) == "-1000.0"); // NOLINT(readability-magic-numbers)
        CHECK(num::to_string<float64_t>(-1.0e10) == "-10000000000.0"); // NOLINT(readability-magic-numbers)
    }

    // FIXME C++17 Enable and fix from/to_string for float128_t and add tests
}


TEMPLATE_TEST_CASE("conf::numeric::from_string", "[yarp::conf]", bool)
{
    using T = TestType;

    SECTION("Test default value")
    {
        CHECK(num::from_string<T>("1") == true);
        CHECK(num::from_string<T>("true") == true);
        CHECK(num::from_string<T>("True") == true);
        CHECK(num::from_string<T>("TRUE") == true);
        CHECK(num::from_string<T>("yes") == true);
        CHECK(num::from_string<T>("Yes") == true);
        CHECK(num::from_string<T>("YES") == true);
        CHECK(num::from_string<T>("on") == true);
        CHECK(num::from_string<T>("On") == true);
        CHECK(num::from_string<T>("ON") == true);

        CHECK(num::from_string<T>("0") == false);
        CHECK(num::from_string<T>("false") == false);
        CHECK(num::from_string<T>("False") == false);
        CHECK(num::from_string<T>("FALSE") == false);
        CHECK(num::from_string<T>("no") == false);
        CHECK(num::from_string<T>("No") == false);
        CHECK(num::from_string<T>("NO") == false);
        CHECK(num::from_string<T>("off") == false);
        CHECK(num::from_string<T>("Off") == false);
        CHECK(num::from_string<T>("OFF") == false);

        CHECK(num::from_string<T>("") == false);
        CHECK(num::from_string<T>("trueANDMORE") == false);
        CHECK(num::from_string<T>("STRING_VALUE") == false);
    }

    SECTION("Test alternative default value (false)")
    {
        CHECK(num::from_string<T>("1", false) == true);
        CHECK(num::from_string<T>("true", false) == true);
        CHECK(num::from_string<T>("True", false) == true);
        CHECK(num::from_string<T>("TRUE", false) == true);
        CHECK(num::from_string<T>("yes", false) == true);
        CHECK(num::from_string<T>("Yes", false) == true);
        CHECK(num::from_string<T>("YES", false) == true);
        CHECK(num::from_string<T>("on", false) == true);
        CHECK(num::from_string<T>("On", false) == true);
        CHECK(num::from_string<T>("ON", false) == true);

        CHECK(num::from_string<T>("0", false) == false);
        CHECK(num::from_string<T>("false", false) == false);
        CHECK(num::from_string<T>("False", false) == false);
        CHECK(num::from_string<T>("FALSE", false) == false);
        CHECK(num::from_string<T>("no", false) == false);
        CHECK(num::from_string<T>("No", false) == false);
        CHECK(num::from_string<T>("NO", false) == false);
        CHECK(num::from_string<T>("off", false) == false);
        CHECK(num::from_string<T>("Off", false) == false);
        CHECK(num::from_string<T>("OFF", false) == false);

        CHECK(num::from_string<T>("", false) == false);
        CHECK(num::from_string<T>("trueANDMORE", false) == false);
        CHECK(num::from_string<T>("STRING_VALUE", false) == false);
    }

    SECTION("Test alternative default value (true)")
    {
        CHECK(num::from_string<T>("1", true) == true);
        CHECK(num::from_string<T>("true", true) == true);
        CHECK(num::from_string<T>("True", true) == true);
        CHECK(num::from_string<T>("TRUE", true) == true);
        CHECK(num::from_string<T>("yes", true) == true);
        CHECK(num::from_string<T>("Yes", true) == true);
        CHECK(num::from_string<T>("YES", true) == true);
        CHECK(num::from_string<T>("on", true) == true);
        CHECK(num::from_string<T>("On", true) == true);
        CHECK(num::from_string<T>("ON", true) == true);

        CHECK(num::from_string<T>("0", true) == false);
        CHECK(num::from_string<T>("false", true) == false);
        CHECK(num::from_string<T>("False", true) == false);
        CHECK(num::from_string<T>("FALSE", true) == false);
        CHECK(num::from_string<T>("no", true) == false);
        CHECK(num::from_string<T>("No", true) == false);
        CHECK(num::from_string<T>("NO", true) == false);
        CHECK(num::from_string<T>("off", true) == false);
        CHECK(num::from_string<T>("Off", true) == false);
        CHECK(num::from_string<T>("OFF", true) == false);

        CHECK(num::from_string<T>("", true) == true);
        CHECK(num::from_string<T>("trueANDMORE", true) == true);
        CHECK(num::from_string<T>("STRING_VALUE", true) == true);
    }
}

TEMPLATE_TEST_CASE("conf::numeric::from_string", "[yarp::conf]",
    uint8_t,
    int8_t,
    uint16_t,
    int16_t,
    uint32_t,
    int32_t,
    uint64_t,
    int64_t
)
{
    using T = TestType;
    using uT = std::make_unsigned_t<T>;
    using sT= std::make_signed_t<T>;
    constexpr bool is_unsigned = std::is_unsigned<T>::value;
    constexpr bool is_signed = std::is_signed<T>::value;
    constexpr T default_value = 0;
    constexpr T alt_default_value = 42;

    constexpr auto umin = static_cast<uT>(0);
    constexpr auto umax = std::numeric_limits<uT>::max();
    constexpr auto smin = std::numeric_limits<sT>::min();
    constexpr auto smax = std::numeric_limits<sT>::max();
    constexpr auto smaxpp = static_cast<uT>(smax) + 1;

    const auto umin_s = std::to_string(umin);
    const auto umax_s = std::to_string(umax);
    const auto smin_s = std::to_string(smin);
    const auto smax_s = std::to_string(smax);
    const auto smaxpp_s = std::to_string(smaxpp);

    const auto y1_s = std::to_string(1);
    const auto y2_s = std::to_string(2);
    const auto yneg1_s = std::to_string(-1);
    const auto yneg2_s = std::to_string(-2);

    const std::string yempty_s;
    const std::string ynot_numeric_s{"NOT_NUMERIC"};
    const std::string y20error_s{"20ERROR"};

    SECTION("Test default value")
    {
        CHECK(num::from_string<T>(umin_s) == umin);
        CHECK(num::from_string<T>(umax_s) == (is_unsigned ? umax : default_value));
        CHECK(num::from_string<T>(smin_s) == (is_signed ? smin : default_value));
        CHECK(num::from_string<T>(smax_s) == smax);
        CHECK(num::from_string<T>(smaxpp_s) == (is_unsigned ? smaxpp : default_value));
        CHECK(num::from_string<T>(y1_s) == 1);
        CHECK(num::from_string<T>(y2_s) == 2);
        CHECK(num::from_string<T>(yneg1_s) == (is_signed ? -1 : default_value));
        CHECK(num::from_string<T>(yneg2_s) == (is_signed ? -2 : default_value));
        CHECK(num::from_string<T>(yempty_s) == default_value);
        CHECK(num::from_string<T>(ynot_numeric_s) == default_value);
        CHECK(num::from_string<T>(y20error_s) == default_value);
    }

    SECTION("Test alternative default value")
    {
        CHECK(num::from_string<T>(umin_s, alt_default_value) == umin);
        CHECK(num::from_string<T>(umax_s, alt_default_value) == (is_unsigned ? umax : alt_default_value));
        CHECK(num::from_string<T>(smin_s, alt_default_value) == (is_signed ? smin : alt_default_value));
        CHECK(num::from_string<T>(smax_s, alt_default_value) == smax);
        CHECK(num::from_string<T>(smaxpp_s, alt_default_value) == (is_unsigned ? smaxpp : alt_default_value));
        CHECK(num::from_string<T>(y1_s, alt_default_value) == 1);
        CHECK(num::from_string<T>(y2_s, alt_default_value) == 2);
        CHECK(num::from_string<T>(yneg1_s, alt_default_value) == (is_signed ? -1 : alt_default_value));
        CHECK(num::from_string<T>(yneg2_s, alt_default_value) == (is_signed ? -2 : alt_default_value));
        CHECK(num::from_string<T>(yempty_s, alt_default_value) == alt_default_value);
        CHECK(num::from_string<T>(ynot_numeric_s, alt_default_value) == alt_default_value);
        CHECK(num::from_string<T>(y20error_s, alt_default_value) == alt_default_value);
    }
}

TEMPLATE_TEST_CASE("conf::numeric::from_string", "[yarp::conf]",
    float32_t,
    float64_t
)
{
    using T = TestType;

    constexpr T default_value = 0.0;
    constexpr T alt_default_value = 42.0;

    SECTION("Test default value")
    {
        CHECK(num::from_string<T>("0") == static_cast<T>(0.0));
        CHECK(num::from_string<T>("0.0") == static_cast<T>(0.0));
        CHECK(num::from_string<T>(".0") == static_cast<T>(0.0));
        CHECK(num::from_string<T>("0,0") == default_value);

        CHECK(num::from_string<T>("1") == static_cast<T>(1.0));
        CHECK(num::from_string<T>("1.0") == static_cast<T>(1.0));
        CHECK(num::from_string<T>("0.1") == static_cast<T>(0.1)); // NOLINT(readability-magic-numbers)
        CHECK(num::from_string<T>(".1") == static_cast<T>(0.1)); // NOLINT(readability-magic-numbers)

        CHECK(num::from_string<T>("-1.1e3") == static_cast<T>(-1.1e3)); // NOLINT(readability-magic-numbers)
        CHECK(num::from_string<T>("-1.1e10") == static_cast<T>(-1.1e10)); // NOLINT(readability-magic-numbers)
        CHECK(num::from_string<T>("-1.1e-3") == static_cast<T>(-1.1e-3)); // NOLINT(readability-magic-numbers)
        CHECK(num::from_string<T>("-1.1e-10") == static_cast<T>(-1.1e-10)); // NOLINT(readability-magic-numbers)

        CHECK(num::from_string<T>("inf") == std::numeric_limits<T>::infinity());
        CHECK(num::from_string<T>("-inf") == -std::numeric_limits<T>::infinity());
        CHECK(std::isnan(num::from_string<T>("nan")));

        CHECK(num::from_string<T>("") == default_value);
        CHECK(num::from_string<T>("13ANDMORE") == default_value);
        CHECK(num::from_string<T>("13.0.0") == default_value);
        CHECK(num::from_string<T>("STRING_VALUE") == default_value);
    }

    SECTION("Test alternative default value")
    {
        CHECK(num::from_string<T>("0", alt_default_value) == static_cast<T>(0.0));
        CHECK(num::from_string<T>("0.0", alt_default_value) == static_cast<T>(0.0));
        CHECK(num::from_string<T>(".0", alt_default_value) == static_cast<T>(0.0));
        CHECK(num::from_string<T>("0,0", alt_default_value) == alt_default_value);

        CHECK(num::from_string<T>("1", alt_default_value) == static_cast<T>(1.0));
        CHECK(num::from_string<T>("1.0", alt_default_value) == static_cast<T>(1.0));
        CHECK(num::from_string<T>("0.1", alt_default_value) == static_cast<T>(0.1)); // NOLINT(readability-magic-numbers)
        CHECK(num::from_string<T>(".1", alt_default_value) == static_cast<T>(0.1)); // NOLINT(readability-magic-numbers)

        CHECK(num::from_string<T>("-1.1e3", alt_default_value) == static_cast<T>(-1.1e3)); // NOLINT(readability-magic-numbers)
        CHECK(num::from_string<T>("-1.1e10", alt_default_value) == static_cast<T>(-1.1e10)); // NOLINT(readability-magic-numbers)
        CHECK(num::from_string<T>("-1.1e-3", alt_default_value) == static_cast<T>(-1.1e-3)); // NOLINT(readability-magic-numbers)
        CHECK(num::from_string<T>("-1.1e-10", alt_default_value) == static_cast<T>(-1.1e-10)); // NOLINT(readability-magic-numbers)

        CHECK(num::from_string<T>("inf", alt_default_value) == std::numeric_limits<T>::infinity());
        CHECK(num::from_string<T>("-inf", alt_default_value) == -std::numeric_limits<T>::infinity());
        CHECK(std::isnan(num::from_string<T>("nan", alt_default_value)));

        CHECK(num::from_string<T>("", alt_default_value) == alt_default_value);
        CHECK(num::from_string<T>("13ANDMORE", alt_default_value) == alt_default_value);
        CHECK(num::from_string<T>("13.0.0", alt_default_value) == alt_default_value);
        CHECK(num::from_string<T>("STRING_VALUE", alt_default_value) == alt_default_value);
    }
}
