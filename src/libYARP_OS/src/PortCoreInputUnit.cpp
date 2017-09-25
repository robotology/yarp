/*
 * Copyright (C) 2006, 2007 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/impl/PortCoreInputUnit.h>

#include <yarp/os/Name.h>
#include <yarp/os/PortReport.h>
#include <yarp/os/PortInfo.h>
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

bool PortCoreInputUnit::start() {


    YARP_DEBUG(Logger::get(), ConstString("new input connection to ")+
               getOwner().getName()+ " starting");

    /*
    if (ip!=YARP_NULLPTR) {
        Route route = ip->getRoute();
        YARP_DEBUG(Logger::get(), ConstString("starting output for ") +
                   route.toString());
    }
    */

    phase.wait();

    bool result = PortCoreUnit::start();
    if (result) {
        YARP_DEBUG(Logger::get(), ConstString("new input connection to ")+
                   getOwner().getName()+ " started ok");
        phase.wait();
        phase.post();
    } else {
        YARP_DEBUG(Logger::get(), ConstString("new input connection to ")+
                   getOwner().getName()+ " failed to start");

        phase.post();
    }

    return result;
}


void PortCoreInputUnit::run() {
    running = true;
    phase.post();

    Route route;
    bool wasNoticed = false;
    bool posted = false;

    bool done = false;

    yAssert(ip!=YARP_NULLPTR);

    PortCommand cmd;

    bool ok = true;
    if (!reversed) {
        ip->open(getName().c_str());
    }
    if (!ok) {
            YARP_DEBUG(Logger::get(), ConstString("new input connection to ")+
                    getOwner().getName()+ " is broken");
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
                                        route.getToName(), "*"), true);
        }
        officialRoute = route;
        setMode();
        getOwner().reportUnit(this, true);

        ConstString msg = ConstString("Receiving input from ") +
            route.getFromName() + " to " + route.getToName() +
            " using " +
            route.getCarrierName();
        if (Name(route.getFromName()).isRooted()) {
            if (reversed||ip->getConnection().isPush()) {
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
        info.sourceName = route.getFromName().c_str();
        info.targetName = route.getToName().c_str();
        info.portName = info.targetName;
        info.carrierName = route.getCarrierName().c_str();

        if (info.sourceName!="admin"&&info.sourceName!="null") {
            getOwner().report(info);
            wasNoticed = true;
        }
    }

    if (!reversed) {
        if (!ip->getConnection().isPush()) {
            /* IP=OP */
            OutputProtocol *op = &(ip->getOutput());
            Route r = op->getRoute();
            // reverse route
            r.swapNames();
            op->rename(r);

            getOwner().addOutput(op);
            ip = YARP_NULLPTR;
            done = true;
        }
    }

    if (closing) {
        done = true;
    }

    void *id = (void *)this;

    if (ip!=YARP_NULLPTR && !ip->getConnection().canEscape()) {
        InputStream *is = &ip->getInputStream();
        is->setReadEnvelopeCallback(envelopeReadCallback, this);
    }

    while (!done) {
        ConnectionReader& br = ip->beginRead();

        if (br.getReference()!=YARP_NULLPTR) {
            //printf("HAVE A REFERENCE\n");
            if (localReader!=YARP_NULLPTR) {
                bool ok = localReader->read(br);
                if (!br.isActive()) { break; }
                if (!ok) continue;
            } else {
                PortManager& man = getOwner();
                bool ok = man.readBlock(br, id, YARP_NULLPTR);
                if (!br.isActive()) { break; }
                if (!ok) continue;
            }
            //printf("DONE WITH A REFERENCE\n");
            if (ip!=YARP_NULLPTR) {
                ip->endRead();
            }
            continue;
        }

        if (ip->getConnection().canEscape()) {
            bool ok = cmd.read(br);
            if (!br.isActive()) { break; }
            if (!ok) continue;
        } else {
            cmd = PortCommand('d', "");
            if (!ip->isOk()) { break; }
        }

        if (closing||isDoomed()) {
            break;
        }
        char key = cmd.getKey();
        //printf("Port command is [%c:%d/%s]\n",
        //         (key>=32)?key:'?', key, cmd.getText().c_str());

        PortManager& man = getOwner();
        OutputStream *os = YARP_NULLPTR;
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
            man.removeOutput(cmd.getText().substr(1, ConstString::npos), id, os);
            break;
        case '~':
            YARP_SPRINTF3(Logger::get(),
                          debug,
                          "Port command (%s): %s should remove input: %s",
                          route.toString().c_str(),
                          getOwner().getName().c_str(),
                          cmd.getText().c_str());
            man.removeInput(cmd.getText().substr(1, ConstString::npos), id, os);
            break;
        case '*':
            man.describe(id, os);
            break;
        case 'D':
        case 'd':
            {
                if (key=='D') {
                    ip->suppressReply();
                }

                ConstString env = cmd.getText();
#ifndef YARP_NO_DEPRECATED // since YARP 2.3.68
                bool suppressed = false;
                if (env.length()>1) {
                    if (!suppressed) {
                        // This is the backwards-compatible
                        // method for signalling replies are
                        // not expected.  To be used until
                        // YARP 2.1.2 is a "long time ago".
                        if (env[1]=='o') {
                            ip->suppressReply();
                        }
                    }
                    if (env.length()>2) {
                        //YARP_ERROR(Logger::get(),
                        //"***** received an envelope! [%s]", env.c_str());
                        ConstString env2 = env.substr(2, env.length());
                        man.setEnvelope(env2);
                        ip->setEnvelope(env2);
                    }
                }
#else // YARP_NO_DEPRECATED
                if (env.length()>2) {
                    //YARP_ERROR(Logger::get(),
                    //"***** received an envelope! [%s]", env.c_str());
                    ConstString env2 = env.substr(2, env.length());
                    man.setEnvelope(env2);
                    ip->setEnvelope(env2);
                }
#endif // YARP_NO_DEPRECATED
                if (localReader) {
                    localReader->read(br);
                    if (!br.isActive()) { done = true; break; }
                } else {
                    if (ip->getReceiver().acceptIncomingData(br)) {
                        ConnectionReader* cr = &(ip->getReceiver().modifyIncomingData(br));
                        yarp::os::impl::PortDataModifier& modifier = getOwner().getPortModifier();
                        modifier.inputMutex.lock();
                        if (modifier.inputModifier) {
                            if (modifier.inputModifier->acceptIncomingData(*cr)) {
                                cr = &(modifier.inputModifier->modifyIncomingData(*cr));
                                modifier.inputMutex.unlock();
                                man.readBlock(*cr, id, os);
                            }
                            else {
                                modifier.inputMutex.unlock();
                                skipIncomingData(*cr);
                            }
                        }
                        else {
                            modifier.inputMutex.unlock();
                            man.readBlock(*cr, id, os);
                        }
                    }
                    else
                        skipIncomingData(br);
                    if (!br.isActive()) { done = true; break; }
                }
            }
            break;
        case 'a':
            {
                man.adminBlock(br, id, os);
            }
            break;
        case 'r':
            /*
              In YARP implementation, OP=IP.
              (This information is used rarely, and when used
              is tagged with OP=IP keyword)
              If it were not true, memory alloc would need to
              reorganized here
            */
            {
                OutputProtocol *op = &(ip->getOutput());
                ip->endRead();
                Route r = op->getRoute();
                // reverse route
                r.swapNames();
                op->rename(r);

                getOwner().addOutput(op);
                ip = YARP_NULLPTR;
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
            if (os!=YARP_NULLPTR) {
                BufferedConnectionWriter bw(true);
                bw.appendLine("This is a YARP port.  Here are the commands it responds to:");
                bw.appendLine("*       Gives a description of this port");
                bw.appendLine("d       Signals the beginning of input for the port's owner");
                bw.appendLine("do      The same as \"d\" except replies should be suppressed (\"data-only\")");
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
            if (os!=YARP_NULLPTR) {
                BufferedConnectionWriter bw(true);
                bw.appendLine("Port command not understood.");
                bw.appendLine("Type d to send data to the port's owner.");
                bw.appendLine("Type ? for help.");
                bw.write(*os);
            }
            break;
        }
        if (ip!=YARP_NULLPTR) {
            ip->endRead();
        }
        if (ip==YARP_NULLPTR) {
            break;
        }
        if (closing||isDoomed()||(!ip->isOk())) {
            break;
        }
    }

    setDoomed();

    YARP_DEBUG(Logger::get(), "PortCoreInputUnit closing ip");
    access.wait();
    if (ip!=YARP_NULLPTR) {
        ip->close();
    }
    access.post();
    YARP_DEBUG(Logger::get(), "PortCoreInputUnit closed ip");

    ConstString msg = ConstString("Removing input from ") +
        route.getFromName() + " to " + route.getToName();

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
        info.message = msg.c_str();
        info.tag = yarp::os::PortInfo::PORTINFO_CONNECTION;
        info.incoming = true;
        info.created = false;
        info.sourceName = route.getFromName().c_str();
        info.targetName = route.getToName().c_str();
        info.portName = info.targetName;
        info.carrierName = route.getCarrierName().c_str();

        if (info.sourceName!="admin") {
            getOwner().report(info);
        }
    }

    if (localReader!=YARP_NULLPTR) {
        delete localReader;
        localReader = YARP_NULLPTR;
    }

    running = false;
    finished = true;

    // it would be nice to get my entry removed from the port immediately,
    // but it would be a bit dodgy to delete this object and join this
    // thread within and from themselves
}


bool PortCoreInputUnit::interrupt() {
    // give a kick (unfortunately unavoidable)
    access.wait();
    if (!closing) {
        if (ip!=YARP_NULLPTR) {
            ip->interrupt();
        }
        closing = true;
    }
    access.post();
    return true;
}

void PortCoreInputUnit::closeMain() {
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

    if (ip!=YARP_NULLPTR) {
        ip->close();
        delete ip;
        ip = YARP_NULLPTR;
    }
    running = false;
    closing = false;
}


Route PortCoreInputUnit::getRoute() {
    return officialRoute;
}


bool PortCoreInputUnit::skipIncomingData(yarp::os::ConnectionReader& reader) {
    size_t pending = reader.getSize();
    if (pending>0) {
        while (pending>0) {
            char buf[10000];
            size_t next = (pending<sizeof(buf))?pending:sizeof(buf);
            reader.expectBlock(&buf[0], next);
            pending -= next;
        }
        return true;
    }
    return false;
}


bool PortCoreInputUnit::isBusy() {
    bool busy = false;
    access.wait();
    if (ip!=YARP_NULLPTR) {
        busy = ip->isReplying();
    }
    access.post();
    return busy;
}


void PortCoreInputUnit::envelopeReadCallback(void* data, const Bytes& envelope)
{
    PortCoreInputUnit *p = reinterpret_cast<PortCoreInputUnit*>(data);
    if (!p) {
        return;
    }
    p->getOwner().setEnvelope(envelope.get());
    p->ip->setEnvelope(envelope.get());
}
