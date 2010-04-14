// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <ace/ACE.h>


#include <yarp/os/Network.h>
#include <yarp/os/Time.h>

#include <yarp/os/impl/Companion.h>
#include <yarp/os/impl/NameClient.h>
#include <yarp/os/impl/NameConfig.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/String.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Vocab.h>

#include <yarp/os/impl/InputStream.h>
#include <yarp/os/impl/OutputProtocol.h>
#include <yarp/os/impl/Carriers.h>
#include <yarp/os/impl/IOException.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/impl/StreamConnectionReader.h>
#include <yarp/os/impl/Route.h>
#include <yarp/os/impl/PortCommand.h>

using namespace yarp::os::impl;
using namespace yarp::os;

extern "C" void yarpCustomInit();
extern "C" void yarpCustomFini();

bool Network::connect(const char *src, const char *dest, 
                      const char *carrier, bool quiet) {
    int result = -1;
    if (carrier!=NULL) {
        // prepend carrier
        String fullDest = String(carrier) + ":/" + Companion::slashify(dest);
        result = Companion::connect(src,fullDest.c_str(),quiet);
    } else {
        result = Companion::connect(src,dest,quiet);
    }
    return result == 0;

}


bool Network::disconnect(const char *src, const char *dest, bool quiet) {
    int result = Companion::disconnect(src,dest,quiet);
    return result == 0;
}

bool Network::sync(const char *port, bool quiet) {
    //if (!result) {
    int result = Companion::wait(port,quiet);
    //}
    if (result==0) {
        Companion::poll(port,true);
    }
    return result == 0;
}


int Network::main(int argc, char *argv[]) {
    return Companion::main(argc,argv);
}


void Network::init() {

    // Broken pipes need to be dealt with through other means
    ACE_OS::signal(SIGPIPE, SIG_IGN);

    ACE::init();
    String quiet = NameConfig::getEnv("YARP_QUIET");
    Bottle b2(quiet.c_str());
    if (b2.get(0).asInt()>0) {
        Logger::get().setVerbosity(-b2.get(0).asInt());
    } else {
        String verbose = NameConfig::getEnv("YARP_VERBOSE");
        Bottle b(verbose.c_str());
        if (b.get(0).asInt()>0) {
            YARP_INFO(Logger::get(), 
                      "YARP_VERBOSE environment variable is set");
            Logger::get().setVerbosity(b.get(0).asInt());
        }
    }
    Logger::get().setPid();
	// make sure system is actually able to do things fast
	Time::turboBoost();

    yarpCustomInit();
}

void Network::fini() {
    yarpCustomFini();
    ACE::fini();
}

Contact Network::queryName(const char *name) {
    NameClient& nic = NameClient::getNameClient();
    Address address = nic.queryName(name);
    //printf("address is %s %d\n",address.toString().c_str(), address.isValid());
    return address.toContact();
}


Contact Network::registerName(const char *name) {
    NameClient& nic = NameClient::getNameClient();
    Address address = nic.registerName(name);
    return address.toContact();
}


Contact Network::registerContact(const Contact& contact) {
    NameClient& nic = NameClient::getNameClient();
    Address address = nic.registerName(contact.getName().c_str(),
                                       Address::fromContact(contact));
    return address.toContact();
}

Contact Network::unregisterName(const char *name) {
    NameClient& nic = NameClient::getNameClient();
    Address address = nic.unregisterName(name);
    return address.toContact();
}


Contact Network::unregisterContact(const Contact& contact) {
    NameClient& nic = NameClient::getNameClient();
    Address address = nic.unregisterName(contact.getName().c_str());
    return address.toContact();
}


bool Network::setProperty(const char *name,
                          const char *key,
                          const Value& value) {
    Bottle command;
    command.addString("bot");
    command.addString("set");
    command.addString(name);
    command.addString(key);
    command.add(value);
    Bottle reply;
    NameClient& nic = NameClient::getNameClient();
    nic.send(command,reply);
    return reply.size()>0;
}


Value *Network::getProperty(const char *name, const char *key) {
    Bottle command;
    command.addString("bot");
    command.addString("get");
    command.addString(name);
    command.addString(key);
    Bottle reply;
    NameClient& nic = NameClient::getNameClient();
    nic.send(command,reply);
    return Value::makeValue(reply.toString());
}


bool Network::setLocalMode(bool flag) {
    NameClient& nic = NameClient::getNameClient();
    bool state = nic.isFakeMode();
    nic.setFakeMode(flag);
    return state;
}


void Network::assertion(bool shouldBeTrue) {
    // could replace with ACE assertions, except should not 
    // evaporate in release mode
    YARP_ASSERT(shouldBeTrue);
}


ConstString Network::readString(bool *eof) {
    return ConstString(Companion::readString(eof).c_str());
}



bool Network::write(const Contact& contact, 
                    PortWriter& cmd,
                    PortReader& reply,
                    bool admin,
                    bool quiet) {

    // This is a little complicated because we make the connection
    // without using a port ourselves.  With a port it is easy.

    const char *connectionName = "anon";
    ConstString name = contact.getName();
    const char *targetName = name.c_str();  // use carefully!
    Address address = Address::fromContact(contact);
    if (!address.isValid()) {
        NameClient& nic = NameClient::getNameClient();
        address = nic.queryName(targetName);
    }
    if (!address.isValid()) {
        if (!quiet) {
            YARP_ERROR(Logger::get(),"could not find port");
        }
        return false;
    }
    
    OutputProtocol *out = Carriers::connect(address);
    if (out==NULL) {
        if (!quiet) {
            YARP_ERROR(Logger::get(),"cannot connect to port");
        }
        return false;
    }

    //printf("RPC connection to %s at %s (connection name %s)\n", targetName, 
    //     address.toString().c_str(),
    //     connectionName);
    //Route r(connectionName,targetName,"text_ack");
    Route r(connectionName,targetName,"text_ack");
    out->open(r);
    
    PortCommand pc(0,admin?"a":"d");
    BufferedConnectionWriter bw(out->isTextMode());
    bool ok = pc.write(bw);
    if (!ok) {
        if (!quiet) {
            YARP_ERROR(Logger::get(),"could not write to connection");
        }
        if (out!=NULL) delete out;
        return false;
    }
    ok = cmd.write(bw);
    if (!ok) {
        if (!quiet) {
            YARP_ERROR(Logger::get(),"could not write to connection");
        }
        if (out!=NULL) delete out;
        return false;
    }
    bw.setReplyHandler(reply);
    out->write(bw);
    //InputProtocol& ip = out->getInput();
    //ConnectionReader& reader = ip.beginRead();
    //reply.read(reader);
    //ip.endRead();
    if (out!=NULL) {
        delete out;
        out = NULL;
    }
    return true;
}


bool Network::isConnected(const char *src, const char *dest, bool quiet) {
    Contact contact = Contact::byName(src);
    Bottle cmd, reply;
    cmd.addVocab(Vocab::encode("list"));
    cmd.addVocab(Vocab::encode("out"));
    Network::write(contact,cmd,reply,true);
    for (int i=0; i<reply.size(); i++) {
        if (reply.get(i).asString()==dest) {
            if (!quiet) {
                printf("Connection from %s to %s found\n",
                       src, dest);
            }
            return true;
        }
    }
    if (!quiet) {
        printf("No connection from %s to %s found\n",
               src, dest);
    }
    return false;
}


ConstString Network::getNameServerName() {
    NameConfig nc;
    String name = nc.getNamespace();
    return name.c_str();
}


Contact Network::getNameServerContact() {
    NameClient& nic = NameClient::getNameClient();
    return nic.getAddress().toContact();
}



bool Network::setNameServerName(const char *name) {
    NameConfig nc;
    String fname = nc.getConfigFileName(YARP_CONFIG_NAMESPACE_FILENAME);
    nc.writeConfig(fname,String(name));
    nc.getNamespace(true);
    NameClient& client = NameClient::getNameClient();
    return client.updateAddress();
}


bool Network::checkNetwork() {
    Contact c = queryName(getNameServerName());
    //printf("Contact is %s %d\n", c.toString().c_str(), c.isValid());
    return c.isValid();
}
