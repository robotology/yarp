/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <yarp/os/all.h>

#include <yarp/os/Carrier.h>
#include <yarp/os/Carriers.h>

#include <yarp/os/impl/TextCarrier.h>

/**
 *
 * A minimal carrier variation - changes nothing except the first
 * 8-bytes of a regular text mode connection.
 *
 */
class TestCarrier : public yarp::os::impl::TextCarrier
{

public:
    virtual std::string getName() const override
    {
        return "test";
    }

    virtual std::string getSpecifierName() const override
    {
        return "TESTTEST";
    }

    virtual Carrier *create() const override
    {
        return new TestCarrier();
    }
};

int main(int argc, char *argv[])
{
    yarp::os::Network yarp;
    yarp::os::Carriers::addCarrierPrototype(new TestCarrier);

    yarp::os::BufferedPort<yarp::os::Bottle> out, in;
    out.open("/test/out");
    in.open("/test/in");

    yarp::os::Network::connect("/test/out","/test/in","test");

    out.prepare().fromString("1 2 3");
    out.write();

    yarp::os::Bottle * bot = in.read();
    if (bot!=NULL) {
        printf("Got message %s\n", bot->toString().c_str());
    }

    out.close();
    in.close();

    return 0;
}
