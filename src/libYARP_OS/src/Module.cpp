/*
 * Copyright (C) 2007 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_NO_DEPRECATED

#include <yarp/os/Module.h>

#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Property.h>
#include <yarp/os/Searchable.h>
#include <yarp/os/Network.h>
#include <yarp/os/Vocab.h>

#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/PlatformSignal.h>

#include <cstdio>
#include <cstdlib>


using namespace yarp::os::impl;
using namespace yarp::os;


class ModuleHelper : public yarp::os::PortReader,
                     public yarp::os::TypedReaderCallback<yarp::os::Bottle>,
                     public Thread,
                     public SearchMonitor {

private:
    Module& owner;

public:
    ModuleHelper(Module& owner) : owner(owner) {}

    /**
     * Handler for reading messages from the network, and passing
     * them on to the respond() method.
     * @param connection a network connection to a port
     * @return true if the message was read successfully
     */
    virtual bool read(yarp::os::ConnectionReader& connection) override;

    using yarp::os::TypedReaderCallback<yarp::os::Bottle>::onRead;
    /**
     * Alternative handler for reading messages from the network, and passing
     * them on to the respond() method.  There can be no replies made
     * if this handler is used.
     * @param v the message
     */
    virtual void onRead(yarp::os::Bottle& v) override {
        yarp::os::Bottle reply;
        owner.safeRespond(v, reply);
    }

    /**
     * Attach this object to a source of messages.
     * @param source a BufferedPort or PortReaderBuffer that
     * receives data.
     */
    bool attach(yarp::os::TypedReader<yarp::os::Bottle>& source,
                bool handleStream) {
        if (handleStream) {
            source.useCallback(*this);
        }
        source.setReplier(*this);
        return true;
    }


    bool attach(yarp::os::Port& source) {
        source.setReader(*this);
        return true;
    }

    virtual void run() override {
        printf("Listening to terminal (type \"quit\" to stop module)\n");
        bool isEof = false;
        while (!(isEof||isStopping()||owner.isStopping())) {
            ConstString str = NetworkBase::readString(&isEof);
            if (!isEof) {
                Bottle cmd(str.c_str());
                Bottle reply;
                bool ok = owner.safeRespond(cmd, reply);
                if (ok) {
                    //printf("ALL: %s\n", reply.toString().c_str());
                    //printf("ITEM 1: %s\n", reply.get(0).toString().c_str());
                    if (reply.get(0).toString()=="help") {
                        for (int i=0; i<reply.size(); i++) {
                            printf("%s\n",
                                           reply.get(i).toString().c_str());
                        }
                    } else {
                        printf("%s\n", reply.toString().c_str());
                    }
                } else {
                    printf("Command not understood -- %s\n", str.c_str());
                }
            }
        }
        //printf("terminal shutting down\n");
        //owner.interruptModule();
    }


    // SearchMonitor role

private:
    Property comment, fallback, present, actual, reported;
    Bottle order;
public:
    virtual void report(const SearchReport& report, const char *context) override {
        ConstString ctx = context;
        ConstString key = report.key.c_str();
        ConstString prefix = "";

        prefix = ctx;
        prefix += ".";

        key = prefix + key;
        if (key.substr(0, 1)==".") {
            key = key.substr(1, key.length());
        }

        if (!present.check(key.c_str())) {
            present.put(key.c_str(), "present");
            order.addString(key.c_str());
        }

        if (report.isFound) {
            actual.put(key.c_str(), report.value);
        }

        if (report.isComment==true) {
            comment.put(key.c_str(), report.value);
            return;
        }

        if (report.isDefault==true) {
            fallback.put(key.c_str(), report.value);
            return;
        }

        if (comment.check(key.c_str())) {
            if (!reported.check(key.c_str())) {
                if (report.isFound) {
                    ConstString hasValue = report.value.c_str();
                    if (hasValue.length()>35) {
                        hasValue = hasValue.substr(0, 30) + " ...";
                    }
                    printf("Checking \"%s\": = %s (%s)\n", key.c_str(),
                           hasValue.c_str(),
                           comment.check(key.c_str(),
                                         Value("")).toString().c_str());
                } else {
                    reported.put(key.c_str(), 1);
                    bool hasDefault = fallback.check(key.c_str());
                    ConstString defString = "";
                    if (hasDefault) {
                        defString += " ";
                        defString += "(default ";
                        ConstString theDefault =
                            fallback.find(key.c_str()).toString().c_str();
                        if (theDefault=="") {
                            defString += "is blank";
                        } else {
                            defString += theDefault;
                        }
                        defString += ")";
                    }
                    printf("Checking \"%s\": %s%s\n", key.c_str(),
                           comment.check(key.c_str(),
                                         Value("")).toString().c_str(),
                           defString.c_str());
                }
            }
        }
    }

    Bottle getOptions() {
        return order;
    }

    ConstString getComment(const char *option) {
        ConstString desc = comment.find(option).toString();
        return desc;
    }

    Value getDefaultValue(const char *option) {
        return fallback.find(option);
    }

    Value getValue(const char *option) {
        return actual.find(option);
    }

};


bool ModuleHelper::read(ConnectionReader& connection) {
    Bottle cmd, response;
    if (!cmd.read(connection)) { return false; }
    //printf("command received: %s\n", cmd.toString().c_str());
    bool result = owner.safeRespond(cmd, response);
    if (response.size()>=1) {
        ConnectionWriter *writer = connection.getWriter();
        if (writer!=YARP_NULLPTR) {
            if (response.get(0).toString()=="many") {
                for (int i=1; i<response.size(); i++) {
                    Value& v = response.get(i);
                    if (v.isList()) {
                        v.asList()->write(*writer);
                    } else {
                        Bottle b;
                        b.add(v);
                        b.write(*writer);
                    }
                }
            } else {
                response.write(*writer);
            }

            //printf("response sent: %s\n", response.toString().c_str());
        }
    }
    return result;
}



#define HELPER(x) (*((ModuleHelper*)(x)))

Module::Module() {
    stopFlag = false;
    implementation = new ModuleHelper(*this);
    yAssert(implementation!=YARP_NULLPTR);
}

Module::~Module() {
    if (implementation!=YARP_NULLPTR) {
        HELPER(implementation).stop();
        delete &HELPER(implementation);
        implementation = YARP_NULLPTR;
    }
}

double Module::getPeriod() {
    return 0.0;
}

bool Module::updateModule() {
    // insert a delay so, if user accidentally doesn't override this
    // method, the thread won't kill the processor
    yarp::os::Time::delay(0.5);
    return true;
}

bool Module::interruptModule() {
    return false;
}

bool Module::respond(const Bottle& command, Bottle& reply) {
    return basicRespond(command, reply);
}

bool Module::isStopping() {
    return stopFlag;
}

void Module::setName(const char *name) {
    this->name = name;
}

bool Module::basicRespond(const Bottle& command, Bottle& reply) {
    switch (command.get(0).asVocab()) {
    case VOCAB3('s', 'e', 't'):
        state.put(command.get(1).toString(), command.get(2));
        reply.addVocab(Vocab::encode("ack"));
        return true;
        break;
    case VOCAB3('g', 'e', 't'):
        reply.add(state.check(command.get(1).toString(), Value(0)));
        return true;
        break;
    case VOCAB4('q', 'u', 'i', 't'):
    case VOCAB4('e', 'x', 'i', 't'):
    case VOCAB3('b', 'y', 'e'):
        reply.addVocab(Vocab::encode("bye"));
        stopFlag = true;
        interruptModule();
        return true;
    default:
        reply.add("command not recognized");
        return false;
    }
    return false;
}

bool Module::safeRespond(const Bottle& command, Bottle& reply) {
    bool ok = respond(command, reply);
    if (!ok) {
        // just in case derived classes don't correctly pass on messages
        ok = basicRespond(command, reply);
    }
    return ok;
}


static Module *module = YARP_NULLPTR;
static bool terminated = false;
static void handler (int) {
    Time::useSystemClock();
    static int ct = 0;
    ct++;
    if (ct>3) {
        printf("Aborting...\n");
        std::exit(1);
    }
    printf("[try %d of 3] Trying to shut down\n",
                   ct);
    terminated = true;
    if (module!=YARP_NULLPTR) {
        Bottle cmd, reply;
        cmd.fromString("quit");
        module->safeRespond(cmd, reply);
        //printf("sent %s, got %s\n", cmd.toString().c_str(),
        //     reply.toString().c_str());
    }
}


bool Module::runModule() {
    if (module==YARP_NULLPTR) {
        module = this;
        //module = &HELPER(implementation);
    } else {
        printf("Module::runModule() signal handling currently only good for one module\n");
    }
    yarp::os::impl::signal(SIGINT, handler);
    yarp::os::impl::signal(SIGTERM, handler);
    while (updateModule()) {
        if (terminated) break;
        if (isStopping()) break;
        Time::delay(getPeriod());
        if (isStopping()) break;
        if (terminated) break;
    }
    printf("Module closing\n");
    close();
    printf("Module finished\n");
    if (1) { //terminated) {
        // only portable way to bring down a thread reading from
        // the keyboard -- no good way to interrupt.
        std::exit(1);
    }
    return true;
}



bool Module::attach(Port& port) {
    return HELPER(implementation).attach(port);
}

bool Module::attach(TypedReader<Bottle>& port, bool handleStream) {
    return HELPER(implementation).attach(port, handleStream);
}



bool Module::attachTerminal() {
    HELPER(implementation).start();
    return true;
}

int Module::runModule(int argc, char *argv[], bool skipFirst) {
    if (!openFromCommand(argc, argv, skipFirst)) {
        printf("Module failed to open\n");
        return 1;
    }
    attachTerminal();
    bool ok = runModule();
    close();
    return ok?0:1;
}

bool Module::openFromCommand(int argc, char *argv[], bool skipFirst) {
    Property options;
    options.fromCommand(argc, argv, skipFirst);

    options.setMonitor(&HELPER(implementation));
    // check if we're being asked to read the options from file
    Value *val;
    if (options.check("file", val, "configuration file to use, if any")) {
        ConstString fname = val->toString();
        options.unput("file");
        printf("Working with config file %s\n", fname.c_str());
        options.fromConfigFile(fname, false);

        // interpret command line options as a set of flags again
        // (just in case we need to override something)
        options.fromCommand(argc, argv, true, false);
    }

    // probably folloing options will be removed, so don't advertise them
    options.setMonitor(YARP_NULLPTR);
    // check if we want to use nested options (less ambiguous)
    if (options.check("nested", val)||options.check("lispy", val)) {
        ConstString lispy = val->toString();
        options.fromString(lispy);
    }
    options.setMonitor(&HELPER(implementation));

    name = options.check("name", Value(name), "name of module").asString();

    return open(options);
}


ConstString Module::getName(const char *subName) {
    if (subName==YARP_NULLPTR) {
        return name;
    }
    ConstString base = name.c_str();
    if (subName[0]!='/') {
      base += "/";
    }
    base += subName;
    return base.c_str();
}

#endif // YARP_NO_DEPRECATED
