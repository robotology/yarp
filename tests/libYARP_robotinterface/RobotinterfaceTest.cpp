/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/robotinterface/Param.h>
#include <yarp/robotinterface/XMLReader.h>

#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/PolyDriverList.h>
#include <yarp/dev/IMultipleWrapper.h>

#include <yarp/os/LogStream.h>

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
    std::string mockDriverParamValue;

    void reset()
    {
        mockDriverWasOpened = false;
        mockWrapperWasOpened = false;
        mockAttachWasCalled = false;
        mockDetachWasCalled = false;
        mockWrapperWasClosed = false;
        mockDriverWasClosed = false;
        mockDriverParamValue.clear();
    }
};

GlobalState globalState;

class yarp::dev::RobotInterfaceTestMockDriver :
        public yarp::dev::DeviceDriver
{
public:
    ~RobotInterfaceTestMockDriver() override;

    //DEVICE DRIVER
    bool open(yarp::os::Searchable& config) override;
    bool close() override;
};

yarp::dev::RobotInterfaceTestMockDriver::~RobotInterfaceTestMockDriver()
{
}

bool yarp::dev::RobotInterfaceTestMockDriver::open(yarp::os::Searchable& config)
{
    globalState.mockDriverWasOpened = true;
    globalState.mockDriverParamValue = config.check("theparam", yarp::os::Value("theparam_unset"), "The param").asString();
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
    ~RobotInterfaceTestMockWrapper() override;

    //DEVICE DRIVER
    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    //IMULTIPLEWRAPPER
    bool attachAll(const PolyDriverList&) override;
    bool detachAll() override;
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

    SECTION("Check valid robot file with unset param")
    {
        // Reset test flags
        globalState.reset();

        // Add dummy devices to YARP drivers factory
        yarp::dev::Drivers::factory().add(new yarp::dev::DriverCreatorOf<yarp::dev::RobotInterfaceTestMockDriver>("robotinterface_test_mock_device", "", "RobotInterfaceTestMockDriver"));

        // Load empty XML configuration file
        std::string XMLString = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<!DOCTYPE robot PUBLIC \"-//YARP//DTD yarprobotinterface 3.1//EN\" \"http://www.yarp.it/DTD/yarprobotinterfaceV3.1.dtd\">\n"
                                "<robot name=\"RobotWithOneDevice\" prefix=\"RobotWithOneDevice\">\n"
                                "  <devices>\n"
                                "    <device name=\"dummy_device\" type=\"robotinterface_test_mock_device\">\n"
                                "    </device>\n"
                                 "  </devices>\n"
                                "</robot>\n";

        yarp::robotinterface::XMLReader reader;
        yarp::robotinterface::XMLReaderResult result = reader.getRobotFromString(XMLString);

        // Check parsing fails on empty string
        CHECK(result.parsingIsSuccessful);

        // Verify that only one device has been loaded
        CHECK(result.robot.devices().size() == 1);

        // Verify that the devices were not opened and the attach was not called
        CHECK(!globalState.mockDriverWasOpened);
        CHECK(!globalState.mockDriverWasClosed);
        CHECK(globalState.mockDriverParamValue.empty());

        // Start the robot (open the device and call "attach" actions)
        bool ok = result.robot.enterPhase(yarp::robotinterface::ActionPhaseStartup);
        CHECK(ok);

        // Check that the device was opened and attach called
        CHECK(globalState.mockDriverWasOpened);
        CHECK(!globalState.mockDriverWasClosed);
        CHECK(globalState.mockDriverParamValue == "theparam_unset");

        // Stop the robot
        ok = result.robot.enterPhase(yarp::robotinterface::ActionPhaseInterrupt1);
        CHECK(ok);
        ok = result.robot.enterPhase(yarp::robotinterface::ActionPhaseShutdown);
        CHECK(ok);

        // Check that the device was closed and detach called
        CHECK(globalState.mockDriverWasOpened);
        CHECK(globalState.mockDriverWasClosed);
        CHECK(globalState.mockDriverParamValue == "theparam_unset");
    }

    SECTION("Check valid robot file with fixed param")
    {
        // Reset test flags
        globalState.reset();

        // Add dummy devices to YARP drivers factory
        yarp::dev::Drivers::factory().add(new yarp::dev::DriverCreatorOf<yarp::dev::RobotInterfaceTestMockDriver>("robotinterface_test_mock_device", "", "RobotInterfaceTestMockDriver"));

        // Load empty XML configuration file
        std::string XMLString = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<!DOCTYPE robot PUBLIC \"-//YARP//DTD yarprobotinterface 3.1//EN\" \"http://www.yarp.it/DTD/yarprobotinterfaceV3.1.dtd\">\n"
                                "<robot name=\"RobotWithOneDevice\" prefix=\"RobotWithOneDevice\">\n"
                                "  <devices>\n"
                                "    <device name=\"dummy_device\" type=\"robotinterface_test_mock_device\">\n"
                                "      <param name=\"theparam\"> theparam_fixed </param>\n"
                                "    </device>\n"
                                 "  </devices>\n"
                                "</robot>\n";

        yarp::robotinterface::XMLReader reader;
        yarp::robotinterface::XMLReaderResult result = reader.getRobotFromString(XMLString);

        // Check parsing fails on empty string
        CHECK(result.parsingIsSuccessful);

        // Verify that only one device has been loaded
        CHECK(result.robot.devices().size() == 1);

        // Verify that the devices were not opened and the attach was not called
        CHECK(!globalState.mockDriverWasOpened);
        CHECK(!globalState.mockDriverWasClosed);
        CHECK(globalState.mockDriverParamValue.empty());

        // Start the robot (open the device and call "attach" actions)
        bool ok = result.robot.enterPhase(yarp::robotinterface::ActionPhaseStartup);
        CHECK(ok);

        // Check that the device was opened and attach called
        CHECK(globalState.mockDriverWasOpened);
        CHECK(!globalState.mockDriverWasClosed);
        CHECK(globalState.mockDriverParamValue == "theparam_fixed");

        // Stop the robot
        ok = result.robot.enterPhase(yarp::robotinterface::ActionPhaseInterrupt1);
        CHECK(ok);
        ok = result.robot.enterPhase(yarp::robotinterface::ActionPhaseShutdown);
        CHECK(ok);

        // Check that the device was closed and detach called
        CHECK(globalState.mockDriverWasOpened);
        CHECK(globalState.mockDriverWasClosed);
        CHECK(globalState.mockDriverParamValue == "theparam_fixed");
    }

    SECTION("Check valid robot file with extern param")
    {
        // Reset test flags
        globalState.reset();

        // Add dummy devices to YARP drivers factory
        yarp::dev::Drivers::factory().add(new yarp::dev::DriverCreatorOf<yarp::dev::RobotInterfaceTestMockDriver>("robotinterface_test_mock_device", "", "RobotInterfaceTestMockDriver"));

        // Load empty XML configuration file
        std::string XMLString = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<!DOCTYPE robot PUBLIC \"-//YARP//DTD yarprobotinterface 3.1//EN\" \"http://www.yarp.it/DTD/yarprobotinterfaceV3.1.dtd\">\n"
                                "<robot name=\"RobotWithOneDevice\" prefix=\"RobotWithOneDevice\">\n"
                                "  <devices>\n"
                                "    <device name=\"dummy_device\" type=\"robotinterface_test_mock_device\">\n"
                                "      <param name=\"theparam\" extern-name=\"theparam\"> theparam_fixed </param>\n"
                                "    </device>\n"
                                 "  </devices>\n"
                                "</robot>\n";

        yarp::robotinterface::XMLReader reader;
        yarp::os::Property config;
        config.put("theparam", "theparam_alt");
        yarp::robotinterface::XMLReaderResult result = reader.getRobotFromString(XMLString, config);

        // Check parsing fails on empty string
        CHECK(result.parsingIsSuccessful);

        // Verify that only one device has been loaded
        CHECK(result.robot.devices().size() == 1);

        // Verify that the devices were not opened and the attach was not called
        CHECK(!globalState.mockDriverWasOpened);
        CHECK(!globalState.mockDriverWasClosed);
        CHECK(globalState.mockDriverParamValue.empty());

        // Start the robot (open the device and call "attach" actions)
        bool ok = result.robot.enterPhase(yarp::robotinterface::ActionPhaseStartup);
        CHECK(ok);

        // Check that the device was opened and attach called
        CHECK(globalState.mockDriverWasOpened);
        CHECK(!globalState.mockDriverWasClosed);
        CHECK(globalState.mockDriverParamValue == "theparam_alt");

        // Stop the robot
        ok = result.robot.enterPhase(yarp::robotinterface::ActionPhaseInterrupt1);
        CHECK(ok);
        ok = result.robot.enterPhase(yarp::robotinterface::ActionPhaseShutdown);
        CHECK(ok);

        // Check that the device was closed and detach called
        CHECK(globalState.mockDriverWasOpened);
        CHECK(globalState.mockDriverWasClosed);
        CHECK(globalState.mockDriverParamValue == "theparam_alt");
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

    SECTION("Check valid robot file with one device attaching to an external device")
    {
        // Reset test flags
        globalState.reset();

        // Add dummy devices to YARP drivers factory
        yarp::dev::Drivers::factory().add(new yarp::dev::DriverCreatorOf<yarp::dev::RobotInterfaceTestMockDriver>("robotinterface_test_mock_device", "", "RobotInterfaceTestMockDriver"));
        yarp::dev::Drivers::factory().add(new yarp::dev::DriverCreatorOf<yarp::dev::RobotInterfaceTestMockWrapper>("robotinterface_test_mock_wrapper", "", "RobotInterfaceTestMockWrapper"));

        // Create dummy device externally
        yarp::os::Property p;
        p.put("device", "robotinterface_test_mock_device");

        yarp::dev::PolyDriver dummyDevice;
        dummyDevice.open(p);

        // Add the dummy device to the PolyDriverList with the appropriate name
        yarp::dev::PolyDriverList externalDriverList;
        externalDriverList.push(&dummyDevice, "dummy_device");

        // Load  XML configuration file
        std::string XMLString = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<!DOCTYPE robot PUBLIC \"-//YARP//DTD yarprobotinterface 3.0//EN\" \"http://www.yarp.it/DTD/yarprobotinterfaceV3.0.dtd\">\n"
                                "<robot name=\"RobotWithOneDevice\" prefix=\"RobotWithOneDevice\">\n"
                                "  <devices>\n"
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

        // Verify that only one (internal) device has been loaded
        CHECK(result.robot.devices().size() == 1);

        // Verify that only the robotinterface_test_mock_device device opened and the attach was not called
        CHECK(globalState.mockDriverWasOpened);
        CHECK(!globalState.mockWrapperWasClosed);
        CHECK(!globalState.mockAttachWasCalled);
        CHECK(!globalState.mockDetachWasCalled);
        CHECK(!globalState.mockWrapperWasClosed);
        CHECK(!globalState.mockDriverWasClosed);

        // Start the robot (open the device and call "attach" actions)
        bool ok = result.robot.setExternalDevices(externalDriverList);
        CHECK(ok);
        ok = result.robot.enterPhase(yarp::robotinterface::ActionPhaseStartup);
        CHECK(ok);

        // Check that the also the wrapper was opened and attach called
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

        // Check that the wrapper device was closed and detach called, while the external device was not closed
        CHECK(globalState.mockDriverWasOpened);
        CHECK(globalState.mockWrapperWasOpened);
        CHECK(globalState.mockAttachWasCalled);
        CHECK(globalState.mockDetachWasCalled);
        CHECK(globalState.mockWrapperWasClosed);
        CHECK(!globalState.mockDriverWasClosed);

        ok = dummyDevice.close();
        CHECK(ok);

        CHECK(globalState.mockDriverWasClosed);
    }

    SECTION("Check error in case of name conflict between internal devices and external devices")
    {
        // Reset test flags
        globalState.reset();

        // Add dummy devices to YARP drivers factory
        yarp::dev::Drivers::factory().add(new yarp::dev::DriverCreatorOf<yarp::dev::RobotInterfaceTestMockDriver>("robotinterface_test_mock_device", "", "RobotInterfaceTestMockDriver"));
        yarp::dev::Drivers::factory().add(new yarp::dev::DriverCreatorOf<yarp::dev::RobotInterfaceTestMockWrapper>("robotinterface_test_mock_wrapper", "", "RobotInterfaceTestMockWrapper"));

        // Create dummy device externally
        yarp::os::Property p;
        p.put("device", "robotinterface_test_mock_device");

        yarp::dev::PolyDriver dummyDevice;
        dummyDevice.open(p);

        // Add the dummy device to the PolyDriverList with the name "dummy_device"
        yarp::dev::PolyDriverList externalDriverList;
        externalDriverList.push(&dummyDevice, "dummy_device");

        // Load  XML configuration file that also contains an internal device with name "dummy_device"
        std::string XMLString = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
                                "<!DOCTYPE robot PUBLIC \"-//YARP//DTD yarprobotinterface 3.0//EN\" \"http://www.yarp.it/DTD/yarprobotinterfaceV3.0.dtd\">\n"
                                "<robot name=\"RobotWithOneDevice\" prefix=\"RobotWithOneDevice\">\n"
                                "  <devices>\n"
                                "    <device name=\"dummy_device\" type=\"robotinterface_test_mock_wrapper\">\n"
                                "    </device>\n"
                                "  </devices>\n"
                                "</robot>\n";

        yarp::robotinterface::XMLReader reader;
        yarp::robotinterface::XMLReaderResult result = reader.getRobotFromString(XMLString);

        CHECK(result.parsingIsSuccessful);

        // Verify that only one (internal) device has been loaded
        CHECK(result.robot.devices().size() == 1);

        // Set the external devices list and check that it fails due to naming conflicts
        bool ok = result.robot.setExternalDevices(externalDriverList);
        CHECK(!ok);

        // Cleanup
        ok = dummyDevice.close();
        CHECK(ok);
    }
}
