/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/impl/PortCoreInputUnit.h>

#include <yarp/os/Name.h>
#include <yarp/os/PortInfo.h>
#include <yarp/os/PortReport.h>
#include <yarp/os/Time.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/PlatformSignal.h>
#include <yarp/os/impl/PortCommand.h>

#include <cstdio>

//#define YMSG(x) printf x;
//#define YTRACE(x) YMSG(("at %s\n", x))


using namespace yarp::os::impl;
using namespace yarp::os;

PortCoreInputUnit::PortCoreInputUnit(PortCore& owner,
                                     int index,
                                     InputProtocol* ip,
                                     bool reversed) :
        PortCoreUnit(owner, index),
        ip(ip),
        phase(1),
        access(1),
        closing(false),
        finished(false),
        running(false),
        name(owner.getName()),
        localReader(nullptr),
        reversed(reversed)
{
    yAssert(ip != nullptr);

    yarp::os::PortReaderCreator* creator = owner.getReadCreator();
    if (creator != nullptr) {
        localReader = creator->create();
    }
}

PortCoreInputUnit::~PortCoreInputUnit()
{
    closeMain();
    if (localReader != nullptr) {
        delete localReader;
        localReader = nullptr;
    }
}


bool PortCoreInputUnit::start()
{


    YARP_DEBUG(Logger::get(), std::string("new input connection to ") + getOwner().getName() + " starting");

    /*
    if (ip!=nullptr) {
        Route route = ip->getRoute();
        YARP_DEBUG(Logger::get(), std::string("starting output for ") +
                   route.toString());
    }
    */

    phase.wait();

    bool result = PortCoreUnit::start();
    if (result) {
        YARP_DEBUG(Logger::get(), std::string("new input connection to ") + getOwner().getName() + " started ok");
        phase.wait();
        phase.post();
    } else {
        YARP_DEBUG(Logger::get(), std::string("new input connection to ") + getOwner().getName() + " failed to start");

        phase.post();
    }

    return result;
}


void PortCoreInputUnit::run()
{
    running = true;
    phase.post();

    Route route;
    bool wasNoticed = false;
    bool posted = false;

    bool done = false;

    yAssert(ip != nullptr);

    PortCommand cmd;

    bool ok = true;
    if (!reversed) {
        ip->open(getName());
    }
    if (!ok) {
        YARP_DEBUG(Logger::get(), std::string("new input connection to ") + getOwner().getName() + " is broken");
        done = true;
    } else {
        route = ip->getRoute();

        // just before going official, tag any lurking inputs from
        // the same source as undesired
        if (Name(route.getFromName()).isRooted()) {
            YARP_SPRINTF3(Logger::get(),
                          debug,
                          "Port %s starting up, flushing routes %s->*->%s",
                          getOwner().getName().c_str(),
                          route.getFromName().c_str(),
                          route.getToName().c_str());
            getOwner().removeIO(Route(route.getFromName(),
                                      route.getToName(),
                                      "*"),
                                true);
        }
        officialRoute = route;
        setMode();
        getOwner().reportUnit(this, true);

        std::string msg = std::string("Receiving input from ") + route.getFromName() + " to " + route.getToName() + " using " + route.getCarrierName();
        if (Name(route.getFromName()).isRooted()) {
            if (reversed || ip->getConnection().isPush()) {
                YARP_INFO(Logger::get(), msg);
                posted = true;
            } else {
                YARP_DEBUG(Logger::get(), msg);
            }
        } else {
            YARP_DEBUG(Logger::get(), msg);
        }

        // Report the new connection
        PortInfo info;
        info.message = msg;
        info.tag = yarp::os::PortInfo::PORTINFO_CONNECTION;
        info.incoming = true;
        info.created = true;
        info.sourceName = route.getFromName();
        info.targetName = route.getToName();
        info.portName = info.targetName;
        info.carrierName = route.getCarrierName();

        if (info.sourceName != "admin" && info.sourceName != "null") {
            getOwner().report(info);
            wasNoticed = true;
        }
    }

    if (!reversed) {
        if (!ip->getConnection().isPush()) {
            /* IP=OP */
            OutputProtocol* op = &(ip->getOutput());
            Route r = op->getRoute();
            // reverse route
            r.swapNames();
            op->rename(r);

            getOwner().addOutput(op);
            ip = nullptr;
            done = true;
        }
    }

    if (closing) {
        done = true;
    }

    void* id = (void*)this;

    if (ip != nullptr && !ip->getConnection().canEscape()) {
        InputStream* is = &ip->getInputStream();
        is->setReadEnvelopeCallback(envelopeReadCallback, this);
    }

    while (!done) {
        if (ip == nullptr) {
            break;
        }
        ConnectionReader& br = ip->beginRead();

        if (br.getReference() != nullptr) {
            //printf("HAVE A REFERENCE\n");
            if (localReader != nullptr) {
                bool ok = localReader->read(br);
                if (!br.isActive()) {
                    break;
                }
                if (!ok) {
                    continue;
                }
            } else {
                PortCore& man = getOwner();
                bool ok = man.readBlock(br, id, nullptr);
                if (!br.isActive()) {
                    break;
                }
                if (!ok) {
                    continue;
                }
            }
            //printf("DONE WITH A REFERENCE\n");
            if (ip != nullptr) {
                ip->endRead();
            }
            continue;
        }

        if (ip->getConnection().canEscape()) {
            bool ok = cmd.read(br);
            if (!br.isActive()) {
                break;
            }
            if (!ok) {
                continue;
            }
        } else {
            cmd = PortCommand('d', "");
            if (!ip->isOk()) {
                break;
            }
        }

        if (closing || isDoomed()) {
            break;
        }
        char key = cmd.getKey();
        //printf("Port command is [%c:%d/%s]\n",
        //         (key>=32)?key:'?', key, cmd.getText().c_str());

        PortCore& man = getOwner();
        OutputStream* os = nullptr;
        if (br.isTextMode()) {
            os = &(ip->getOutputStream());
        }

        switch (key) {
        case '/':
            YARP_SPRINTF3(Logger::get(),
                          debug,
                          "Port command (%s): %s should add connection: %s",
                          route.toString().c_str(),
                          getOwner().getName().c_str(),
                          cmd.getText().c_str());
            man.addOutput(cmd.getText(), id, os);
            break;
        case '!':
            YARP_SPRINTF3(Logger::get(),
                          debug,
                          "Port command (%s): %s should remove output: %s",
                          route.toString().c_str(),
                          getOwner().getName().c_str(),
                          cmd.getText().c_str());
            man.removeOutput(cmd.getText().substr(1, std::string::npos), id, os);
            break;
        case '~':
            YARP_SPRINTF3(Logger::get(),
                          debug,
                          "Port command (%s): %s should remove input: %s",
                          route.toString().c_str(),
                          getOwner().getName().c_str(),
                          cmd.getText().c_str());
            man.removeInput(cmd.getText().substr(1, std::string::npos), id, os);
            break;
        case '*':
            man.describe(id, os);
            break;
        case 'D':
        case 'd': {
            if (key == 'D') {
                ip->suppressReply();
            }

            std::string env = cmd.getText();
            if (env.length() > 2) {
                //YARP_ERROR(Logger::get(),
                //"***** received an envelope! [%s]", env.c_str());
                std::string env2 = env.substr(2, env.length());
                man.setEnvelope(env2);
                ip->setEnvelope(env2);
            }
            if (localReader != nullptr) {
                localReader->read(br);
                if (!br.isActive()) {
                    done = true;
                    break;
                }
            } else {
                if (ip->getReceiver().acceptIncomingData(br)) {
                    ConnectionReader* cr = &(ip->getReceiver().modifyIncomingData(br));
                    yarp::os::impl::PortDataModifier& modifier = getOwner().getPortModifier();
                    modifier.inputMutex.lock();
                    if (modifier.inputModifier != nullptr) {
                        if (modifier.inputModifier->acceptIncomingData(*cr)) {
                            cr = &(modifier.inputModifier->modifyIncomingData(*cr));
                            modifier.inputMutex.unlock();
                            man.readBlock(*cr, id, os);
                        } else {
                            modifier.inputMutex.unlock();
                            skipIncomingData(*cr);
                        }
                    } else {
                        modifier.inputMutex.unlock();
                        man.readBlock(*cr, id, os);
                    }
                } else {
                    skipIncomingData(br);
                }
                if (!br.isActive()) {
                    done = true;
                    break;
                }
            }
        } break;
        case 'a': {
            man.adminBlock(br, id, os);
        } break;
        case 'r':
            /*
              In YARP implementation, OP=IP.
              (This information is used rarely, and when used
              is tagged with OP=IP keyword)
              If it were not true, memory alloc would need to
              reorganized here
            */
            {
                OutputProtocol* op = &(ip->getOutput());
                ip->endRead();
                Route r = op->getRoute();
                // reverse route
                r.swapNames();
                op->rename(r);

                getOwner().addOutput(op);
                ip = nullptr;
                done = true;
            }
            break;
        case 'q':
            done = true;
            break;
#if !defined(NDEBUG)
        case 'i':
            printf("Interrupt requested\n");
            //yarp::os::impl::kill(0, 2); // SIGINT
            //yarp::os::impl::kill(Logger::get().getPid(), 2); // SIGINT
            yarp::os::impl::kill(Logger::get().getPid(), 15); // SIGTERM
            break;
#endif
        case '?':
        case 'h':
            if (os != nullptr) {
                BufferedConnectionWriter bw(true);
                bw.appendLine("This is a YARP port.  Here are the commands it responds to:");
                bw.appendLine("*       Gives a description of this port");
                bw.appendLine("d       Signals the beginning of input for the port's owner");
                bw.appendLine(R"(do      The same as "d" except replies should be suppressed ("data-only"))");
                bw.appendLine("q       Disconnects");
#if !defined(NDEBUG)
                bw.appendLine("i       Interrupt parent process (unix only)");
#endif
                bw.appendLine("r       Reverse connection type to be a reader");
                bw.appendLine("/port   Requests to send output to /port");
                bw.appendLine("!/port  Requests to stop sending output to /port");
                bw.appendLine("~/port  Requests to stop receiving input from /port");
                bw.appendLine("a       Signals the beginning of an administrative message");
                bw.appendLine("?       Gives this help");
                bw.write(*os);
            }
            break;
        default:
            if (os != nullptr) {
                BufferedConnectionWriter bw(true);
                bw.appendLine("Port command not understood.");
                bw.appendLine("Type d to send data to the port's owner.");
                bw.appendLine("Type ? for help.");
                bw.write(*os);
            }
            break;
        }
        if (ip != nullptr) {
            ip->endRead();
        }
        if (ip == nullptr) {
            break;
        }
        if (closing || isDoomed() || (!ip->isOk())) {
            break;
        }
    }

    setDoomed();

    YARP_DEBUG(Logger::get(), "PortCoreInputUnit closing ip");
    access.wait();
    if (ip != nullptr) {
        ip->close();
    }
    access.post();
    YARP_DEBUG(Logger::get(), "PortCoreInputUnit closed ip");

    std::string msg = std::string("Removing input from ") + route.getFromName() + " to " + route.getToName();

    if (Name(route.getFromName()).isRooted()) {
        if (posted) {
            YARP_INFO(Logger::get(), msg);
        }
    } else {
        YARP_DEBUG(Logger::get(), "PortCoreInputUnit (unrooted) shutting down");
    }

    getOwner().reportUnit(this, false);

    if (wasNoticed) {
        // Report the disappearing connection
        PortInfo info;
        info.message = msg;
        info.tag = yarp::os::PortInfo::PORTINFO_CONNECTION;
        info.incoming = true;
        info.created = false;
        info.sourceName = route.getFromName();
        info.targetName = route.getToName();
        info.portName = info.targetName;
        info.carrierName = route.getCarrierName();

        if (info.sourceName != "admin") {
            getOwner().report(info);
        }
    }

    if (localReader != nullptr) {
        delete localReader;
        localReader = nullptr;
    }

    running = false;
    finished = true;

    // it would be nice to get my entry removed from the port immediately,
    // but it would be a bit dodgy to delete this object and join this
    // thread within and from themselves
}

bool PortCoreInputUnit::isInput()
{
    return true;
}

void PortCoreInputUnit::close()
{
    closeMain();
}

bool PortCoreInputUnit::isFinished()
{
    return finished;
}

const std::string& PortCoreInputUnit::getName()
{
    return name;
}

bool PortCoreInputUnit::interrupt()
{
    // give a kick (unfortunately unavoidable)
    access.wait();
    if (!closing) {
        if (ip != nullptr) {
            ip->interrupt();
        }
        closing = true;
    }
    access.post();
    return true;
}

void PortCoreInputUnit::setCarrierParams(const yarp::os::Property& params)
{
    if (ip != nullptr) {
        ip->getReceiver().setCarrierParams(params);
    }
}

void PortCoreInputUnit::getCarrierParams(yarp::os::Property& params)
{
    if (ip != nullptr) {
        ip->getReceiver().getCarrierParams(params);
    }
}

// return the protocol object
InputProtocol* PortCoreInputUnit::getInPutProtocol()
{
    return ip;
}

void PortCoreInputUnit::closeMain()
{
    access.wait();
    Route r = getRoute();
    access.post();

    Logger log(r.toString().c_str(), Logger::get());

    YARP_DEBUG(log, "PortCoreInputUnit closing");

    if (running) {
        YARP_DEBUG(log, "PortCoreInputUnit joining");
        interrupt();
        join();
        YARP_DEBUG(log, "PortCoreInputUnit joined");
    }

    if (ip != nullptr) {
        ip->close();
        delete ip;
        ip = nullptr;
    }
    running = false;
    closing = false;
}


Route PortCoreInputUnit::getRoute()
{
    return officialRoute;
}


bool PortCoreInputUnit::skipIncomingData(yarp::os::ConnectionReader& reader)
{
    size_t pending = reader.getSize();
    if (pending > 0) {
        while (pending > 0) {
            char buf[10000];
            size_t next = (pending < sizeof(buf)) ? pending : sizeof(buf);
            reader.expectBlock(&buf[0], next);
            pending -= next;
        }
        return true;
    }
    return false;
}


bool PortCoreInputUnit::isBusy()
{
    bool busy = false;
    access.wait();
    if (ip != nullptr) {
        busy = ip->isReplying();
    }
    access.post();
    return busy;
}


void PortCoreInputUnit::envelopeReadCallback(void* data, const Bytes& envelope)
{
    auto* p = reinterpret_cast<PortCoreInputUnit*>(data);
    if (p == nullptr) {
        return;
    }
    p->getOwner().setEnvelope(envelope.get());
    p->ip->setEnvelope(envelope.get());
}
