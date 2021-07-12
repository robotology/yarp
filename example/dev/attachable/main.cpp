/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/sig/Vector.h>

#include <yarp/dev/IMultipleWrapper.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>

using yarp::dev::DeviceDriver;
using yarp::dev::DriverCreator;
using yarp::dev::DriverCreatorOf;
using yarp::dev::Drivers;
using yarp::dev::IMultipleWrapper;
using yarp::dev::PolyDriver;
using yarp::dev::PolyDriverList;
using yarp::os::Property;
using yarp::os::Searchable;

class MyWrapper :
        public IMultipleWrapper,
        public DeviceDriver
{
    PolyDriverList m_drivers;

public:
    bool open(Searchable& config) override
    {
        YARP_UNUSED(config);
        // parse options
        // probably here we tell the wrappers what devices it needs
        // "p1", "p2"...
        return true;
    }

    bool attachAll(const PolyDriverList& p) override
    {
        m_drivers = p;

        // we got the list of drivers, now we can do what we want
        // usually we will acquire the interfaces we need by
        // calling view() multiple times.


        // here we just do a simple check
        std::cout << "MyWrapper::attachAll\n";

        int nd = m_drivers.size();

        std::cout << "Received list of " << nd << " drivers:\n";

        bool allValid = true;
        for (int k = 0; k < nd; k++) {
            std::cout << m_drivers[k]->key.c_str();
            if (m_drivers[k]->poly) {
                // this could be a good place where to call
                // m_drivers[k]->poly.view() and get some interfaces
                std::cout << " is valid\n";
            } else {
                std::cout << " not valid\n";
                allValid = false;
            }
        }

        return allValid;
    }

    bool detachAll() override
    {
        //nothing to do
        std::cout << "Detaching all devices (actually doing nothing)\n";
        return true;
    }
};


int main(int argc, char* argv[])
{
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);

    //add new device to yarp
    DriverCreator* fakey_factory = new DriverCreatorOf<MyWrapper>("wrapper", "", "MyWrapper");
    Drivers::factory().add(fakey_factory); // hand factory over to YARP

    std::cout << "Testing multiple attachable interface/data structure\n";

    // create list of devices we are going to wrap
    PolyDriver p1;
    PolyDriver p2;
    PolyDriver p3;

    //open devices as usual
    //p1.open(...);
    //p2.open(...);

    // we should create the instance of our wrapper device
    PolyDriver wrapper;
    Property config;
    config.fromString("(device wrapper)");
    wrapper.open(config);

    //attach devices, first get interface
    IMultipleWrapper* iat;
    wrapper.view(iat);

    PolyDriverList list;
    list.push(&p1, "p1");
    list.push(&p2, "p2");
    list.push(&p3, "p3");

    iat->attachAll(list);

    iat->detachAll();

    std::cout << "Goodbye\n";

    return 0;
}
