/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/all.h>
#include <yarp/dev/ReturnValue.h>

#include <TestReturnValue.h>

#include <catch2/catch_amalgamated.hpp>

using namespace yarp::os;
using yarp::dev::ReturnValue;

namespace {

constexpr auto comm_error = ReturnValue::return_code::return_value_error_nws_nwc_communication_error;

// Server side implementation, used to check that a successful call still
// returns the values sent by the server.
class TestReturnValueServer : public TestReturnValue
{
public:
    int32_t stored_value = 0;

    ReturnValue setValueRPC(const std::int32_t value) override
    {
        stored_value = value;
        return ReturnValue_ok;
    }

    return_getValue getValueRPC() override
    {
        return_getValue r;
        r.ret = ReturnValue_ok;
        r.value = stored_value;
        return r;
    }

    return_getTwoValues getTwoValuesRPC() override
    {
        return_getTwoValues r;
        r.ret = ReturnValue_ok;
        r.ret2 = ReturnValue_error_not_ready;
        r.text = "hello";
        return r;
    }

    bool getBoolRPC() override
    {
        return true;
    }
};

// Calls all the methods of a client whose write is expected to fail, and
// checks that every yarp::dev::ReturnValue reports a communication error.
void check_failed_calls(TestReturnValue& client)
{
    // Case 1: yarp::dev::ReturnValue returned directly
    ReturnValue ret = client.setValueRPC(42);
    CHECK(ret == comm_error);
    CHECK_FALSE(ret);

    // Case 2: struct with one yarp::dev::ReturnValue field
    return_getValue rv = client.getValueRPC();
    CHECK(rv.ret == comm_error);
    CHECK_FALSE(rv.ret);
    CHECK(rv.value == 0);

    // Case 2: struct with more than one yarp::dev::ReturnValue field
    return_getTwoValues rtv = client.getTwoValuesRPC();
    CHECK(rtv.ret == comm_error);
    CHECK(rtv.ret2 == comm_error);
    CHECK(rtv.text.empty());

    // Case 3: no yarp::dev::ReturnValue involved, default value returned
    CHECK(client.getBoolRPC() == false);
}

} // namespace


TEST_CASE("IdlThriftTest4", "[yarp::idl::thrift4]")
{
    yarp::os::Network::setLocalMode(true);

    SECTION("Failed write: client not attached to any port")
    {
        TestReturnValue client;
        check_failed_calls(client);
    }

    SECTION("Failed write: client port not connected to any server")
    {
        Port port;
        REQUIRE(port.open("/test4/client"));
        TestReturnValue client;
        REQUIRE(client.yarp().attachAsClient(port));
        check_failed_calls(client);
        port.close();
    }

    SECTION("Successful call: values returned by the server are preserved")
    {
        TestReturnValueServer server;
        TestReturnValue client;
        REQUIRE(client.yarp().attachAsClient(server));

        ReturnValue ret = client.setValueRPC(42);
        CHECK(ret == ReturnValue::return_code::return_value_ok);

        return_getValue rv = client.getValueRPC();
        CHECK(rv.ret == ReturnValue::return_code::return_value_ok);
        CHECK(rv.value == 42);

        return_getTwoValues rtv = client.getTwoValuesRPC();
        CHECK(rtv.ret == ReturnValue::return_code::return_value_ok);
        CHECK(rtv.ret2 == ReturnValue::return_code::return_value_error_not_ready);
        CHECK(rtv.text == "hello");

        CHECK(client.getBoolRPC() == true);
    }

    yarp::os::Network::setLocalMode(false);
}

int main(int argc, char* argv[])
{
    Network yarp;
    return Catch::Session().run(argc, argv);
}
