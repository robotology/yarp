/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/companion/impl/Companion.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Contactable.h>
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/TypedReaderCallback.h>
#include <yarp/os/Value.h>

using yarp::companion::impl::Companion;
using yarp::os::Bottle;
using yarp::os::BufferedPort;
using yarp::os::Contactable;
using yarp::os::NetworkBase;
using yarp::os::Property;
using yarp::os::Semaphore;
using yarp::os::Stamp;
using yarp::os::TypedReaderCallback;
using yarp::os::Value;

namespace {
class inPortProcessor :  public TypedReaderCallback<Bottle>
{
public:
    Semaphore*   sema{nullptr};
    Contactable* port{nullptr};
    std::mutex   mutex;
    Bottle       value;
    size_t       value_size{0};
    Stamp        stamp;

    inPortProcessor() = default;

    void init(Contactable& port, Semaphore& sema)
    {
        this->port = &port;
        this->sema = &sema;
    }

    using yarp::os::TypedReaderCallback<Bottle>::onRead;
    void onRead(Bottle& datum) override
    {
        mutex.lock();
        value = datum;
        value_size= value.size();
        port->getEnvelope(stamp);
        mutex.unlock();
        sema->post();
    }
};
} // namespace

int Companion::cmdSplit(int argc, char *argv[])
{
    BufferedPort<Bottle >*  outPort = nullptr;
    BufferedPort<Bottle >   inPort;
    inPortProcessor*        inData = nullptr;

    //a simple help
    if (argc == 0)
    {
        yCInfo(COMPANION, "This is yarp split. Please provide the name of the port to process, e.g:");
        yCInfo(COMPANION, "  yarp split /port1");
        return -1;
    }

    //some options
    std::string portname = argv[0];
    Property options;
    options.fromCommand(argc, argv, false);
    std::string carrier = options.check("carrier", Value("udp")).asString().c_str();

    //initializations
    inData  = new inPortProcessor;
    Semaphore product(0);
    inData->init(inPort, product);
    inPort.useCallback(*inData);

    //open the input port and try the connection to the remote
    inPort.open("...");
    bool b = yarp::os::NetworkBase::connect(portname, inPort.getName().c_str(), carrier, false);
    if (!b)
    {
        yCError(COMPANION, "Failed to connect.");
        return -1;
    }

    yCInfo(COMPANION, "Waiting for data..");
    size_t siz = 0;
    while(true)
    {
        //wait for new data to be received
        product.wait();
        while (product.check()) {
            product.wait();
        }

        //open some ports. The number depends on the first received data.
        if (outPort == nullptr)
        {
            inData->mutex.lock();
            siz = inData->value_size;
            inData->mutex.unlock();
            if (siz != 0)
            {
                yCInfo(COMPANION, std::string("Received a Bottle of "+std::to_string(siz)+" elements.").c_str());
                outPort = new BufferedPort<Bottle> [siz];
                for (size_t i = 0; i < siz; i++)
                {
                    std::string portName = portname+"/split" + std::to_string(i) +":o";
                    outPort[i].open(portName);
                }
                yCInfo(COMPANION, "Ready");
            }
            else
            {
                yCError(COMPANION, "Data with invalid size received.");
            }
        }

        //Check if the data size is not constant
        inData->mutex.lock();
        if (inData->value_size != siz)
        {
            yCError(COMPANION, "Data with invalid size received.");
        }
        inData->mutex.unlock();

        //write data on each single port
        for (size_t i = 0; i < siz; i++)
        {
            if (outPort[i].getOutputCount() > 0)
            {
                Bottle& b = outPort[i].prepare();
                b.clear();
                inData->mutex.lock();
                b.add(inData->value.get(i));
                outPort[i].setEnvelope(inData->stamp);
                inData->mutex.unlock();
                outPort[i].write();
            }
        }
    }
    yCInfo(COMPANION, "Complete.");

    //cleanup
    delete [] outPort;
    delete inData;
    return 0;
}
