// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <yarp/NameServer.h>
#include <yarp/Logger.h>
#include <yarp/PortCore.h>
#include <yarp/SplitString.h>
#include <yarp/NetType.h>
#include <yarp/ManagedBytes.h>
#include <yarp/NameConfig.h>
#include <yarp/FallbackNameServer.h>
#include <yarp/Companion.h>
#include <yarp/os/Time.h>
#include <yarp/os/Value.h>

#include <ace/Containers_T.h>

using namespace yarp;
using namespace yarp::os;

// produce a correctly parsed string in presence of quoting
static ConstString STR_HELP(const char *txt) {
    Value v;
    v.fromString(txt);
    return v.asString();
}
#define STR(x) STR_HELP(x).c_str()


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//
// Basic functionality
//
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////


Address NameServer::unregisterName(const String& name) {
    Address prev = queryName(name);
    if (prev.isValid()) {
        if (prev.getPort()!=-1) {
            NameRecord& rec = getNameRecord(prev.getRegName());
            if (rec.isReusablePort()) {
                HostRecord& host = getHostRecord(prev.getName());
                host.release(prev.getPort());
            }
            //NameRecord& rec = getNameRecord(name);
            rec.clear();
            tmpNames.release(name);
        }
    }
  
    return queryName(name);
}



Address NameServer::registerName(const String& name, 
                                 const Address& address,
                                 const String& remote) {
    bool reusablePort = false;

    YARP_DEBUG(Logger::get(),"in registerName...");

    if (name!="...") {
        unregisterName(name);
    }

    Address suggestion = address;

    if (!suggestion.isValid()) {
        suggestion = Address("...",0,"...",name);
    }

    String portName = name;
    if (portName == "...") {
        portName = tmpNames.get();
    }

    String carrier = suggestion.getCarrierName();
    if (carrier == "...") {
        carrier = "tcp";
    }

    String machine = suggestion.getName();
    if (machine == "...") {
        if (carrier!="mcast") {
            if (remote=="...") {
                YARP_ERROR(Logger::get(),"remote machine name was not found!  can only guess it is local...");
                machine = "localhost";
            } else {
                machine = remote; 
            }
        } else {
            machine = mcastRecord.get();
        }
    }

    int port = suggestion.getPort();
    if (port == 0) {
        port = getHostRecord(machine).get();
        reusablePort = true;
    }

    suggestion = Address(machine,port,carrier,portName);

    YARP_DEBUG(Logger::get(),String("Thinking about registering ") +
               suggestion.toString());
  
    NameRecord& nameRecord = getNameRecord(suggestion.getRegName());
    nameRecord.setAddress(suggestion,reusablePort);

    return nameRecord.getAddress();
}


Address NameServer::queryName(const String& name) {
    String base = name;
    String pat = "";
    if (name.strstr("/net=") == 0) {
        int patStart = 5;
        int patEnd = name.find('/',patStart);
        if (patEnd>=patStart) {
            pat = name.substr(patStart,patEnd-patStart);
            base = name.substr(patEnd);
            YARP_DEBUG(Logger::get(),String("Special query form ") +
                       name + " (" + pat + "/" + base + ")");
        }
    }

    NameRecord *rec = getNameRecord(base,false);
    if (rec!=NULL) {
        if (pat!="") {
            String ip = rec->matchProp("ips",pat);
            if (ip!="") {
                SplitString sip(ip.c_str());
                return (rec->getAddress()).addName(sip.get(0));
            }
        }
        return rec->getAddress();
    }
    return Address();
}


NameServer::NameRecord *NameServer::getNameRecord(const String& name, 
                                                  bool create) {
    ACE_Hash_Map_Entry<String,NameRecord> *entry = NULL;
    int result = nameMap.find(name,entry);
    if (result==-1) {
        if (!create) {
            return NULL;
        }
        nameMap.bind(name,NameRecord());
        result = nameMap.find(name,entry);
    }
    YARP_ASSERT(result!=-1);
    YARP_ASSERT(entry!=NULL);
    return &(entry->int_id_);
}


NameServer::HostRecord *NameServer::getHostRecord(const String& name,
                                                  bool create) {
    ACE_Hash_Map_Entry<String,HostRecord> *entry = NULL;
    int result = hostMap.find(name,entry);
    if (result==-1) {
        if (!create) {
            return NULL;
        }
        hostMap.bind(name,HostRecord());
        result = hostMap.find(name,entry);
        YARP_ASSERT(entry!=NULL);
        entry->int_id_.setBase(basePort);
    }
    YARP_ASSERT(result!=-1);
    YARP_ASSERT(entry!=NULL);
    return &(entry->int_id_);
}









////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//
// Remote interface
//
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////



void NameServer::setup() {

    basePort = 10002; // this gets replaced

    dispatcher.add("register", &NameServer::cmdRegister);
    dispatcher.add("unregister", &NameServer::cmdUnregister);
    dispatcher.add("query", &NameServer::cmdQuery);
    dispatcher.add("help", &NameServer::cmdHelp);
    dispatcher.add("set", &NameServer::cmdSet);
    dispatcher.add("get", &NameServer::cmdGet);
    dispatcher.add("check", &NameServer::cmdCheck);
    dispatcher.add("match", &NameServer::cmdMatch);
    dispatcher.add("list", &NameServer::cmdList);
    dispatcher.add("route", &NameServer::cmdRoute);
    dispatcher.add("gc", &NameServer::cmdGarbageCollect);
}

String NameServer::cmdRegister(int argc, char *argv[]) {

    String remote = argv[0];
    argc--;
    argv++;

    if (argc<1) {
        return "need at least one argument";
    }
    String portName = STR(argv[0]);

    String machine = "...";
    String carrier = "...";
    int port = 0;
    if (argc>=2) {
        carrier = argv[1];
    }
    if (argc>=3) {
        machine = argv[2];
    }
    if (argc>=4) {
        if (String("...") == argv[3]) {
            port = 0;
        } else {
            port = NetType::toInt(argv[3]);
        }
    }

    Address address = registerName(portName,
                                   Address(machine,port,carrier,portName),
                                   remote);

    YARP_DEBUG(Logger::get(), 
               String("name server register address -- ") +
               address.toString());
  
    return terminate(textify(address));
}


String NameServer::cmdQuery(int argc, char *argv[]) {
    // ignore source
    argc--;
    argv++;

    if (argc<1) {
        return "need at least one argument";
    }
    String portName = STR(argv[0]);
    Address address = queryName(portName);
    return terminate(textify(address));
}

String NameServer::cmdUnregister(int argc, char *argv[]) {
    // ignore source
    argc--;
    argv++;

    if (argc<1) {
        return "need at least one argument";
    }
    String portName = STR(argv[0]);
    Address address = unregisterName(portName);
    return terminate(textify(address));
}


String NameServer::cmdRoute(int argc, char *argv[]) {
    // ignore source
    argc--;
    argv++;

    if (argc<2) {
        return terminate("need at least two arguments: the source port and the target port\n(followed by an optional list of carriers in decreasing order of desirability)");
    }
    String src = STR(argv[0]);
    String dest = STR(argv[1]);

    argc-=2;
    argv+=2;

    char *altArgv[] = { "local", "shmem", "mcast", "udp", "tcp", "text" };
    int altArgc = 6;

    if (argc==0) {
        argc = altArgc;
        argv = altArgv;
    }


    NameRecord& srcRec = getNameRecord(src);
    NameRecord& destRec = getNameRecord(dest);
    String pref = "";

    for (int i=0; i<argc; i++) {
        String carrier = argv[i];
        if (srcRec.checkProp("offers",carrier) &&
            destRec.checkProp("accepts",carrier)) {
            bool ok = true;
            if (carrier=="local"||carrier=="shmem") {
                if (srcRec.getProp("ips") == destRec.getProp("ips")) {
                    if (carrier=="local") {
                        if (srcRec.getProp("process") != destRec.getProp("process")) {
                            ok = false;
                        }
                    }
                } else {
                    ok = false;
                }
            }
            if (ok) {
                pref = carrier;
                break;
            }
        }
    }
    if (pref!="") {
        pref = pref + ":/" + dest;
    } else {
        pref = dest;
    }

    String result = "port " + src + " route " + dest + " = " + pref + "\n";
    return terminate(result);
}


String NameServer::cmdHelp(int argc, char *argv[]) {
    // ignore source
    argc--;
    argv++;

    String result = "Here are some ways to use the name server:\n";
    //ACE_Vector<String> names = dispatcher.getNames();
    //for (unsigned i=0; i<names.size(); i++) {
    //const String& name = names[i];
    //result += String("   ") + name + " ...\n";
    //}
    result += String("+ help\n");
    result += String("+ list\n");
    result += String("+ register $portname\n");
    result += String("+ unregister $portname\n");
    result += String("+ query $portname\n");
    result += String("+ set $portname $property $value\n");
    result += String("+ get $portname $property\n");
    result += String("+ check $portname $property\n");
    result += String("+ match $portname $property $prefix\n");
    result += String("+ route $port1 $port2\n");
    result += String("+ gc\n");
    return terminate(result);
}


String NameServer::cmdSet(int argc, char *argv[]) {
    // ignore source
    argc--;
    argv++;

    if (argc<2) {
        return "need at least two arguments: the port name, and a key";
    }
    String target = STR(argv[0]);
    String key = argv[1];
    NameRecord& nameRecord = getNameRecord(target);
    nameRecord.clearProp(key);
    for (int i=2; i<argc; i++) {
        nameRecord.addProp(key,argv[i]);
    }
    return terminate(String("port ") + target + " property " + key + " = " +
                     nameRecord.getProp(key) + "\n");
}

String NameServer::cmdGet(int argc, char *argv[]) {
    // ignore source
    argc--;
    argv++;

    if (argc<2) {
        return "need exactly two arguments: the port name, and a key";
    }
    String target = STR(argv[0]);
    String key = argv[1];
    NameRecord& nameRecord = getNameRecord(target);
    return terminate(String("port ") + target + " property " + key + " = " +
                     nameRecord.getProp(key) + "\n");
}

String NameServer::cmdMatch(int argc, char *argv[]) {
    // ignore source
    argc--;
    argv++;

    if (argc<3) {
        return "need exactly three arguments: the port name, a key, and a prefix";
    }
    String target = STR(argv[0]);
    String key = argv[1];
    String prefix = argv[2];
    NameRecord& nameRecord = getNameRecord(target);
    return terminate(String("port ") + target + " property " + key + " = " +
                     nameRecord.matchProp(key,prefix) + "\n");
}

String NameServer::cmdCheck(int argc, char *argv[]) {
    // ignore source
    argc--;
    argv++;

    if (argc<2) {
        return "need at least two arguments: the port name, and a key";
    }
    String response = "";
    String target = STR(argv[0]);
    String key = argv[1];
    NameRecord& nameRecord = getNameRecord(target);
    for (int i=2; i<argc; i++) {
        String val = "false";
        if (nameRecord.checkProp(key,argv[i])) {
            val = "true";
        }
        if (i>2) {
            response += "\n";
        }
        response += "port " + target + " property " + 
            key + " value " + argv[i] + " present " + val;
    }
    response += "\n";
    return terminate(response);
}


String NameServer::cmdList(int argc, char *argv[]) {
    String response = "";

    ACE_Ordered_MultiSet<String> lines;
    for (NameMapHash::iterator it = nameMap.begin(); it!=nameMap.end(); it++) {
        NameRecord& rec = (*it).int_id_;
        lines.insert(textify(rec.getAddress()));
    }

    // return result in alphabetical order
    ACE_Ordered_MultiSet_Iterator<String> iter(lines);
    iter.first();
    while (!iter.done()) {
        response += *iter;
        iter.advance();
    }

    return terminate(response);
}


String NameServer::cmdGarbageCollect(int argc, char *argv[]) {
    String response = "";

    for (NameMapHash::iterator it = nameMap.begin(); it!=nameMap.end(); it++) {
        NameRecord& rec = (*it).int_id_;
        Address addr = rec.getAddress();
        String port = addr.getRegName();
        if (port!="fallback"&&port!="/root") {
            if (addr.isValid()) {
                OutputProtocol *out = Carriers::connect(addr);
                if (out==NULL) {
                    response += "Removing " + port + "\n";
                    unregisterName(port);
                } else {
                    delete out;
                }
            }
        }
    }

    return terminate(response);
}


String NameServer::textify(const Address& address) {
    String result = "";
    if (address.isValid()) {
        result = "registration name ";
        result = result + address.getRegName() + 
            " ip " + address.getName() + " port " + 
            NetType::toString(address.getPort()) + " type " + 
            address.getCarrierName() + "\n";
    }
    return result;
}


String NameServer::terminate(const String& str) {
    return str + "*** end of message";
}


String NameServer::apply(const String& txt, const Address& remote) {
    String result = "no command given";
    mutex.wait();
    try {
        SplitString ss(txt.c_str());
        if (ss.size()>=2) {
            String key = ss.get(1);
            YARP_DEBUG(Logger::get(),String("dispatching to ") + key);
            ss.set(1,remote.getName().c_str());
            result = dispatcher.dispatch(this,key.c_str(),ss.size()-1,
                                         (char **)(ss.get()+1));
            YARP_DEBUG(Logger::get(), String("name server request -- ") + txt);
            YARP_DEBUG(Logger::get(), String("name server result  -- ") + result);
        }
    } catch (IOException e) {
        YARP_DEBUG(Logger::get(),String("name server sees exception ") + 
                   e.toString());
        mutex.post();
        throw e;
    }
    mutex.post();
    return result;
}





#ifndef DOXYGEN_SHOULD_SKIP_THIS

class MainNameServer : public NameServer, public Readable {

public:
    MainNameServer(int basePort) {
        this->basePort = basePort;
    }

    virtual bool read(ConnectionReader& reader) {
        String ref = "NAME_SERVER ";
        bool ok = true;
        String msg = "?";
        if (ok) {
            msg = ref + reader.expectText().c_str();
        }
        YARP_DEBUG(Logger::get(),String("name server got message ") + msg);
        int index = msg.find("NAME_SERVER");
        if (index==0) {
            Address remote;
            remote = Address::fromContact(reader.getRemoteContact());
            //if (reader.getStreams()!=NULL) {
            //remote = reader.getStreams()->getRemoteAddress();
            //}
            YARP_DEBUG(Logger::get(),String("name server receiving from ") + 
                       remote.toString());
            YARP_DEBUG(Logger::get(),String("name server request is ") + msg);
            String result = apply(msg,remote);
            //OutputStream *os = reader.getReplyStream();
            ConnectionWriter *os = reader.getWriter();
            if (os!=NULL) {
                if (result=="") {
                    result = terminate(String("unknown command ") + msg + "\n");
                }
                //os->writeLine(result);
                //os->flush();
                //os->close();

                // This change is just to make Microsoft Telnet happy
                String tmp;
                for (unsigned int i=0; i<result.length(); i++) {
                    if (result[i]=='\n') {
                        tmp += '\r';
                    }
                    tmp += result[i];
                }
                tmp += '\r';
                os->appendString(tmp.c_str(),'\n');
                //os->appendString(result.c_str(),'\n');
	
                YARP_DEBUG(Logger::get(),String("name server reply is ") + result);
                String resultSparse = result;
                int end = resultSparse.find("\n*** end of message");
                if (end>=0) {
                    resultSparse[end] = '\0';
                }
                YARP_INFO(Logger::get(),resultSparse);
            }
        } else {
            YARP_INFO(Logger::get(),"Name server ignoring unknown command: "+msg);
            //OutputStream *os = reader.getReplyStream();
            ConnectionWriter *os = reader.getWriter();
            if (os!=NULL) {
                // this result is necessary for YARP1 support
                os->appendString("???????????????????????????????????????",'\n');
                //os->flush();
                //os->close();
            }
        }
        return true;
    }
};


#endif /* DOXYGEN_SHOULD_SKIP_THIS */



int NameServer::main(int argc, char *argv[]) {

    // pick an address
    Address suggest("...",0); // suggestion is initially empty

    try {

        if (argc>=1) {
            if (argc>=2) {
                suggest = Address(argv[0],NetType::toInt(argv[1]));
            } else {
                suggest = Address("...",NetType::toInt(argv[0]));
            }
        }
    
        // see what address is lying around
        Address prev;
        NameConfig conf;
        if (conf.fromFile()) {
            prev = conf.getAddress();
        }
    
        // merge
        if (prev.isValid()) {
            if (suggest.getName()=="...") {
                suggest = Address(prev.getName(),suggest.getPort());
            }
            if (suggest.getPort()==0) {
                suggest = Address(suggest.getName(),prev.getPort());
            }
        }
    
        // still something not set?
        if (suggest.getPort()==0) {
            suggest = Address(suggest.getName(),10000);
        }
        if (suggest.getName()=="...") {
            // should get my IP
            suggest = Address(conf.getHostName(),suggest.getPort());
        }
    
        // finally, should make sure IP is local, and if not, correct it
        if (!conf.isLocalName(suggest.getName())) {
            YARP_INFO(Logger::get(),"Overriding non-local address for name server");
            suggest = Address(conf.getHostName(),suggest.getPort());
        }
    
        // and save
        conf.setAddress(suggest);
        if (!conf.toFile()) {
            YARP_ERROR(Logger::get(), String("Could not save configuration file ") +
                       conf.getConfigFileName());
        }
    
        PortCore server;  // we use a subset of the PortCore functions
        MainNameServer name(suggest.getPort() + 2);

        // register root for documentation purposes
        name.registerName("/root",suggest);
        server.listen(Address(suggest.addRegName("/root")));
        server.setReadHandler(name);
        YARP_INFO(Logger::get(), String("Name server listening at ") + 
                  suggest.toString());
        ACE_OS::printf("Name server can be browsed at http://%s:%d/\n",
                       suggest.getName().c_str(), suggest.getPort());

        server.start();
    
        FallbackNameServer fallback(name);
        fallback.start();

        // register fallback root for documentation purposes
        name.registerName("fallback",FallbackNameServer::getAddress());
        YARP_INFO(Logger::get(), String("Bootstrap server listening at ") + 
                  FallbackNameServer::getAddress().toString());
    
        while (true) {
            YARP_DEBUG(Logger::get(),"name server running happily");
            Time::delay(60);
        }
        server.close();
        fallback.close();
        fallback.join();
    
    } catch (IOException e) {
        YARP_DEBUG(Logger::get(),e.toString() + " <<< name server exception");
        YARP_ERROR(Logger::get(), "name server failed to start");
        YARP_ERROR(Logger::get(), String("   reason for failure is \"") + 
                   e.toString() + "\"");
        YARP_ERROR(Logger::get(), "   the name server may already be running?");
        if (suggest.getPort()>0) {
            YARP_ERROR(Logger::get(), String("   or perhaps another service may already be running on port ") + NetType::toString(suggest.getPort()) + "?");
        }
        return 1;
    }

    return 0;
}


