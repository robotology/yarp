/*
 * Copyright (C) 2006, 2008 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/impl/NameServer.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/PortCore.h>
#include <yarp/os/impl/SplitString.h>
#include <yarp/os/NetType.h>
#include <yarp/os/impl/NameConfig.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/os/impl/NameConfig.h>
#ifdef YARP_HAS_ACE
#  include <yarp/os/impl/FallbackNameServer.h>
#endif
#include <yarp/os/impl/Companion.h>
#include <yarp/os/Time.h>
#include <yarp/os/Value.h>
#include <yarp/os/Port.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Network.h>
#include <yarp/os/Property.h>

#include <map>
#include <set>

using namespace yarp::os::impl;
using namespace yarp::os;

//#define YMSG(x) printf x;
//#define YTRACE(x) YMSG(("at %s\n", x))

#define YMSG(x)
#define YTRACE(x)

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


Contact NameServer::unregisterName(const ConstString& name) {
    Contact prev = queryName(name);
    if (prev.isValid()) {
        if (prev.getPort()!=-1) {
            NameRecord& rec = getNameRecord(prev.getRegName());
            if (rec.isReusablePort()) {
                HostRecord& host = getHostRecord(prev.getHost());
                host.release(prev.getPort());
            }
            if (rec.isReusableIp()) {
                if (rec.getAddress().getCarrier()=="mcast") {
                    mcastRecord.releaseAddress(rec.getAddress().getHost().c_str());
                }
            }
            rec.clear();
            tmpNames.release(name);

            Bottle event;
            event.addVocab(Vocab::encode("del"));
            event.addString(name.c_str());
            onEvent(event);
        }
    }

    return queryName(name);
}



Contact NameServer::registerName(const ConstString& name,
                                 const Contact& address,
                                 const ConstString& remote) {
    bool reusablePort = false;
    bool reusableIp = false;

    //YARP_DEBUG(Logger::get(), "in registerName...");

    if (name!="...") {
        unregisterName(name);
    }

    Contact suggestion = address;

    if (!suggestion.isValid()) {
        suggestion = Contact(name, "...", "...", 0);
    }

    ConstString portName = name;
    if (portName == "...") {
        portName = tmpNames.get();
    }

    ConstString carrier = suggestion.getCarrier();
    if (carrier == "...") {
        carrier = "tcp";
    }

    ConstString machine = suggestion.getHost();
    int overridePort = 0;
    if (machine == "...") {
        if (carrier!="mcast") {
            if (remote=="...") {
                YARP_ERROR(Logger::get(), "remote machine name was not found!  can only guess it is local...");
                machine = "127.0.0.1";
            } else {
                machine = remote;
            }
        } else {
            machine = mcastRecord.get();
            overridePort = mcastRecord.lastPortNumber();
            reusableIp = true;
        }
    }

    int port = suggestion.getPort();
    if (port == 0) {
        if (overridePort) {
            port = overridePort;
        } else {
            port = getHostRecord(machine).get();
            reusablePort = true;
        }
    }

    suggestion = Contact(portName, carrier, machine, port);

    YARP_DEBUG(Logger::get(), ConstString("Registering ") +
               suggestion.toURI() + " for " + suggestion.getRegName());

    NameRecord& nameRecord = getNameRecord(suggestion.getRegName());
    nameRecord.setAddress(suggestion, reusablePort, reusableIp);

    Bottle event;
    event.addVocab(Vocab::encode("add"));
    event.addString(suggestion.getRegName().c_str());
    onEvent(event);

    return nameRecord.getAddress();
}


Contact NameServer::queryName(const ConstString& name) {
    ConstString base = name;
    ConstString pat = "";
    if (name.find("/net=") == 0) {
        size_t patStart = 5;
        size_t patEnd = name.find('/', patStart);
        if (patEnd>=patStart && patEnd!=ConstString::npos) {
            pat = name.substr(patStart, patEnd-patStart);
            base = name.substr(patEnd);
            YARP_DEBUG(Logger::get(), ConstString("Special query form ") +
                       name + " (" + pat + "/" + base + ")");
        }
    }

    NameRecord *rec = getNameRecord(base, false);
    if (rec!=nullptr) {
        if (pat!="") {
            ConstString ip = rec->matchProp("ips", pat);
            if (ip!="") {
                SplitString sip(ip.c_str());
                Contact c = rec->getAddress();
                c.setHost(sip.get(0));
                return c;
            }
        }
        return rec->getAddress();
    }
    return Contact();
}


NameServer::NameRecord *NameServer::getNameRecord(const ConstString& name,
                                                  bool create) {
    std::map<ConstString, NameRecord>::iterator entry = nameMap.find(name);
    if (entry == nameMap.end()) {
        if (!create) {
            return nullptr;
        }
        nameMap[name] = NameRecord();
        entry = nameMap.find(name);
    }
    yAssert(entry != nameMap.end());
    return &(entry->second);
}


NameServer::HostRecord *NameServer::getHostRecord(const ConstString& name,
                                                  bool create) {
    std::map<ConstString, HostRecord>::iterator entry = hostMap.find(name);
    if (entry == hostMap.end()) {
        if (!create) {
            return nullptr;
        }
        hostMap[name] = HostRecord();
        entry = hostMap.find(name);
        entry->second.setBase(basePort);
    }
    yAssert(entry != hostMap.end());
    return &(entry->second);
}









////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
//
// Remote interface
//
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////



void NameServer::setup() {

    basePort = NetworkBase::getDefaultPortRange()+2;

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
    dispatcher.add("bot", &NameServer::cmdBot);
    dispatcher.add("announce", &NameServer::cmdAnnounce);

    ndispatcher.add("list", &NameServer::ncmdList);
    ndispatcher.add("query", &NameServer::ncmdQuery);
    ndispatcher.add("version", &NameServer::ncmdVersion);
    ndispatcher.add("set", &NameServer::ncmdSet);
    ndispatcher.add("get", &NameServer::ncmdGet);
}

ConstString NameServer::cmdRegister(int argc, char *argv[]) {

    ConstString remote = argv[0];
    argc--;
    argv++;

    if (argc<1) {
        return "need at least one argument";
    }
    ConstString portName = STR(argv[0]);

    ConstString machine = "...";
    ConstString carrier = "...";
    int port = 0;
    if (argc>=2) {
        carrier = argv[1];
    }
    if (argc>=3) {
        machine = argv[2];
    }
    if (argc>=4) {
        if (ConstString("...") == argv[3]) {
            port = 0;
        } else {
            port = NetType::toInt(argv[3]);
        }
    }

    Contact address = registerName(portName, Contact(portName, carrier, machine, port), remote);

    //YARP_DEBUG(Logger::get(),
    //ConstString("name server register address -- ") +
    //address.toString());

    return terminate(textify(address));
}


ConstString NameServer::cmdQuery(int argc, char *argv[]) {
    // ignore source
    argc--;
    argv++;

    if (argc<1) {
        return "need at least one argument";
    }
    ConstString portName = STR(argv[0]);
    Contact address = queryName(portName);
    return terminate(textify(address));
}

ConstString NameServer::cmdUnregister(int argc, char *argv[]) {
    // ignore source
    argc--;
    argv++;

    if (argc<1) {
        return "need at least one argument";
    }
    ConstString portName = STR(argv[0]);
    Contact address = unregisterName(portName);
    return terminate(textify(address));
}


ConstString NameServer::cmdAnnounce(int argc, char *argv[]) {
    // ignore source
    argc--;
    argv++;

    return terminate("ok\n");
}

ConstString NameServer::cmdRoute(int argc, char *argv[]) {
    // ignore source
    argc--;
    argv++;

    if (argc<2) {
        return terminate("need at least two arguments: the source port and the target port\n(followed by an optional list of carriers in decreasing order of desirability)");
    }
    ConstString src = STR(argv[0]);
    ConstString dest = STR(argv[1]);

    argc-=2;
    argv+=2;

    const char *altArgv[] = {
        "local", "shmem", "mcast", "udp", "tcp", "text"
    };
    int altArgc = 6;

    if (argc==0) {
        argc = altArgc;
        argv = (char**)altArgv;
    }


    NameRecord& srcRec = getNameRecord(src);
    NameRecord& destRec = getNameRecord(dest);
    ConstString pref = "";

    for (int i=0; i<argc; i++) {
        ConstString carrier = argv[i];
        if (srcRec.checkProp("offers", carrier) &&
            destRec.checkProp("accepts", carrier)) {
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

    ConstString result = "port ";
    result += src + " route " + dest + " = " + pref + "\n";
    return terminate(result);
}


ConstString NameServer::cmdHelp(int argc, char *argv[]) {
    // ignore source
    argc--;
    argv++;

    ConstString result = "Here are some ways to use the name server:\n";
    //ACE_Vector<ConstString> names = dispatcher.getNames();
    //for (unsigned i=0; i<names.size(); i++) {
    //const ConstString& name = names[i];
    //result += ConstString("   ") + name + " ...\n";
    //}
    result += ConstString("+ help\n");
    result += ConstString("+ list\n");
    result += ConstString("+ register $portname\n");
    result += ConstString("+ register $portname $carrier $ipAddress $portNumber\n");
    result += ConstString("  (if you want a field set automatically, write '...')\n");
    result += ConstString("+ unregister $portname\n");
    result += ConstString("+ query $portname\n");
    result += ConstString("+ set $portname $property $value\n");
    result += ConstString("+ get $portname $property\n");
    result += ConstString("+ check $portname $property\n");
    result += ConstString("+ match $portname $property $prefix\n");
    result += ConstString("+ route $port1 $port2\n");
    result += ConstString("+ gc\n");
    return terminate(result);
}


ConstString NameServer::cmdSet(int argc, char *argv[]) {
    // ignore source
    argc--;
    argv++;

    if (argc<2) {
        return "need at least two arguments: the port name, and a key";
    }
    ConstString target = STR(argv[0]);
    ConstString key = argv[1];
    NameRecord& nameRecord = getNameRecord(target);
    nameRecord.clearProp(key);
    for (int i=2; i<argc; i++) {
        nameRecord.addProp(key, argv[i]);
    }
    return terminate(ConstString("port ") + target + " property " + key + " = " +
                     nameRecord.getProp(key) + "\n");
}

ConstString NameServer::cmdGet(int argc, char *argv[]) {
    // ignore source
    argc--;
    argv++;

    if (argc<2) {
        return "need exactly two arguments: the port name, and a key";
    }
    ConstString target = STR(argv[0]);
    ConstString key = argv[1];
    NameRecord& nameRecord = getNameRecord(target);
    return terminate(ConstString("port ") + target + " property " + key + " = " +
                     nameRecord.getProp(key) + "\n");
}

ConstString NameServer::cmdMatch(int argc, char *argv[]) {
    // ignore source
    argc--;
    argv++;

    if (argc<3) {
        return "need exactly three arguments: the port name, a key, and a prefix";
    }
    ConstString target = STR(argv[0]);
    ConstString key = argv[1];
    ConstString prefix = argv[2];
    NameRecord& nameRecord = getNameRecord(target);
    return terminate(ConstString("port ") + target + " property " + key + " = " +
                     nameRecord.matchProp(key, prefix) + "\n");
}

ConstString NameServer::cmdCheck(int argc, char *argv[]) {
    // ignore source
    argc--;
    argv++;

    if (argc<2) {
        return "need at least two arguments: the port name, and a key";
    }
    ConstString response = "";
    ConstString target = STR(argv[0]);
    ConstString key = argv[1];
    NameRecord& nameRecord = getNameRecord(target);
    for (int i=2; i<argc; i++) {
        ConstString val = "false";
        if (nameRecord.checkProp(key, argv[i])) {
            val = "true";
        }
        if (i>2) {
            response += "\n";
        }
        response += "port ";
        response += target + " property " +
            key + " value " + argv[i] + " present " + val;
    }
    response += "\n";
    return terminate(response);
}


ConstString NameServer::cmdList(int argc, char *argv[]) {
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);
    ConstString response = "";

    std::multiset<ConstString> lines;
    for (auto it = nameMap.begin(); it!=nameMap.end(); ++it) {
        NameRecord& rec = it->second;
        lines.insert(textify(rec.getAddress()));
    }

    // return result in alphabetical order
    for (const auto& line : lines) {
        response += line;
    }

    return terminate(response);
}


ConstString NameServer::cmdBot(int argc, char *argv[]) {
    ConstString txt = "";
    argc--;
    argv++;
    if (argc>=1) {
        ConstString key = argv[0];
        argc--;
        argv++;
        Bottle result = ndispatcher.dispatch(this, key.c_str(), argc, argv);
        txt = result.toString().c_str();
    }
    return txt;
}


Bottle NameServer::ncmdList(int argc, char *argv[]) {
    Bottle response;

    ConstString prefix = "";

    if (argc==1) {
        prefix = STR(argv[0]);
    }

    response.addString("ports");
    for (auto it = nameMap.begin(); it!=nameMap.end(); ++it) {
        NameRecord& rec = it->second;
        ConstString iname = rec.getAddress().getRegName();
        if (iname.find(prefix)==0) {
            if (iname==prefix || iname[prefix.length()]=='/' ||
                prefix[prefix.length()-1]=='/') {
                if (rec.getAddress().isValid()) {
                    response.addList() = botify(rec.getAddress());
                }
            }
        }
    }

    return response;
}


yarp::os::Bottle NameServer::ncmdQuery(int argc, char *argv[]) {
    Bottle response;
    if (argc==1) {
        ConstString portName = STR(argv[0]);
        Contact address = queryName(portName);
        response = botify(address);
    }
    return response;
}


yarp::os::Bottle NameServer::ncmdVersion(int argc, char *argv[]) {
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);
    Bottle response;
    response.addString("version");
    response.addString(Companion::version().c_str());
    return response;
}


yarp::os::Bottle NameServer::ncmdSet(int argc, char *argv[]) {

    Bottle response;
    if (argc >= 2) {
        ConstString target = STR(argv[0]);
        ConstString key = STR(argv[1]);
        NameRecord& nameRecord = getNameRecord(target);
        nameRecord.clearProp(key);
        for (int i=2; i<argc; i++) {
            nameRecord.addProp(key, argv[i]);
        }
        response.addString("ok");
    }
    return response;
}

yarp::os::Bottle NameServer::ncmdGet(int argc, char *argv[]) {
    Bottle response;
    if (argc==2) {
        ConstString target = STR(argv[0]);
        ConstString key = argv[1];
        NameRecord& nameRecord = getNameRecord(target);
        return Bottle(nameRecord.getProp(key).c_str());
    }
    return response;
}



ConstString NameServer::cmdGarbageCollect(int argc, char *argv[]) {
    YARP_UNUSED(argc);
    YARP_UNUSED(argv);
    ConstString response = "";

    response = "No cleaning done.\n";

    return terminate(response);
}


ConstString NameServer::textify(const Contact& address) {
    ConstString result = "";
    if (address.isValid()) {
        if (address.getPort()>=0) {
            result = "registration name ";
            result = result + address.getRegName() +
                " ip " + address.getHost() + " port " +
                NetType::toString(address.getPort()) + " type " +
                address.getCarrier() + "\n";
        } else {
            result = "registration name ";
            result = result + address.getRegName() +
                " ip " + "none" + " port " +
                "none" + " type " +
                address.getCarrier() + "\n";
        }
    }
    return result;
}


Bottle NameServer::botify(const Contact& address) {
    Bottle result;
    if (address.isValid()) {
        Bottle bname;
        bname.addString("name");
        bname.addString(address.getRegName().c_str());
        Bottle bip;
        bip.addString("ip");
        bip.addString(address.getHost().c_str());
        Bottle bnum;
        bnum.addString("port_number");
        bnum.addInt(address.getPort());
        Bottle bcarrier;
        bcarrier.addString("carrier");
        bcarrier.addString(address.getCarrier().c_str());

        result.addString("port");
        result.addList() = bname;
        result.addList() = bip;
        result.addList() = bnum;
        result.addList() = bcarrier;
    } else {
        Bottle bstate;
        bstate.addString("error");
        bstate.addInt(-2);
        bstate.addString("port not known");
        result.addString("port");
        result.addList() = bstate;
    }
    return result;
}


static ConstString ns_terminate(const ConstString& str) {
    return str + "*** end of message";
}

ConstString NameServer::terminate(const ConstString& str) {
    return ns_terminate(str);
}


ConstString NameServer::apply(const ConstString& txt, const Contact& remote) {
    ConstString result = "no command given";
    mutex.wait();

    SplitString ss(txt.c_str());
    if (ss.size()>=2) {
        ConstString key = ss.get(1);
        //YARP_DEBUG(Logger::get(), ConstString("dispatching to ") + key);
        ss.set(1, remote.getHost().c_str());
        result = dispatcher.dispatch(this, key.c_str(), ss.size()-1,
                                     (char **)(ss.get()+1));
        if (result == "") {
            Bottle b = ndispatcher.dispatch(this, key.c_str(), ss.size()-1,
                                            (char **)(ss.get()+1));
            result = b.toString().c_str();
            if (result!="") {
                result = result + "\n";
                result = terminate(result);
            }
        }
        //YARP_DEBUG(Logger::get(), ConstString("name server request -- ") + txt);
        //YARP_DEBUG(Logger::get(), ConstString("name server result  -- ") + result);
    }
    mutex.post();
    return result;
}


bool NameServer::apply(const Bottle& cmd, Bottle& result,
                       const Contact& remote) {
    Bottle rcmd;
    rcmd.addString("ignored_legacy");
    rcmd.append(cmd);
    ConstString in = rcmd.toString().c_str();
    ConstString out = apply(in, remote).c_str();
    result.fromString(out.c_str());
    return true;
}



#ifndef DOXYGEN_SHOULD_SKIP_THIS

class MainNameServerWorker : public PortReader {
private:
    NameServer *server;
public:
    MainNameServerWorker(NameServer *server) {
        this->server = server;
    }

    virtual bool read(ConnectionReader& reader) override {
        YTRACE("NameServer::read start");
        ConstString ref = "NAME_SERVER ";
        bool ok = true;
        ConstString msg = "?";
        bool haveMessage = false;
        if (ok) {
            if (reader.isTextMode()) {
                msg = reader.expectText().c_str();
            } else {
                // migrate to binary mode support, eventually optimize
                Bottle b;
                b.read(reader);
                msg = b.toString().c_str();
            }
            haveMessage = (msg!="");
            msg = ref + msg;
        }
        if (reader.isActive()&&haveMessage) {
            YARP_DEBUG(Logger::get(), ConstString("name server got message ") + msg);
            size_t index = msg.find("NAME_SERVER");
            if (index==0) {
                Contact remote = reader.getRemoteContact();
                YARP_DEBUG(Logger::get(),
                           ConstString("name server receiving from ") +
                           remote.toURI());
                YARP_DEBUG(Logger::get(),
                           ConstString("name server request is ") + msg);
                ConstString result = server->apply(msg, remote);
                ConnectionWriter *os = reader.getWriter();
                if (os!=nullptr) {
                    if (result=="") {
                        result = ns_terminate(ConstString("unknown command ") +
                                              msg + "\n");
                    }
                    // This change is just to make Microsoft Telnet happy
                    ConstString tmp;
                    for (unsigned int i=0; i<result.length(); i++) {
                        if (result[i]=='\n') {
                            tmp += '\r';
                        }
                        tmp += result[i];
                    }
                    tmp += '\r';
                    os->appendString(tmp.c_str(), '\n');

                    YARP_DEBUG(Logger::get(),
                               ConstString("name server reply is ") + result);
                    ConstString resultSparse = result;
                    size_t end = resultSparse.find("\n*** end of message");
                    if (end!=ConstString::npos) {
                        resultSparse[end] = '\0';
                    }
                    YARP_INFO(Logger::get(), resultSparse);
                }
            } else {
                YARP_INFO(Logger::get(),
                          ConstString("Name server ignoring unknown command: ")+msg);
                ConnectionWriter *os = reader.getWriter();
            }
        }
        YTRACE("NameServer::read stop");
        return true;
    }
};


class MainNameServer : public NameServer, public PortReaderCreator {
private:
    Port *port;
public:
    MainNameServer(int basePort, Port *port = nullptr) : port(port) {
        setBasePort(basePort);
    }

    void setPort(Port& port) {
        this->port = &port;
    }

    virtual void onEvent(Bottle& event) override {
        if (port!=nullptr) {
            port->write(event);
        }
    }

    virtual PortReader *create() override {
        return new MainNameServerWorker(this);
    }
};


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
