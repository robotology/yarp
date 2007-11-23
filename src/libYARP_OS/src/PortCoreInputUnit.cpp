// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/os/Time.h>
#include <yarp/PortCoreInputUnit.h>
#include <yarp/PortCommand.h>
#include <yarp/Logger.h>
#include <yarp/BufferedConnectionWriter.h>
#include <yarp/Name.h>
#include <yarp/os/Time.h>
#include <yarp/os/PortReport.h>


#define YMSG(x) ACE_OS::printf x;
#define YTRACE(x) YMSG(("at %s\n",x))


using namespace yarp;
using namespace yarp::os;

bool PortCoreInputUnit::start() {

    if (ip!=NULL) {
        Route route = ip->getRoute();
        YARP_DEBUG(Logger::get(),String("starting output for ") + 
                   route.toString());
    }

    phase.wait();

    bool result = PortCoreUnit::start();
    if (result) {
        phase.wait();
        phase.post();
    } else {
        phase.post();
    }

    return result;
}


void PortCoreInputUnit::run() {
    running = true;
    phase.post();

    Route route;
    bool wasNoticed = false;

    try {
        bool done = false;

        YARP_ASSERT(ip!=NULL);

        PortCommand cmd;
  
        if (autoHandshake) {
            ip->open(getName().c_str());
            route = ip->getRoute();
            String msg = String("Receiving input from ") + 
                route.getFromName() + " to " + route.getToName() + 
                " using " +
                route.getCarrierName();
            if (Name(route.getFromName()).isRooted()) {
                YARP_INFO(Logger::get(),msg);
            }

            // Report the new connection
            PortInfo info;
            info.message = msg.c_str();
            info.tag = yarp::os::PortInfo::PORTINFO_CONNECTION;
            info.incoming = true;
            info.created = true;
            info.sourceName = route.getFromName().c_str();
            info.targetName = route.getToName().c_str();
            info.portName = info.targetName;
            info.carrierName = route.getCarrierName().c_str();

            if (info.sourceName!="admin") {
                getOwner().report(info);
                wasNoticed = true;
            }

        } else {
            ip->open(""); // anonymous connection
            route = ip->getRoute();
        }

        if (closing) {
            done = true;
        }

        void *id = (void *)this;

        while (!done) {
            try {
                ConnectionReader& br = ip->beginRead();

                if (br.getReference()!=NULL) {
                    //printf("HAVE A REFERENCE\n");
                    if (localReader!=NULL) {
                        localReader->read(br);
                    } else {
                        PortManager& man = getOwner();
                        man.readBlock(br,id,NULL);
                    }
                    //printf("DONE WITH A REFERENCE\n");
                    if (ip!=NULL) {
                        ip->endRead();
                    }
                    continue;
                }

                if (autoHandshake&&(ip->canEscape())) {
                    cmd.readBlock(br);
                } else {
                    cmd = PortCommand('d',"");
                }

                if (closing||isDoomed()) {
                    done = true;
                    break;
                }
                char key = cmd.getKey();
                //ACE_OS::printf("Port command is [%c:%d/%s]\n",
                //	     (key>=32)?key:'?', key, cmd.getText().c_str());
	
                PortManager& man = getOwner();
                OutputStream *os = NULL;
                if (br.isTextMode()) {
                    os = &(ip->getOutputStream());
                }
	
                switch (key) {
                case '/':
                    YARP_DEBUG(Logger::get(),String("asking to add output to ")+
                               cmd.getText());
                    man.addOutput(cmd.getText(),id,os);
                    break;
                case '!':
                    man.removeOutput(cmd.getText().substring(1,String::npos),id,os);
                    break;
                case '~':
                    man.removeInput(cmd.getText().substring(1,String::npos),id,os);
                    break;
                case '*':
                    man.describe(id,os);
                    break;
                case 'D':
                case 'd':
                    {
                        bool suppressed = false;
                        
                        // this will be the new way to signal that
                        // replies are not expected.
                        if (key=='D') {
                            ip->suppressReply();
                        }

                        try {
                            String env = cmd.getText();
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
                                    //"***** received an envelope!");
                                    man.setEnvelope(env.substr(2,env.length()));
                                }
                            }
                            if (localReader) {
                                localReader->read(br);
                            } else {
                                man.readBlock(br,id,os);
                            }
                        } catch (IOException e) {
                            YARP_DEBUG(Logger::get(),e.toString() + " <<< user level PortCoreInputUnit exception, passing on");
                            done = true;
                            throw e;
                        }
                    }
                    break;
                case 'a':
                    {
                        try {
                            man.adminBlock(br,id,os);
                        } catch (IOException e) {
                            YARP_DEBUG(Logger::get(),e.toString() + " <<< admin level PortCoreInputUnit exception, passing on");
                            done = true;
                            throw e;
                        }
                    }
                    break;
                case 'r':
                    /*
                      In YARP implementation, OP = IP.
                      This is the one place we use that information.
                      If it were not true, memory alloc would need to
                      reorganized here
                     */
                    {
                        OutputProtocol *op = &(ip->getOutput());
                        ip->endRead();
                        Route r = op->getRoute();
                        // reverse route
                        op->rename(Route().addFromName(r.getToName()).addToName(r.getFromName()).addCarrierName(r.getCarrierName()));
                                   
                        getOwner().addOutput(op);
                        ip = NULL;
                        done = true;
                    }
                    break;
                case 'q':
                    done = true;
                    break;
                case 'i':
                    printf("Interrupt requested\n");
                    //ACE_OS::kill(0,2); // SIGINT
                    //ACE_OS::kill(Logger::get().getPid(),2); // SIGINT
                    ACE_OS::kill(Logger::get().getPid(),15); // SIGTERM
                    break;
                case '?':
                case 'h':
                    if (os!=NULL) {
                        BufferedConnectionWriter bw(true);
                        bw.appendLine("This is a YARP port.  Here are the commands it responds to:");
                        bw.appendLine("*       Gives a description of this port");
                        bw.appendLine("d       Signals the beginning of input for the port's owner");
                        bw.appendLine("do      The same as \"d\" except replies should be suppressed (\"data-only\")");
                        bw.appendLine("q       Disconnects");
                        bw.appendLine("i       Interrupt parent process (unix only)");
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
                    if (os!=NULL) {
                        BufferedConnectionWriter bw(true);
                        bw.appendLine("Port command not understood.");
                        bw.appendLine("Type d to send data to the port's owner.");
                        bw.appendLine("Type ? for help.");
                        bw.write(*os);
                    }
                    break;
                }
                if (ip!=NULL) {
                    ip->endRead();
                }
            } catch (IOException e) {
                YARP_DEBUG(Logger::get(),e.toString() + " <<< initial PortCoreInputUnit exception");
                if (!ip->checkStreams()) {
                    // pass it on
                    YARP_DEBUG(Logger::get(), "passing on exception");
                    throw e;
                } else {
                    // clear out any garbage
                    ip->resetStreams();
                }
            }
            if (ip==NULL) {
                done = true;
                break;
            }
            if (closing||isDoomed()||(!ip->checkStreams())) {
                done = true;
                break;
            }
        }
    } catch (IOException e) {
        /* ok, ports die - it is their nature */
        YARP_DEBUG(Logger::get(),e.toString() + " <<< PortCoreInputUnit exception");
    }

    setDoomed(true);
  
    YARP_DEBUG(Logger::get(),"PortCoreInputUnit closing ip");
    access.wait();
    if (ip!=NULL) {
        ip->close();
    }
    access.post();
    YARP_DEBUG(Logger::get(),"PortCoreInputUnit closed ip");

    if (autoHandshake) {
        String msg = String("Removing input from ") + 
            route.getFromName() + " to " + route.getToName();

        if (Name(route.getFromName()).isRooted()) {
            YARP_INFO(Logger::get(),msg);
		} else {
	        YARP_DEBUG(Logger::get(),"PortCoreInputUnit (unrooted) shutting down");
		}

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

    } else {
        YARP_DEBUG(Logger::get(),"PortCoreInputUnit shutting down");
    }

    if (localReader!=NULL) {
        delete localReader;
        localReader = NULL;
    }

    running = false;
    finished = true;

    // it would be nice to get my entry removed from the port immediately,
    // but it would be a bit dodgy to delete this object and join this
    // thread within and from themselves
}



void PortCoreInputUnit::runSimulation() {
    /*
    // simulation
    running = true;
    while (true) {
    ACE_OS::printf("tick\n");
    Time::delay(0.3);
    if (closing) {
    break;
    }
    }
    */

    ACE_OS::printf("stopping\n");

    running = false;
    finished = true;
}


void PortCoreInputUnit::closeMain() {
    YARP_DEBUG(Logger::get(),"PortCoreInputUnit closing");

    if (running) {
        // give a kick (unfortunately unavoidable)
        access.wait();
        if (ip!=NULL) {
            YARP_DEBUG(Logger::get(),"PortCoreInputUnit interrupting");
            //while (running) {
            //YARP_DEBUG(Logger::get(),"PortCoreInputUnit interrupt pulse");
            ip->interrupt();
            //  Time::delay(0.01);
            //}
            YARP_DEBUG(Logger::get(),"PortCoreInputUnit interrupted");
        }
        closing = true;
        access.post();
        YARP_DEBUG(Logger::get(),"PortCoreInputUnit joining");
        join();
        YARP_DEBUG(Logger::get(),"PortCoreInputUnit joined");
    }

    if (ip!=NULL) {
        try {
            ip->close();
        } catch (IOException e) { /*ok*/ }
        try {
            delete ip;
        } catch (IOException e) { /*ok*/ }
        ip = NULL;
    }
    running = false;
    closing = false;
    //finished = false;
    //setDoomed(false);
}


Route PortCoreInputUnit::getRoute() {
    if (ip!=NULL) {
        return ip->getRoute();
    }
    return PortCoreUnit::getRoute();
}


