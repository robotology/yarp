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
class CompanionMergeInput :
        public TypedReaderCallback<Bottle>
{
public:
    Contactable *port{nullptr};
    Semaphore *sema{nullptr};
    std::mutex mutex;

    Bottle value;
    Stamp stamp;

    CompanionMergeInput() = default;

    void init(Contactable& port, Semaphore& sema) {
        this->port = &port;
        this->sema = &sema;
    }

    using yarp::os::TypedReaderCallback<Bottle>::onRead;
    void onRead(Bottle& datum) override {
        mutex.lock();
        value = datum;
        port->getEnvelope(stamp);
        mutex.unlock();
        sema->post();
    }
};
} // namespace

int Companion::cmdMerge(int argc, char *argv[])
{
    BufferedPort<Bottle >   outPort;
    BufferedPort<Bottle >*  inPort = nullptr;
    CompanionMergeInput *   inData = nullptr;
    yarp::os::Stamp         outStamp;

    int nPorts = argc;
    if (nPorts == 0) {
        yCInfo(COMPANION, "This is yarp merge. Please provide a list of ports to read from, e.g:");
        yCInfo(COMPANION, "  yarp merge /port1 /port2");
        yCInfo(COMPANION, "Alternative syntax:");
        yCInfo(COMPANION, "  yarp merge --input /p1 /p2 --output /p3 --worker /prefix --carrier udp");
        return -1;
    }

    Property options;
    options.fromCommand(argc, argv, false);
    Bottle& inputs = options.findGroup("input");
    if (!inputs.isNull()) {
        nPorts = inputs.size()-1;
    }

    inPort  = new BufferedPort<Bottle > [nPorts];
    inData  = new CompanionMergeInput   [nPorts];

    Semaphore product(0);

    //set a callback
    for (int i = 0; i< nPorts; i++) {
        inData[i].init(inPort[i], product);
        inPort[i].useCallback(inData[i]);
    }

    //open the ports
    char buff[255];
    std::string s = options.check("worker", Value("/portsMerge/i")).asString();
    for (int i = 0; i< nPorts; i++) {
        sprintf(buff, "%s%d", s.c_str(), i);
        inPort[i].open(buff);
    }
    s = options.check("output", Value("/portsMerge/o0")).asString();
    outPort.open(s);

    //makes the connection
    for (int i=0; i<nPorts; i++) {
        std::string tmp;
        if (!inputs.isNull()) {
            tmp = inputs.get(i+1).asString();
        } else {
            tmp = argv[i];
        }
        bool b = yarp::os::NetworkBase::connect(tmp.c_str(), inPort[i].getName().c_str(), options.check("carrier", Value("udp")).asString().c_str(), false);
        if (!b) {
            delete [] inPort;
            delete [] inData;
            return -1;
        }
    }

    yCInfo(COMPANION, "Ready. Output goes to %s", outPort.getName().c_str());
    while(true) {
        product.wait();
        while (product.check()) {
            product.wait();
        }

        //write
        outStamp.update();
        if (outPort.getOutputCount()>0) {
            Bottle &out=outPort.prepare();
            out.clear();
            for (int i = 0; i< nPorts; i++) {
                inData[i].mutex.lock();
                out.append(inData[i].value);
                inData[i].mutex.unlock();
            }
            outPort.setEnvelope(outStamp);
            outPort.write();
        }
    }

    delete [] inPort  ;
    delete [] inData  ;
    return 0;
}
