/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/robotinterface/Param.h>
#include <yarp/robotinterface/XMLReader.h>

#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/IMultipleWrapper.h>

#include <catch.hpp>
#include <harness.h>

TEST_CASE("robotinterface::ParamTest", "[yarp::robotinterface]")
{
    SECTION("Check yarp::robotinterface::Param")
    {
        std::string name = "name";
        std::string value = "value";
        bool isGroup = false;
        yarp::robotinterface::Param param(name, value, isGroup);

        CHECK(param.name() == name);
        CHECK(param.value() == value);
        CHECK(param.isGroup() ==  isGroup);
    }
};

// Dummy device used in "Check valid robot file with two devices" test
namespace yarp {
    namespace dev {
        class RobotInterfaceTestMockDriver;
        class RobotInterfaceTestMockWrapper;
    }
}

struct GlobalState
{
    bool mockDriverWasOpened;
    bool mockWrapperWasOpened;
    bool mockAttachWasCalled;
    bool mockDetachWasCalled;
    bool mockWrapperWasClosed;
    bool mockDriverWasClosed;

    void reset()
    {
        mockDriverWasOpened = false;
        mockWrapperWasOpened = false;
        mockAttachWasCalled = false;
        mockDetachWasCalled = false;
        mockWrapperWasClosed = false;
        mockDriverWasClosed = false;
    }
};

GlobalState globalState;

class yarp::dev::RobotInterfaceTestMockDriver :
        public yarp::dev::DeviceDriver
{
public:
    virtual ~RobotInterfaceTestMockDriver();

    //DEVICE DRIVER
    virtual bool open(yarp::os::Searchable& config);
    virtual bool close();
};

yarp::dev::RobotInterfaceTestMockDriver::~RobotInterfaceTestMockDriver()
{
}

bool yarp::dev::RobotInterfaceTestMockDriver::open(yarp::os::Searchable& config)
{
    globalState.mockDriverWasOpened = true;
    return true;
}

bool yarp::dev::RobotInterfaceTestMockDriver::close()
{
    globalState.mockDriverWasClosed = true;
    return true;
}

class yarp::dev::RobotInterfaceTestMockWrapper :
        public yarp::dev::DeviceDriver,
        public yarp::dev::IMultipleWrapper
{
public:
    virtual ~RobotInterfaceTestMockWrapper();

    //DEVICE DRIVER
    virtual bool open(yarp::os::Searchable& config);
    virtual bool close();

    //IMULTIPLEWRAPPER
    virtual bool attachAll(const PolyDriverList&);
    virtual bool detachAll();
};

yarp::dev::RobotInterfaceTestMockWrapper::~RobotInterfaceTestMockWrapper()
{
}

bool yarp::dev::RobotInterfaceTestMockWrapper::open(yarp::os::Searchable&)
{
    globalState.mockWrapperWasOpened = true;
    return true;
}

bool yarp::dev::RobotInterfaceTestMockWrapper::close()
{
    globalState.mockWrapperWasClosed = true;
    return true;
}

bool yarp::dev::RobotInterfaceTestMockWrapper::attachAll(const PolyDriverList&)
{
    globalState.mockAttachWasCalled = true;
    return true;
}

bool yarp::dev::RobotInterfaceTestMockWrapper::detachAll()
{
    globalState.mockDetachWasCalled = true;
    return true;
}



TEST_CASE("robotinterface::XMLReaderTest", "[yarp::robotinterface]")
{
    SECTION("Check empty string")
    {
        // Load empty XML configuration file
        std::string XMLString = "";
        yarp::robotinterface::XMLReader reader;
        yarp::robotinterface::XMLReaderResult result = reader.getRobotFromString(XMLString);

        // Check parsing fails on empty string
        CHECK(!result.parsingIsSuccessful);
    }

    SECTION("Check valid robot file with no devices")
    {
        // Load empty XML configuration file
        std::string XMLString = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<!DOCTYPE robot PUBLIC \"-//YARP//DTD yarprobotinterface 3.0//EN\" \"http://www.yarp.it/DTD/yarprobotinterfaceV3.0.dtd\">\n"
                                "<robot name=\"RobotWithNoDevices\" prefix=\"RobotWithNoDevices\">\n"
                                "  <devices>\n"
                                "  </devices>\n"
                                "</robot>\n";

        yarp::robotinterface::XMLReader reader;
        yarp::robotinterface::XMLReaderResult result = reader.getRobotFromString(XMLString);

        // Check parsing fails on empty string
        CHECK(result.parsingIsSuccessful);

        // Verify that no device has been loaded
        CHECK(result.robot.devices().size() == 0);
    }

    SECTION("Check valid robot file with two devices")
    {
        // Reset test flags
        globalState.reset();

        // Add dummy devices to YARP drivers factory
        yarp::dev::Drivers::factory().add(new yarp::dev::DriverCreatorOf<yarp::dev::RobotInterfaceTestMockDriver>("robotinterface_test_mock_device", "", "RobotInterfaceTestMockDriver"));
        yarp::dev::Drivers::factory().add(new yarp::dev::DriverCreatorOf<yarp::dev::RobotInterfaceTestMockWrapper>("robotinterface_test_mock_wrapper", "", "RobotInterfaceTestMockWrapper"));

        // Load empty XML configuration file
        std::string XMLString = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<!DOCTYPE robot PUBLIC \"-//YARP//DTD yarprobotinterface 3.0//EN\" \"http://www.yarp.it/DTD/yarprobotinterfaceV3.0.dtd\">\n"
                                "<robot name=\"RobotWithOneDevice\" prefix=\"RobotWithOneDevice\">\n"
                                "  <devices>\n"
                                "    <device name=\"dummy_device\" type=\"robotinterface_test_mock_device\">\n"
                                "    </device>\n"
                                "    <device name=\"dummy_wrapper\" type=\"robotinterface_test_mock_wrapper\">\n"
                                "      <action phase=\"startup\" level=\"5\" type=\"attach\">\n"
                                "        <paramlist name=\"networks\">\n"
                                "          <elem name=\"attached_device\">  dummy_device </elem>\n"
                                "        </paramlist>\n"
                                "      </action>\n"
                                "      <action phase=\"shutdown\" level=\"5\" type=\"detach\" />\n"
                                "    </device>\n"
                                "  </devices>\n"
                                "</robot>\n";

        yarp::robotinterface::XMLReader reader;
        yarp::robotinterface::XMLReaderResult result = reader.getRobotFromString(XMLString);

        // Check parsing fails on empty string
        CHECK(result.parsingIsSuccessful);

        // Verify that only one device has been loaded
        CHECK(result.robot.devices().size() == 2);

        // Verify that the devices were not opened and the attach was not called
        CHECK(!globalState.mockDriverWasOpened);
        CHECK(!globalState.mockWrapperWasClosed);
        CHECK(!globalState.mockAttachWasCalled);
        CHECK(!globalState.mockDetachWasCalled);
        CHECK(!globalState.mockWrapperWasClosed);
        CHECK(!globalState.mockDriverWasClosed);

        // Start the robot (open the device and call "attach" actions)
        bool ok = result.robot.enterPhase(yarp::robotinterface::ActionPhaseStartup);
        CHECK(ok);

        // Check that the device was opened and attach called
        CHECK(globalState.mockDriverWasOpened);
        CHECK(globalState.mockWrapperWasOpened);
        CHECK(globalState.mockAttachWasCalled);
        CHECK(!globalState.mockDetachWasCalled);
        CHECK(!globalState.mockWrapperWasClosed);
        CHECK(!globalState.mockDriverWasClosed);

        // Stop the robot
        ok = result.robot.enterPhase(yarp::robotinterface::ActionPhaseInterrupt1);
        CHECK(ok);
        ok = result.robot.enterPhase(yarp::robotinterface::ActionPhaseShutdown);
        CHECK(ok);

        // Check that the device was closed and detach called
        CHECK(globalState.mockDriverWasOpened);
        CHECK(globalState.mockWrapperWasOpened);
        CHECK(globalState.mockAttachWasCalled);
        CHECK(globalState.mockDetachWasCalled);
        CHECK(globalState.mockWrapperWasClosed);
        CHECK(globalState.mockDriverWasClosed);
    }
}
