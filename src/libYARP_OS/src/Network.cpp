// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/Network.h>
#include <yarp/os/Time.h>
#include <yarp/os/Thread.h>

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
#include <yarp/os/impl/NameConfig.h>
#include <yarp/os/impl/ThreadImpl.h>
#include <yarp/os/impl/PlatformStdio.h>
#include <yarp/os/impl/PlatformSignal.h>

using namespace yarp::os::impl;
using namespace yarp::os;

static int __yarp_is_initialized = 0;

static bool needsLookup(const Contact& contact) {
    if (contact.getHost()!="") return false;
    if (contact.getCarrier()=="topic") return false;
    return true;
}

static int noteDud(const Contact& src) {
    //printf("DUD %s\n", src.toString().c_str());
    NameClient& nic = NameClient::getNameClient();
    NameStore *store = nic.getQueryBypass();
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
    /*
    printf("CON %s %s %d\n", src.toString().c_str(),
           dest.toString().c_str(),
           mode);
    */
    ContactStyle rpc;
    rpc.admin = true;
    rpc.quiet = style.quiet;

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
    YARP_SPRINTF2(Logger::get(),debug,"asking %s: %s",
                  src.toString().c_str(), cmd.toString().c_str());
    bool ok = NetworkBase::write(src,cmd,reply,rpc);
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

static int metaConnect(const char *csrc,
                       const char *cdest,
                       ContactStyle style,
                       int mode) {
    ConstString src = csrc;
    ConstString dest = cdest;

    // get the expressed contacts, without name server input
    Contact dynamicSrc = Contact::fromString(src);
    Contact dynamicDest = Contact::fromString(dest);

    bool topical = false;
    if (dynamicSrc.getCarrier()=="topic" || 
        dynamicDest.getCarrier()=="topic") {
        topical = true;
    }
    
    // fetch completed contacts from name server, if needed
    Contact staticSrc;
    Contact staticDest;
    if (needsLookup(dynamicSrc)&&!topical) {
        staticSrc = NetworkBase::queryName(dynamicSrc.getName());
        if (!staticSrc.isValid()) {
            if (!style.quiet) {
                fprintf(stderr, "Failure: could not find source port %s\n",
                        src.c_str());
            }
            return 1;
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
    if (needsLookup(dynamicDest)&&!topical) {
        staticDest = NetworkBase::queryName(dynamicDest.getName());
        if (!staticDest.isValid()) {
            if (!style.quiet) {
                fprintf(stderr, "Failure: could not find destination port %s\n",
                        dest.c_str());
            }
            return 1;
        }
    } else {
        staticDest = dynamicDest;
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
        if (mode==YARP_ENACT_CONNECT) {
            cmd.add("subscribe");
        } else if (mode==YARP_ENACT_DISCONNECT) {
            cmd.add("unsubscribe");
        } else {
            fprintf(stderr,"Failure: cannot check subscriptions yet\n");
            return 1;
        }
        if (style.carrier!="") {
            if (srcIsTopic) {
                dynamicDest = dynamicDest.addCarrier(style.carrier);
            } else {
                dynamicSrc = dynamicSrc.addCarrier(style.carrier);
            }
        }
        cmd.add(dynamicSrc.toString().c_str());
        cmd.add(dynamicDest.toString().c_str());
        bool ok = NetworkBase::write(NetworkBase::getNameServerContact(),
                                     cmd,
                                     reply);
        bool fail = (reply.get(0).toString()=="fail")||!ok;
        if (fail) {
            if (!style.quiet) {
                fprintf(stderr,"Failure: name server did not accept connection to topic.\n");
            }
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

    /*
    printf("Status competence %d:%d push %d pull %d\n",
           srcIsCompetent,
           destIsCompetent,
           connectionIsPush,
           connectionIsPull);
    */

    /*
      // desperation
    if (!(srcIsCompetent||destIsCompetent)) {
        srcIsCompetent = true;
        destIsCompetent = true;
    }
    */

    int result = -1;

    if (srcIsCompetent&&connectionIsPush) {
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

    fprintf(stderr,"Failure: no method known for this connection type: [%s]->[%s]\n", csrc, cdest);

    return 1;
}


bool NetworkBase::connect(const char *src, const char *dest, 
                      const char *carrier, bool quiet) {
    ContactStyle style;
    style.quiet = quiet;
    if (carrier!=NULL) {
        style.carrier = carrier;
    }
    int result = metaConnect(src,dest,style,YARP_ENACT_CONNECT);
    /*
    if (carrier!=NULL) {
        // prepend carrier
        String fullDest = String(carrier) + ":/" + Companion::slashify(dest);
        result = Companion::connect(src,fullDest.c_str(),quiet);
    } else {
        result = Companion::connect(src,dest,quiet);
    }
    */
    return result == 0;

}


bool NetworkBase::disconnect(const char *src, const char *dest, bool quiet) {
    //int result = Companion::disconnect(src,dest,quiet);
    //return result == 0;
    ContactStyle style;
    style.quiet = quiet;
    int result = metaConnect(src,dest,style,YARP_ENACT_DISCONNECT);
    return result == 0;
}


bool NetworkBase::exists(const char *port, bool quiet) {
    int result = Companion::exists(port,quiet);
    if (result==0) {
        Companion::poll(port,true);
    }
    return result == 0;
}


bool NetworkBase::sync(const char *port, bool quiet) {
    //if (!result) {
    int result = Companion::wait(port,quiet);
    //}
    if (result==0) {
        Companion::poll(port,true);
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
       String stack = NameConfig::getEnv("YARP_STACK_SIZE");
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
#ifdef YARP_HAS_ACE       
        ACE::fini();
#endif
    }
    if (__yarp_is_initialized>0) __yarp_is_initialized--;
}

Contact NetworkBase::queryName(const char *name) {
    NameClient& nic = NameClient::getNameClient();
    Address address = nic.queryName(name);
    //printf("address is %s %d\n",address.toString().c_str(), address.isValid());
    return address.toContact();
}


Contact NetworkBase::registerName(const char *name) {
    NameClient& nic = NameClient::getNameClient();
    Address address = nic.registerName(name);
    return address.toContact();
}


Contact NetworkBase::registerContact(const Contact& contact) {
    NameClient& nic = NameClient::getNameClient();
    Address address = nic.registerName(contact.getName().c_str(),
                                       Address::fromContact(contact));
    return address.toContact();
}

Contact NetworkBase::unregisterName(const char *name) {
    NameClient& nic = NameClient::getNameClient();
    Address address = nic.unregisterName(name);
    return address.toContact();
}


Contact NetworkBase::unregisterContact(const Contact& contact) {
    NameClient& nic = NameClient::getNameClient();
    Address address = nic.unregisterName(contact.getName().c_str());
    return address.toContact();
}


bool NetworkBase::setProperty(const char *name,
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


Value *NetworkBase::getProperty(const char *name, const char *key) {
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


bool NetworkBase::setLocalMode(bool flag) {
    NameClient& nic = NameClient::getNameClient();
    bool state = nic.isFakeMode();
    nic.setFakeMode(flag);
    return state;
}

bool NetworkBase::getLocalMode() {
    NameClient& nic = NameClient::getNameClient();
    bool state = nic.isFakeMode();
    return state;
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
                        const ContactStyle& style) {

    // This is a little complicated because we make the connection
    // without using a port ourselves.  With a port it is easy.

    const char *connectionName = "admin";
    ConstString name = contact.getName();
    const char *targetName = name.c_str();  // use carefully!
    Address address = Address::fromContact(contact);
    if (!address.isValid()) {
        NameClient& nic = NameClient::getNameClient();
        address = nic.queryName(targetName);
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
    BufferedConnectionWriter bw(out->isTextMode());
    bool ok = pc.write(bw);
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


bool NetworkBase::isConnected(const char *src, const char *dest, bool quiet) {
    ContactStyle style;
    style.quiet = quiet;
    int result = metaConnect(src,dest,style,YARP_ENACT_EXISTS);
    if (result!=0) {
        if (!quiet) {
            printf("No connection from %s to %s found\n",
                   src, dest);
        }
    }
    return result == 0;

    /*
    Contact contact = Contact::fromString(src);
    Bottle cmd, reply;
    cmd.addVocab(Vocab::encode("list"));
    cmd.addVocab(Vocab::encode("out"));
    NetworkBase::write(contact,cmd,reply,true);
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
    */
}


ConstString NetworkBase::getNameServerName() {
    NameConfig nc;
    String name = nc.getNamespace();
    return name.c_str();
}


Contact NetworkBase::getNameServerContact() {
    NameClient& nic = NameClient::getNameClient();
    return nic.getAddress().toContact();
}



bool NetworkBase::setNameServerName(const char *name) {
    NameConfig nc;
    String fname = nc.getConfigFileName(YARP_CONFIG_NAMESPACE_FILENAME);
    nc.writeConfig(fname,String(name));
    nc.getNamespace(true);
    NameClient& client = NameClient::getNameClient();
    return client.updateAddress();
}


bool NetworkBase::checkNetwork() {
    Contact c = queryName(getNameServerName());
    //printf("Contact is %s %d\n", c.toString().c_str(), c.isValid());
    return c.isValid();
}


bool NetworkBase::initialized() {
    return __yarp_is_initialized>0;
}


void NetworkBase::setVerbosity(int verbosity) {
    Logger::get().setVerbosity(verbosity);
}

void NetworkBase::queryBypass(NameStore *store) {
    NameClient& client = NameClient::getNameClient();
    client.queryBypass(store);
}


ConstString NetworkBase::getEnvironment(const char *key,
                                        bool *found) {
    return NameConfig::getEnv(key,found).c_str();
}

void NetworkBase::lock() {
    ThreadImpl::threadMutex.wait();
}

void NetworkBase::unlock() {
    ThreadImpl::threadMutex.post();
}


#ifdef YARP_HAS_ACE

#include <yarp/os/SharedLibraryClass.h>

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
        printf("CREATE 2\n");
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

    virtual bool sendIndex(Protocol& proto) {
        return getContent().sendIndex(proto);
    }

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
public:
    StubCarrier(const char *dll_name, const char *fn_name) {
        factory = new SharedLibraryClassFactory<Carrier>();
        if (factory==NULL) return;
        factory->addRef();
        if (!factory->open(dll_name, fn_name)) {

            int problem = factory->getStatus();
            switch (problem) {
            case SharedLibraryFactory::STATUS_LIBRARY_NOT_LOADED:
                fprintf(stderr,"cannot load shared library\n");
                break;
            case SharedLibraryFactory::STATUS_FACTORY_NOT_FOUND:
                fprintf(stderr,"cannot find YARP hook in shared library\n");
                break;
            case SharedLibraryFactory::STATUS_FACTORY_NOT_FUNCTIONAL:
                fprintf(stderr,"YARP hook in shared library misbehaved\n");
                break;
            default:
                fprintf(stderr,"Unknown error\n");
                break;
            }
            return;
        }
        if (!car.open(*factory)) {
            fprintf(stderr,"Failed to create %s from shared library %s\n",
                    fn_name, dll_name);
        }
    }

    Carrier& getContent() {
        return car.getContent();
    }

    virtual Carrier *create() {
        ForwardingCarrier *ncar = new ForwardingCarrier(factory,this);
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
};

#endif

bool NetworkBase::registerCarrier(const char *name,const char *dll) {
#ifdef YARP_HAS_ACE
    //printf("Registering carrier %s from %s\n", name, dll);
    StubCarrier *factory = new StubCarrier(dll,name);
    if (factory==NULL) {
        YARP_ERROR(Logger::get(),"Failed to create carrier");
        return false;
    }
    if (!factory->isValid()) {
        YARP_ERROR(Logger::get(),"Failed to create valid carrier");
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


