/*
 * Copyright (C) 2017 iCub Facility, Istituto Italiano di Tecnologia (IIT)
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <cmath>
#include <string>

#include <rtf/Asserter.h>
#include <rtf/TestAssert.h>
#include <rtf/dll/Plugin.h>

#include <yarp/rtf/TestCase.h>

#include <yarp/os/Network.h>
#include <yarp/os/NetworkClock.h>

using namespace std;
using namespace RTF;

/**********************************************************************/
class NetworkClockTest : public yarp::rtf::TestCase
{
    yarp::os::NetworkClock * m_simClock;
    std::string extClockName;
    std::string localPortName;

public:
    /******************************************************************/
    NetworkClockTest() :
            TestCase("NetworkClockTest"),
            m_simClock(nullptr)
    {
    }

    /******************************************************************/
    virtual ~NetworkClockTest()
    {
    }

    /******************************************************************/
    virtual bool setup(yarp::os::Property& property)
    {
        RTF_TEST_REPORT("Opening simulation clock, if it exists");
        yarp::os::Network::init();
        extClockName = "/clock";
        localPortName = "/NetworkClockTest/clock";
        RTF_ASSERT_ERROR_IF_FALSE(yarp::os::Network::exists(extClockName.c_str()), "/clock port not found");
        m_simClock = new yarp::os::NetworkClock();
        RTF_ASSERT_ERROR_IF_FALSE(m_simClock, "Simulation clock not correctly allocated.");
        RTF_ASSERT_ERROR_IF_FALSE(m_simClock->open(extClockName.c_str(), localPortName.c_str()), "Simulation clock not correctly opened");
        return true;
    }

    /******************************************************************/
    virtual void tearDown()
    {
        RTF_TEST_REPORT("Closing simulation clock");
        delete m_simClock;
        m_simClock = nullptr;

        yarp::os::Network::fini();
    }

    /******************************************************************/
    virtual void run()
    {
        for(int i=0; i < 10; i++)
        {
            if (m_simClock->isValid())
            {
                // If the clock is valid, exit from the for loop
                break;
            }
            RTF_TEST_REPORT("NetworkClock still not valid.");
            yarp::os::SystemClock::delaySystem(1.0);
        }

        RTF_TEST_FAIL_IF_FALSE(m_simClock->isValid(), "NetworkClock is not valid after 10 system seconds, test failing.");

        // Explicitly connect the ports and try again
        yarp::os::Network::connect(extClockName.c_str(), localPortName.c_str());
        yarp::os::SystemClock::delaySystem(1.0);
        RTF_TEST_FAIL_IF_FALSE(m_simClock->isValid(), "NetworkClock is not valid even after an explicit (non persistent) connection.");

        if (m_simClock->isValid())
        {
            RTF_TEST_REPORT("NetworkClock is valid after explicit connection.");
        }

    }
};

PREPARE_PLUGIN(NetworkClockTest)
