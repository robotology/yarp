// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/Network.h>
#include <yarp/os/Time.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Port.h>

#include <yarp/os/impl/Companion.h>
#include <yarp/os/impl/NameClient.h>
#include <yarp/os/impl/NameConfig.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/String.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/DummyConnector.h>
#include <yarp/os/NameSpace.h>
#include <yarp/os/MultiNameSpace.h>

#include <yarp/os/InputStream.h>
#include <yarp/os/impl/OutputProtocol.h>
#include <yarp/os/impl/Carriers.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/impl/StreamConnectionReader.h>
#include <yarp/os/impl/Route.h>
#include <yarp/os/impl/PortCommand.h>
#include <yarp/os/impl/NameConfig.h>
#include <yarp/os/impl/ThreadImpl.h>
#include <yarp/os/impl/PlatformStdio.h>
#include <yarp/os/impl/PlatformSignal.h>

#ifdef YARP_HAS_ACE
#include <ace/config.h>
#include <ace/String_Base.h>
#endif

#include <stdlib.h>

using namespace yarp::os::impl;
using namespace yarp::os;

static int __yarp_is_initialized = 0;

static MultiNameSpace *__multi_name_space = NULL;

static MultiNameSpace& getNameSpace() {
    if (__multi_name_space == NULL) {
        __multi_name_space = new MultiNameSpace;
        YARP_ASSERT(__multi_name_space!=NULL);
    }
    return *__multi_name_space;
}

static void removeNameSpace() {
    if (__multi_name_space!=NULL) {
        delete __multi_name_space;
        __multi_name_space = NULL;
    }
}

static bool needsLookup(const Contact& contact) {
    if (contact.getHost()!="") return false;
    if (contact.getCarrier()=="topic") return false;
    return true;
}

static int noteDud(const Contact& src) {
    NameStore *store = getNameSpace().getQueryBypass();
    if (store!=NULL) {
        return store->announce(src.getName().c_str(),0);
    }
    Bottle cmd, reply;
    cmd.addString("announce");
    cmd.addString(src.getName().c_str());
    cmd.addInt(0);
    bool ok = NetworkBase::write(NetworkBase::getNameServerContact(),
                                 cmd,
                                 reply);
    return ok?0:1;
 }

static int enactConnection(const Contact& src,
                           const Contact& dest,
                           const ContactStyle& style,
                           int mode,
                           bool reversed) {
    ContactStyle rpc;
    rpc.admin = true;
    rpc.quiet = style.quiet;
    rpc.timeout = style.timeout;

    if (style.persistent) {
        bool ok = false;
        // we don't talk to the ports, we talk to the nameserver
        NameSpace& ns = getNameSpace();
        if (mode==YARP_ENACT_CONNECT) {
            ok = ns.connectPortToPortPersistently(src,dest,style);
        } else if (mode==YARP_ENACT_DISCONNECT) {
            ok = ns.disconnectPortToPortPersistently(src,dest,style);
        } else {
            fprintf(stderr,"Failure: cannot check subscriptions yet\n");
            return 1;
        }
        if (!ok) {
            return 1;
        }
        if (!style.quiet) {
            fprintf(stderr,"Success: port-to-port persistent connection added.\n");
        }
        return 0;
    }

    if (mode==YARP_ENACT_EXISTS) {
        Bottle cmd, reply;
        cmd.addVocab(Vocab::encode("list"));
        cmd.addVocab(Vocab::encode(reversed?"in":"out"));
        cmd.addString(dest.getName().c_str());
        YARP_SPRINTF2(Logger::get(),debug,"asking %s: %s",
                      src.toString().c_str(), cmd.toString().c_str());
        bool ok = NetworkBase::write(src,cmd,reply,rpc);
        if (!ok) {
            noteDud(src);
            return 1;
        }
        if (reply.check("carrier")) {
            if (!style.quiet) {
                printf("Connection found between %s and %s\n",
                       src.getName().c_str(), dest.getName().c_str());
            }
            return 0;
        }
        return 1;
    }

    int act = (mode==YARP_ENACT_DISCONNECT)?VOCAB3('d','e','l'):VOCAB3('a','d','d');

    // Let's ask the destination to connect/disconnect to the source.
    // We assume the YARP carrier will reverse the connection if
    // appropriate when connecting.
    Bottle cmd, reply;
    cmd.addVocab(act);
    Contact c = dest;
    if (style.carrier!="") {
        c = c.addCarrier(style.carrier);
    }
    if (mode!=YARP_ENACT_DISCONNECT) {
        cmd.addString(c.toString());
    } else {
        cmd.addString(c.getName());
    }

    Contact c2 = src;
    if (c2.getPort()<=0) {
        c2 = NetworkBase::queryName(c2.getName());
    }
    if (c2.getCarrier()!="tcp") {
        YARP_SPRINTF2(Logger::get(),debug,"would have asked %s: %s",
                      src.toString().c_str(), cmd.toString().c_str());
        return 1;
    }

    YARP_SPRINTF2(Logger::get(),debug,"** asking %s: %s",
                  src.toString().c_str(), cmd.toString().c_str());
    bool ok = NetworkBase::write(c2,cmd,reply,rpc);
    if (!ok) {
        noteDud(src);
        return 1;
    }
    ok = false;
    ConstString msg = "";
    if (reply.get(0).isInt()) {
        ok = (reply.get(0).asInt()==0);
        msg = reply.get(1).asString();
    } else {
        // older protocol
        msg = reply.get(0).asString();
        ok = msg[0]=='A'||msg[0]=='R';
    }
    if (mode==YARP_ENACT_DISCONNECT && !ok) {
        msg = "no such connection\n";
    }
    if (mode==YARP_ENACT_CONNECT && !ok) {
        noteDud(dest);
    }
    if (!style.quiet) {
        fprintf(stderr,"%s %s",
                ok?"Success:":"Failure:",
                msg.c_str());
    }
    return ok?0:1;
}

/*

   Connect two ports, bearing in mind that one of them may not be
   a regular YARP port.

   Normally, YARP sends a request to the source port asking it to
   connect to the destination port.  But the source port may not
   be capable of initiating connections, in which case we can
   request the destination port to connect to the source (this
   is appropriate for carriers that can reverse the initiative).

   The source or destination could also be topic ports, which are
   entirely virtual.  In that case, we just need to tell the name
   server, and it will take care of the details.

*/

static int metaConnect(const ConstString& src,
                       const ConstString& dest,
                       ContactStyle style,
                       int mode) {
    YARP_SPRINTF3(Logger::get(),debug,
                  "working on connection %s to %s (%s)",
                  src.c_str(),
                  dest.c_str(),
                  (mode==YARP_ENACT_CONNECT)?"connect":((mode==YARP_ENACT_DISCONNECT)?"disconnect":"check")
                  );

    // get the expressed contacts, without name server input
    Contact dynamicSrc = Contact::fromString(src);
    Contact dynamicDest = Contact::fromString(dest);

    bool topical = style.persistent;
    if (dynamicSrc.getCarrier()=="topic" ||
        dynamicDest.getCarrier()=="topic") {
        topical = true;
    }

    bool topicalNeedsLookup = !getNameSpace().connectionHasNameOfEndpoints();

    // fetch completed contacts from name server, if needed
    Contact staticSrc;
    Contact staticDest;
    if (needsLookup(dynamicSrc)&&(topicalNeedsLookup||!topical)) {
        staticSrc = NetworkBase::queryName(dynamicSrc.getName());
        if (!staticSrc.isValid()) {
            if (!style.persistent) {
                if (!style.quiet) {
                    fprintf(stderr, "Failure: could not find source port %s\n",
                            src.c_str());
                }
                return 1;
            } else {
                staticSrc = dynamicSrc;
            }
        }
    } else {
        staticSrc = dynamicSrc;
    }
    if (staticSrc.getCarrier()=="") {
        staticSrc = staticSrc.addCarrier("tcp");
    }
    if (staticDest.getCarrier()=="") {
        staticDest = staticDest.addCarrier("tcp");
    }

    if (needsLookup(dynamicDest)&&(topicalNeedsLookup||!topical)) {
        staticDest = NetworkBase::queryName(dynamicDest.getName());
        if (!staticDest.isValid()) {
            if (!style.persistent) {
                if (!style.quiet) {
                    fprintf(stderr, "Failure: could not find destination port %s\n",
                            dest.c_str());
                }
                return 1;
            } else {
                staticDest = dynamicDest;
            }
        }
    } else {
        staticDest = dynamicDest;
    }

    if (staticSrc.getCarrier()=="xmlrpc" &&
        (staticDest.getCarrier()=="xmlrpc"||(staticDest.getCarrier().find("rossrv")==0))&&
        mode==YARP_ENACT_CONNECT) {
        // Unconnectable in general
        // Let's assume the first part is a YARP port, and use "tcp" instead
        staticSrc = staticSrc.addCarrier("tcp");
        staticDest = staticDest.addCarrier("tcp");
    }

    ConstString carrierConstraint = "";

    // see if we can do business with the source port
    bool srcIsCompetent = false;
    bool srcIsTopic = false;
    if (staticSrc.getCarrier()!="topic") {
        if (!topical) {
            Carrier *srcCarrier = NULL;
            if (staticSrc.getCarrier()!="") {
                srcCarrier = Carriers::chooseCarrier(staticSrc.getCarrier().c_str());
            }
            if (srcCarrier!=NULL) {
                String srcBootstrap = srcCarrier->getBootstrapCarrierName();
                if (srcBootstrap!="") {
                    srcIsCompetent = true;
                } else {
                    carrierConstraint = staticSrc.getCarrier();
                }
                delete srcCarrier;
                srcCarrier = NULL;
            }
        }
    } else {
        srcIsTopic = true;
    }

    // see if we can do business with the destination port
    bool destIsCompetent = false;
    bool destIsTopic = false;
    if (staticDest.getCarrier()!="topic") {
        if (!topical) {
            Carrier *destCarrier = NULL;
            if (staticDest.getCarrier()!="") {
                destCarrier = Carriers::chooseCarrier(staticDest.getCarrier().c_str());
            }
            if (destCarrier!=NULL) {
                String destBootstrap = destCarrier->getBootstrapCarrierName();
                if (destBootstrap!="") {
                    destIsCompetent = true;
                } else {
                    carrierConstraint = staticDest.getCarrier();
                }
                delete destCarrier;
                destCarrier = NULL;
            }
        }
    } else {
        destIsTopic = true;
    }

    if (srcIsTopic||destIsTopic) {
        Bottle cmd, reply;
        NameSpace& ns = getNameSpace();

        bool ok = false;
        if (srcIsTopic) {
            if (mode==YARP_ENACT_CONNECT) {
                ok = ns.connectTopicToPort(staticSrc,staticDest,style);
            } else if (mode==YARP_ENACT_DISCONNECT) {
                ok = ns.disconnectTopicFromPort(staticSrc,staticDest,style);
            } else {
                fprintf(stderr,"Failure: cannot check subscriptions yet\n");
                return 1;
            }
        } else {
            if (mode==YARP_ENACT_CONNECT) {
                ok = ns.connectPortToTopic(staticSrc,staticDest,style);
            } else if (mode==YARP_ENACT_DISCONNECT) {
                ok = ns.disconnectPortFromTopic(staticSrc,staticDest,style);
            } else {
                fprintf(stderr,"Failure: cannot check subscriptions yet\n");
                return 1;
            }
        }
        if (!ok) {
            return 1;
        }
        if (!style.quiet) {
            fprintf(stderr,"Success: connection to topic added.\n");
        }
        return 0;
    }

    if (dynamicSrc.getCarrier()!="") {
        style.carrier = dynamicSrc.getCarrier();
    }

    if (dynamicDest.getCarrier()!="") {
        style.carrier = dynamicDest.getCarrier();
    }


    if (style.carrier!="" && carrierConstraint!="") {
        if (style.carrier!=carrierConstraint) {
            fprintf(stderr,"Failure: conflict between %s and %s\n",
                    style.carrier.c_str(),
                    carrierConstraint.c_str());
            return 1;
        }
    }
    if (carrierConstraint!="") {
        style.carrier = carrierConstraint;
    }
    if (style.carrier=="") {
        style.carrier = staticDest.getCarrier();
    }
    if (style.carrier=="") {
        style.carrier = staticSrc.getCarrier();
    }

    bool connectionIsPush = false;
    bool connectionIsPull = false;
    Carrier *connectionCarrier = NULL;
    if (style.carrier!="topic") {
        connectionCarrier = Carriers::chooseCarrier(style.carrier.c_str());
        if (connectionCarrier!=NULL) {
            connectionIsPush = connectionCarrier->isPush();
            connectionIsPull = !connectionIsPush;
        }
    }

    int result = -1;
    if ((srcIsCompetent&&connectionIsPush)||topical) {
        // Classic case.
        Contact c = Contact::fromString(dest);
        if (connectionCarrier!=NULL) delete connectionCarrier;
        return enactConnection(staticSrc,c,style,mode,false);
    }
    if (destIsCompetent&&connectionIsPull) {
        Contact c = Contact::fromString(src);
        if (connectionCarrier!=NULL) delete connectionCarrier;
        return enactConnection(staticDest,c,style,mode,true);
    }

    if (connectionCarrier!=NULL) {
        if (!connectionIsPull) {
            Contact c = Contact::fromString(dest);
            result = connectionCarrier->connect(staticSrc,c,style,mode,false);
        } else {
            Contact c = Contact::fromString(src);
            result = connectionCarrier->connect(staticDest,c,style,mode,true);
        }
    }
    if (connectionCarrier!=NULL) {
        delete connectionCarrier;
        connectionCarrier = NULL;
    }
    if (result!=-1) {
        if (!style.quiet) {
            if (result==0) {
                printf("Success: added connection using custom carrier method\n");
            } else {
                printf("Failure: custom carrier method did not work\n");
            }
        }
        return result;
    }

    if (mode!=YARP_ENACT_DISCONNECT) {
        fprintf(stderr,"Failure: no method known for this connection type: [%s]->[%s]\n", staticSrc.toString().c_str(), staticDest.toString().c_str());
    }

    return 1;
}

bool NetworkBase::connect(const ConstString& src, const ConstString& dest,
                          const ConstString& carrier,
                          bool quiet) {
    ContactStyle style;
    style.quiet = quiet;
    if (carrier!="") {
        style.carrier = carrier;
    }
    return connect(src,dest,style);
}

bool NetworkBase::connect(const ConstString& src,
                          const ConstString& dest,
                          const ContactStyle& style) {
    int result = metaConnect(src,dest,style,YARP_ENACT_CONNECT);
    return result == 0;
}

bool NetworkBase::disconnect(const ConstString& src,
                             const ConstString& dest,
                             bool quiet) {
    ContactStyle style;
    style.quiet = quiet;
    return disconnect(src,dest,style);
}

bool NetworkBase::disconnect(const ConstString& src,
                             const ConstString& dest,
                             const ContactStyle& style) {
    int result = metaConnect(src,dest,style,YARP_ENACT_DISCONNECT);
    return result == 0;
}

bool NetworkBase::isConnected(const ConstString& src,
                              const ConstString& dest,
                              bool quiet) {
    ContactStyle style;
    style.quiet = quiet;
    return isConnected(src,dest,style);
}

bool NetworkBase::exists(const ConstString& port, bool quiet) {
    ContactStyle style;
    style.quiet = quiet;
    return exists(port,style);
}

bool NetworkBase::exists(const ConstString& port, const ContactStyle& style) {
    int result = Companion::exists(port.c_str(),style);
    if (result==0) {
        //Companion::poll(port,true);
        ContactStyle style2 = style;
        style2.admin = true;
        Bottle cmd("[ver]"), resp;
        bool ok = NetworkBase::write(Contact::byName(port),cmd,resp,style2);
        if (!ok) result = 1;
        if (resp.get(0).toString()!="ver") {
            result = 1;
        }
    }
    return result == 0;
}


bool NetworkBase::sync(const ConstString& port, bool quiet) {
    int result = Companion::wait(port.c_str(),quiet);
    if (result==0) {
        Companion::poll(port.c_str(),true);
    }
    return result == 0;
}

int NetworkBase::main(int argc, char *argv[]) {
    return Companion::main(argc,argv);
}

int NetworkBase::runNameServer(int argc, char *argv[]) {
    // call the yarp standard companion name server
    argc--;
    argv++;
    int result = Companion::getInstance().cmdServer(argc,argv);
    return result;
}



void NetworkBase::initMinimum() {
    if (__yarp_is_initialized==0) {
        // Broken pipes need to be dealt with through other means
        ACE_OS::signal(SIGPIPE, SIG_IGN);

#ifdef YARP_HAS_ACE
        ACE::init();
#endif
        ConstString quiet = getEnvironment("YARP_QUIET");
        Bottle b2(quiet.c_str());
        if (b2.get(0).asInt()>0) {
            Logger::get().setVerbosity(-b2.get(0).asInt());
        } else {
            ConstString verbose = getEnvironment("YARP_VERBOSE");
            Bottle b(verbose.c_str());
            if (b.get(0).asInt()>0) {
                YARP_INFO(Logger::get(),
                          "YARP_VERBOSE environment variable is set");
                Logger::get().setVerbosity(b.get(0).asInt());
            }
        }
        ConstString stack = getEnvironment("YARP_STACK_SIZE");
        if (stack!="") {
            int sz = atoi(stack.c_str());
            Thread::setDefaultStackSize(sz);
            YARP_SPRINTF1(Logger::get(), info,
                          "YARP_STACK_SIZE set to %d", sz);
        }
        Logger::get().setPid();
        // make sure system is actually able to do things fast
        Time::turboBoost();

        // prepare carriers
        Carriers::getInstance();
    }
    __yarp_is_initialized++;
}

void NetworkBase::finiMinimum() {
    if (__yarp_is_initialized==1) {
        Carriers::removeInstance();
        NameClient::removeNameClient();
        removeNameSpace();
#ifdef YARP_HAS_ACE
        ACE::fini();
#endif
    }
    if (__yarp_is_initialized>0) __yarp_is_initialized--;
}

Contact NetworkBase::queryName(const ConstString& name) {
    YARP_SPRINTF1(Logger::get(),debug,"query name %s",name.c_str());
    if (getNameServerName()==name) {
        YARP_SPRINTF1(Logger::get(),debug,"query recognized as name server: %s",name.c_str());
        return getNameServerContact();
    }
    Contact c = c.fromString(name);
    if (c.isValid()&&c.getPort()>0) {
        return c;
    }
    return getNameSpace().queryName(name);
}


Contact NetworkBase::registerName(const ConstString& name) {
    YARP_SPRINTF1(Logger::get(),debug,"register name %s",name.c_str());
    return getNameSpace().registerName(name);
}


Contact NetworkBase::registerContact(const Contact& contact) {
    YARP_SPRINTF1(Logger::get(),debug,"register contact %s",
                  contact.toString().c_str());
    return getNameSpace().registerContact(contact);
}

Contact NetworkBase::unregisterName(const ConstString& name) {
    return getNameSpace().unregisterName(name);
}


Contact NetworkBase::unregisterContact(const Contact& contact) {
    return getNameSpace().unregisterContact(contact);
}


bool NetworkBase::setProperty(const char *name,
                              const char *key,
                              const Value& value) {
    return getNameSpace().setProperty(name,key,value);
}


Value *NetworkBase::getProperty(const char *name, const char *key) {
    return getNameSpace().getProperty(name,key);
}


bool NetworkBase::setLocalMode(bool flag) {
    return getNameSpace().setLocalMode(flag);
}

bool NetworkBase::getLocalMode() {
    NameSpace& ns = getNameSpace();
    return ns.localOnly();
}

void NetworkBase::assertion(bool shouldBeTrue) {
    // could replace with ACE assertions, except should not
    // evaporate in release mode
    YARP_ASSERT(shouldBeTrue);
}


ConstString NetworkBase::readString(bool *eof) {
    return ConstString(Companion::readString(eof).c_str());
}

bool NetworkBase::write(const Contact& contact,
                       PortWriter& cmd,
                       PortReader& reply,
                       bool admin,
                       bool quiet,
                       double timeout) {
    ContactStyle style;
    style.admin = admin;
    style.quiet = quiet;
    style.timeout = timeout;
    return write(contact,cmd,reply,style);
}

bool NetworkBase::write(const Contact& contact,
                        PortWriter& cmd,
                        PortReader& reply,
                        const ContactStyle& style) {
    if (!getNameSpace().serverAllocatesPortNumbers()) {
        // switch to more up-to-date method

        Port port;
        port.setAdminMode(style.admin);
        port.openFake("network_write");
        Contact ec = contact;
        if (style.carrier!="") {
            ec = ec.addCarrier(style.carrier);
        }
        if (!port.addOutput(ec.toString().c_str())) {
            if (!style.quiet) {
                ACE_OS::fprintf(stderr, "Cannot make connection to '%s'\n",
                                ec.toString().c_str());
            }
            return false;
        }

        bool ok = port.write(cmd,reply);
        /*
        DummyConnector con;
        cmd.write(con.getWriter());
        Bottle in, out;
        in.read(con.getReader());
        bool ok = port.write(cmd,out);
        out.write(con.getCleanWriter());
        reply.read(con.getReader());

        YARP_SPRINTF3(Logger::get(),
                      debug,
                      "NETWORK WROTE: %s: [%s] -> [%s]",
                      ec.toString().c_str(),
                      in.toString().c_str(),
                      out.toString().c_str());
        */

        return ok;
    }

    const char *connectionName = "admin";
    ConstString name = contact.getName();
    const char *targetName = name.c_str();  // use carefully!
    Address address = Address::fromContact(contact);
    if (!address.isValid()) {
        Contact c = getNameSpace().queryName(targetName);
        address = Address::fromContact(c);
    }
    if (!address.isValid()) {
        if (!style.quiet) {
            YARP_SPRINTF1(Logger::get(),error,
                          "cannot find port %s",
                          targetName);
        }
        return false;
    }

    if (style.timeout>0) {
        address.setTimeout((float)style.timeout);
    }
    OutputProtocol *out = Carriers::connect(address);
    if (out==NULL) {
        if (!style.quiet) {
            YARP_SPRINTF1(Logger::get(),error,
                          "Cannot connect to port %s",
                          targetName);
        }
        return false;
    }
    if (style.timeout>0) {
        out->setTimeout(style.timeout);
    }

    Route r(connectionName,targetName,
            (style.carrier!="")?style.carrier.c_str():"text_ack");
    out->open(r);

    PortCommand pc(0,style.admin?"a":"d");
    BufferedConnectionWriter bw(out->getConnection().isTextMode());
    bool ok = true;
    if (out->getConnection().canEscape()) {
        ok = pc.write(bw);
    }
    if (!ok) {
        if (!style.quiet) {
            YARP_ERROR(Logger::get(),"could not write to connection");
        }
        if (out!=NULL) delete out;
        return false;
    }
    ok = cmd.write(bw);
    if (!ok) {
        if (!style.quiet) {
            YARP_ERROR(Logger::get(),"could not write to connection");
        }
        if (out!=NULL) delete out;
        return false;
    }
    if (style.expectReply) {
        bw.setReplyHandler(reply);
    }
    out->write(bw);
    if (out!=NULL) {
        delete out;
        out = NULL;
    }
    return true;
}

bool NetworkBase::write(const ConstString& port_name,
                               PortWriter& cmd,
                               PortReader& reply) {
    return write(Contact::byName(port_name),cmd,reply);
}

bool NetworkBase::isConnected(const ConstString& src, const ConstString& dest,
                              const ContactStyle& style) {
    int result = metaConnect(src,dest,style,YARP_ENACT_EXISTS);
    if (result!=0) {
        if (!style.quiet) {
            printf("No connection from %s to %s found\n",
                   src.c_str(), dest.c_str());
        }
    }
    return result == 0;
}


ConstString NetworkBase::getNameServerName() {
    NameConfig nc;
    String name = nc.getNamespace(false);
    return name.c_str();
}


Contact NetworkBase::getNameServerContact() {
    return getNameSpace().getNameServerContact();
}



bool NetworkBase::setNameServerName(const ConstString& name) {
    NameConfig nc;
    String fname = nc.getConfigFileName(YARP_CONFIG_NAMESPACE_FILENAME);
    nc.writeConfig(fname,name + "\n");
    nc.getNamespace(true);
    getNameSpace().activate(true);
    return true;
}


bool NetworkBase::checkNetwork() {
    return getNameSpace().checkNetwork();
}


bool NetworkBase::checkNetwork(double timeout) {
    return getNameSpace().checkNetwork(timeout);
}


bool NetworkBase::initialized() {
    return __yarp_is_initialized>0;
}


void NetworkBase::setVerbosity(int verbosity) {
    Logger::get().setVerbosity(verbosity);
}

void NetworkBase::queryBypass(NameStore *store) {
    getNameSpace().queryBypass(store);
}


ConstString NetworkBase::getEnvironment(const char *key,
                                        bool *found) {
    const char *result = ACE_OS::getenv(key);
    if (found != NULL) {
        *found = (result!=NULL);
    }
    if (result == NULL) {
        return "";
    }
    return result;
}

void NetworkBase::setEnvironment(const ConstString& key, const ConstString& val) {
#if defined(WIN32)
    _putenv_s(key.c_str(),val.c_str());
#else
    ACE_OS::setenv(key.c_str(),val.c_str(),1);
#endif
}

void NetworkBase::unsetEnvironment(const ConstString& key) {
#if defined(WIN32)
    _putenv_s(key.c_str(),"");
#else
    ACE_OS::unsetenv(key.c_str());
#endif
}

ConstString NetworkBase::getDirectorySeparator() {
#ifdef _WIN32
    // note this may be wrong under cygwin
    // should be ok for mingw
    return "\\";
#else
    return "/";
#endif
}

ConstString NetworkBase::getPathSeparator() {
#ifdef _WIN32
    // note this may be wrong under cygwin
    // should be ok for mingw
    return ";";
#else
    return ":";
#endif
}

void NetworkBase::lock() {
    ThreadImpl::threadMutex.wait();
}

void NetworkBase::unlock() {
    ThreadImpl::threadMutex.post();
}


#ifdef YARP_HAS_ACE

#include <yarp/os/YarpPlugin.h>

class ForwardingCarrier : public Carrier {
public:
    SharedLibraryClassFactory<Carrier> *factory;
    SharedLibraryClass<Carrier> car;
    Carrier *owner;

    ForwardingCarrier() {
        owner = NULL;
        factory = NULL;
    }

    ForwardingCarrier(SharedLibraryClassFactory<Carrier> *factory,
                      Carrier *owner) :
        factory(factory),
        owner(owner)
    {
        factory->addRef();
        car.open(*factory);
    }

    virtual ~ForwardingCarrier() {
        car.close();
        if (!factory) return;
        factory->removeRef();
        if (factory->getReferenceCount()<=0) {
            delete factory;
        }
        factory = NULL;
    }

    bool isValid() {
        return car.isValid();
    }

    virtual Carrier& getContent() {
        return car.getContent();
    }

    virtual Carrier *create() {
        return owner->create();
    }

    virtual String getName() {
        return getContent().getName();
    }

    virtual bool checkHeader(const yarp::os::Bytes& header) {
        return getContent().checkHeader(header);
    }

    virtual void setParameters(const yarp::os::Bytes& header) {
        getContent().setParameters(header);
    }

    virtual void getHeader(const yarp::os::Bytes& header) {
        getContent().getHeader(header);
    }


    virtual bool isConnectionless() {
        return getContent().isConnectionless();
    }


    virtual bool canAccept() {
        return getContent().canAccept();
    }


    virtual bool canOffer() {
        return getContent().canOffer();
    }


    virtual bool isTextMode() {
        return getContent().isTextMode();
    }


    virtual bool canEscape() {
        return getContent().canEscape();
    }

    virtual bool requireAck() {
        return getContent().requireAck();
    }


    virtual bool supportReply() {
        return getContent().supportReply();
    }


    virtual bool isLocal() {
        return getContent().isLocal();
    }


    virtual bool isPush() {
        return getContent().isPush();
    }

    virtual bool prepareSend(Protocol& proto) {
        return getContent().prepareSend(proto);
    }

    virtual bool sendHeader(Protocol& proto) {
        return getContent().sendHeader(proto);
    }

    virtual bool expectReplyToHeader(Protocol& proto) {
        return getContent().expectReplyToHeader(proto);
    }

    //virtual bool sendIndex(Protocol& proto,SizedWriter& writer) {
    //return getContent().sendIndex(proto,writer);
    //}

    virtual bool write(Protocol& proto, SizedWriter& writer) {
        return getContent().write(proto,writer);
    }

    virtual bool reply(Protocol& proto, SizedWriter& writer) {
        return getContent().reply(proto,writer);
    }

    virtual bool expectExtraHeader(Protocol& proto) {
        return getContent().expectExtraHeader(proto);
    }

    virtual bool respondToHeader(Protocol& proto){
        return getContent().respondToHeader(proto);
    }

    virtual bool expectIndex(Protocol& proto) {
        return getContent().expectIndex(proto);
    }

    virtual bool expectSenderSpecifier(Protocol& proto) {
        return getContent().expectSenderSpecifier(proto);
    }

    virtual bool sendAck(Protocol& proto) {
        return getContent().sendAck(proto);
    }

    virtual bool expectAck(Protocol& proto) {
        return getContent().expectAck(proto);
    }

    virtual bool isActive() {
        return getContent().isActive();
    }

    virtual String toString() {
        return getContent().toString();
    }

    virtual void close() {
        return getContent().close();
    }

    virtual String getBootstrapCarrierName() {
        return getContent().getBootstrapCarrierName();
    }

    virtual int connect(const yarp::os::Contact& src,
                        const yarp::os::Contact& dest,
                        const yarp::os::ContactStyle& style,
                        int mode,
                        bool reversed) {
        return getContent().connect(src,dest,style,mode,reversed);
    }
};


class StubCarrier : public ForwardingCarrier {
private:
    YarpPluginSettings settings;
    YarpPlugin<Carrier> plugin;
public:
    StubCarrier(const char *dll_name, const char *fn_name) {
        settings.setLibraryMethodName(dll_name,fn_name);
        init();
    }

    StubCarrier(const char *name) {
        settings.setPluginName(name);
        init();
    }

    void init() {
        YarpPluginSelector selector;
        selector.scan();
        settings.setSelector(selector);
        if (plugin.open(settings)) {
            car.open(*plugin.getFactory());
            settings.setLibraryMethodName(plugin.getFactory()->getName(),
                                          settings.getMethodName());
        }
    }

    Carrier& getContent() {
        return car.getContent();
    }

    virtual Carrier *create() {
        ForwardingCarrier *ncar = new ForwardingCarrier(plugin.getFactory(),this);
        if (ncar==NULL) {
            return NULL;
        }
        if (!ncar->isValid()) {
            delete ncar;
            ncar = NULL;
            return NULL;
        }
        return ncar;
    }

    ConstString getDllName() const {
       return settings.getLibraryName();
    }

    ConstString getFnName() const {
        return settings.getMethodName();
    }
};

#endif

bool NetworkBase::registerCarrier(const char *name,const char *dll) {
#ifdef YARP_HAS_ACE
    StubCarrier *factory = NULL;
    if (dll==NULL) {
        factory = new StubCarrier(name);
        if (!factory) return false;
    } else {
        factory = new StubCarrier(dll,name);
    }
    if (factory==NULL) {
        YARP_ERROR(Logger::get(),"Failed to register carrier");
        return false;
    }
    if (!factory->isValid()) {
        YARP_SPRINTF2(Logger::get(),error,"Failed to find library %s with carrier %s", dll, name);
        delete factory;
        factory = NULL;
        return false;
    }
    Carriers::addCarrierPrototype(factory);
    return true;
#else
    YARP_ERROR(Logger::get(),"Cannot creat stub carriers without ACE");
    return false;
#endif
}


bool NetworkBase::localNetworkAllocation() {
    bool globalAlloc = getNameSpace().serverAllocatesPortNumbers();
    return !globalAlloc;
}


Contact NetworkBase::detectNameServer(bool useDetectedServer,
                                      bool& scanNeeded,
                                      bool& serverUsed) {
    return getNameSpace().detectNameServer(useDetectedServer,
                                           scanNeeded,
                                           serverUsed);
}


bool NetworkBase::writeToNameServer(PortWriter& cmd,
                                    PortReader& reply,
                                    const ContactStyle& style) {
    return getNameSpace().writeToNameServer(cmd,reply,style);
}


ConstString NetworkBase::getConfigFile(const char *fname) {
    return NameConfig::expandFilename(fname).c_str();
}


int NetworkBase::getDefaultPortRange() {
    ConstString range = NetworkBase::getEnvironment("YARP_PORT_RANGE");
    if (range!="") {
        int irange = NetType::toInt(range.c_str());
        if (irange != 0) return irange;
    }
    return 10000;
}
