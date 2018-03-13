/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/sig/Vector.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Port.h>
#include <yarp/os/Time.h>

//#include <vector>

#include <yarp/gsl/impl/gsl_structs.h>

#include "TestList.h"

using namespace yarp::os::impl;
using namespace yarp::os;
using namespace yarp::sig;


class VectorOfTest : public UnitTest {

public:
    virtual ConstString getName() override { return "VectorOfTest"; }
    void checkSendReceiveInt()
    {
        report(0, "check VectorO<int> send receive");

        {
            Port portIn;
            Port portOut;

            portOut.open("/harness_sig/vtest/o");
            portIn.open("/harness_sig/vtest/i");

            Network::connect("/harness_sig/vtest/o", "/harness_sig/vtest/i");

            portOut.enableBackgroundWrite(true);


            VectorOf<int> vector;
            vector.resize(10);
            for (unsigned int k = 0; k < vector.size(); k++)
            {
                vector[k] = k;
            }

            bool success = true;
            portOut.write(vector);

            VectorOf<int> tmp;
            portIn.read(tmp);

            //compare vector and tmp
            if (tmp.size() != vector.size())
            {
                success = false;
            }
            else
            {
                for (unsigned int k = 0; k < vector.size(); k++)
                {
                    if (tmp[k] != vector[k])
                        success = false;
                }
            }

            checkTrue(success, "VectorOf<int> was sent and received correctly");
            portOut.interrupt();
            portOut.close();
            portIn.interrupt();
            portIn.close();
        }

        report(0, "check VectorOf<int> bottle compatibility");
        {
            //write the same vector again and receive it as a bottle
            Port portIn;
            Port portOut;
            bool success = true;

            portOut.open("/harness_sig/vtest/o");
            portIn.open("/harness_sig/vtest/i");

            Network::connect("/harness_sig/vtest/o", "/harness_sig/vtest/i");

            portOut.enableBackgroundWrite(true);


            VectorOf<int> vector;
            vector.resize(10);
            for (unsigned int k = 0; k < vector.size(); k++)
            {
                vector[k] = k;
            }
            portOut.write(vector);
            Bottle tmp2;
            success = portIn.read(tmp2);
            checkTrue(success,"correctly read from the port");

            //compare vector and tmp
            success = true;
            if (tmp2.size() != (int)vector.size())
            {
                success = false;
            }
            else
            {
                for (unsigned int k = 0; k < vector.size(); k++)
                {
                    if (tmp2.get(k).asInt() != vector[k])
                        success = false;
                }
            }

            checkTrue(success, "VectorOf<int> was received correctly in a Bottle");
            portOut.interrupt();
            portOut.close();
            portIn.interrupt();
            portIn.close();
        }
    }


    virtual void runTests() override {
        Network::setLocalMode(true);
        checkSendReceiveInt();
        Network::setLocalMode(false);
    }
};

static VectorOfTest theVectorOfTest;

UnitTest& getVectorOfTest() {
    return theVectorOfTest;
}
