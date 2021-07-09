/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <cstdlib>

#include <yarp/os/Vocab.h>
#include <yarp/os/NestedContact.h>
#include <yarp/os/Network.h>
#include <yarp/os/Time.h>
#include <yarp/os/SystemClock.h>
#include <yarp/serversql/impl/LogComponent.h>
#include <yarp/serversql/impl/NameServiceOnTriples.h>
#include <yarp/serversql/impl/ParseName.h>

using namespace yarp::os;
using namespace yarp::serversql::impl;
using namespace std;

namespace {
YARP_SERVERSQL_LOG_COMPONENT(NAMESERVICEONTRIPLES, "yarp.serversql.impl.NameServiceOnTriples")
} // namespace


Contact NameServiceOnTriples::query(const std::string& portName,
                                    NameTripleState& act,
                                    const std::string& prefix,
                                    bool nested)
{
    if (!nested) {
        lock();
    }
    Triple t;
    t.setNameValue("port",portName.c_str());
    int result = act.mem.find(t, nullptr);
    TripleContext context;
    context.setRid(result);
    if (result!=-1) {
        std::string host;
        if (!std::string(prefix).empty()) {
            printf("LOOKING AT IPS FOR %s\n", prefix.c_str());
            t.setNameValue("ips","*");
            list<Triple> lst = act.mem.query(t,&context);
            for (auto& it : lst) {
                printf("LOOKING AT IPS %s\n", it.value.c_str());
                if (it.value.find(prefix)==0) {
                    host = it.value;
                    break;
                }
            }
        }
        if (host.empty()) {
            t.setNameValue("host","*");
            list<Triple> lst = act.mem.query(t,&context);
            if (!lst.empty()) {
                host = lst.begin()->value;
            }
        }
        if (host.empty()) {
            host = "localhost";
        }
        t.setNameValue("socket","*");
        list<Triple> lst = act.mem.query(t,&context);
        int sock = 10000;
        if (!lst.empty()) {
            sock = atoi(lst.begin()->value.c_str());
        }
        t.setNameValue("carrier","*");
        std::string carrier = "tcp";
        lst = act.mem.query(t,&context);
        if (!lst.empty()) {
            carrier = lst.begin()->value;
        }
        t.setNameValue("type","*");
        std::string typ = "*";
        lst = act.mem.query(t,&context);
        if (!lst.empty()) {
            typ = lst.begin()->value;
        }
        if (!nested) {
            unlock();
        }
        Contact result = Contact(portName, carrier, host, sock);
        if (!typ.empty() && typ!="*") {
            NestedContact nc;
            nc.fromString(result.getName());
            nc.setTypeName(typ);
            result.setNestedContact(nc);
        }
        return result;
    }
    if (!nested) {
        unlock();
    }
    if (delegate && !nested) {
        return delegate->queryName(portName);
    }
    return Contact();
}


yarp::os::Contact NameServiceOnTriples::query(const std::string& port)
{
    Contact check = Contact::fromString(port);
    if (!check.getHost().empty()) {
        return check;
    }
    Bottle cmd;
    Bottle reply;
    Bottle event;
    Contact remote;
    TripleSource& mem = *db;
    NameTripleState act(cmd,reply,event,remote,mem);
    return query(port,act,"");
}


bool NameServiceOnTriples::cmdQuery(NameTripleState& act, bool nested)
{
    std::string port = act.cmd.get(1).asString();

    ParseName parser;
    parser.apply(port);
    port = parser.getPortName();

    /*
    // port names may be prefixed - sort that out
    std::string base = port;
    std::string pat = "";
    if (base.find("/net=") == 0 || base.find("/NET=") == 0) {
        int patStart = 5;
        int patEnd = base.find('/',patStart);
        if (patEnd>=patStart) {
            pat = base.substr(patStart,patEnd-patStart);
            base = base.substr(patEnd);
        }
        port = base;
    }
    */

    if (act.reply.size()==0 && !act.bottleMode) {
        act.reply.addString("old");
    }
    Bottle& q=(act.bottleMode&&!act.nestedMode)?
        act.reply :
        act.reply.addList();
    Contact c = query(port, act, parser.getNetworkChoice(), nested);
    std::string host = c.getHost();
    std::string carrier = c.getCarrier();
    int sock = c.getPort();
    if (c.isValid()) {
        if (!act.bottleMode) {
            q.addString("registration");
            q.addString("name");
            q.addString(port);
            q.addString("ip");
            q.addString(host);
            q.addString("port");
            q.addInt32(sock);
            q.addString("type");
            q.addString(carrier);
        } else {
            Bottle bname;
            bname.addString("name");
            bname.addString(port);
            Bottle bip;
            bip.addString("ip");
            bip.addString(host);
            Bottle bnum;
            bnum.addString("port_number");
            bnum.addInt32(sock);
            Bottle bcarrier;
            bcarrier.addString("carrier");
            bcarrier.addString(carrier);
            q.addString("port");
            q.addList() = bname;
            q.addList() = bip;
            q.addList() = bnum;
            q.addList() = bcarrier;
        }
    } else {
        if (act.bottleMode) {
            Bottle bstate;
            bstate.addString("error");
            bstate.addInt32(-2);
            bstate.addString("port not known");
            q.addString("port");
            q.addList() = bstate;
        }
    }
    return true;
}

bool NameServiceOnTriples::cmdRegister(NameTripleState& act)
{
    std::string port = act.cmd.get(1).asString();

    lock();
    Triple t;
    t.setNameValue("port",port.c_str());
    int result = act.mem.find(t, nullptr);
    unlock();

    if (result!=-1) {
        // Hmm, we already have a registration.
        // This could be fine - maybe program crashed or was abruptly
        // terminated.  Classically, that is what YARP would assume.
        // Now, let us try checking instead to see if there is a port
        // alive at the registered address.  Note that this can lead
        // to delays...
#if 0
        Contact c = query(port.c_str());
        if (c.isValid()) {
            if (gonePublic) {
                printf(" ? checking prior registration, to avoid accidental collision\n");
                Bottle cmd("[ver]"), reply;
                double timeout = 3.0;
                double pre = SystemClock::nowSystem();
                bool ok = Network::write(c,cmd,reply,true,true,timeout);
                double post = SystemClock::nowSystem();
                if (post-pre>timeout-1) {
                    ok = false;
                }
                if (ok) {
                    printf(" ? prior registration seems to be live! Denying new registration.\n");
                    /*
                      printf("Got a live one! %s - said %s\n",
                      port.c_str(),
                      reply.toString().c_str());
                    */
                    // oops! there is a live port!
                    // give back a blank query
                    act.cmd.fromString("query");
                    return cmdQuery(act);
                } else {
                    printf(" ! prior registration seems to be no longer valid, good!\n");
                }
            }
        }
#endif
        cmdUnregister(act);
        act.reply.clear();
    }

    act.reply.addString("old");

    size_t at = 2;
    int sock = -1;
    std::string carrier = "...";
    std::string machine = "...";
    std::string typ = "*";
    if (act.cmd.size()>at) {
        carrier = act.cmd.get(at).asString();
        at++;
    }
    if (carrier=="...") {
        carrier = "tcp";
    }
    if (act.cmd.size()>at) {
        machine = act.cmd.get(at).asString();
        at++;
    }
    if (machine == "...") {
        if (carrier=="topic") {
            machine = serverContact.getHost();
        } else if (carrier!="mcast") {
            std::string remote = act.remote.getHost();
            if (remote.empty() || remote == "...") {
                //fprintf(stderr,"Not detecting real remote machine name, guessing local\n");
                machine = "localhost";
            } else {
                machine = remote;
            }
        }
    }
    if (act.cmd.size()>at) {
        sock = act.cmd.get(at).asInt32();
        at++;
    } else {
        if (carrier=="topic") {
            sock = serverContact.getPort();
        }
    }
    if (act.cmd.size()>at) {
        typ = act.cmd.get(at).asString();
        at++;
    }
    lock();
    if (port=="..." || (port.length()>0 && port[0]=='=')) {
        Contact c(port, carrier, machine, sock);
        c = alloc->completePortName(c);
        if (port =="...") {
            port = c.getName();
        } else {
            port = c.getName() + port;
        }
    }
    t.setNameValue("port",port.c_str());
    act.mem.remove_query(t, nullptr);
    act.mem.insert(t, nullptr);
    result = act.mem.find(t, nullptr);
    TripleContext context;
    context.setRid(result);
    t.setNameValue("carrier",carrier.c_str());
    act.mem.update(t,&context);
    char buf[100];
    Contact c(port, carrier, machine, sock);
    c = alloc->completeSocket(c);
    sock = c.getPort();
    machine = c.getHost();
    t.setNameValue("host",machine.c_str());
    act.mem.update(t,&context);
    sprintf(buf,"%d",sock);
    t.setNameValue("socket",buf);
    act.mem.update(t,&context);
    if (typ!="*") {
        t.setNameValue("type",typ.c_str());
        act.mem.update(t,&context);
    }
    // now, query to report that it worked
    act.mem.reset();
    act.cmd.clear();
    act.cmd.addString("query");
    act.cmd.addString(port);

    if (carrier!="mcast") {
        Bottle& event = act.event.addList();
        event.addVocab32("add");
        event.addString(port);
    }
    unlock();

    return cmdQuery(act);
}


bool NameServiceOnTriples::announce(const std::string& name, int activity)
{
    if (subscriber != nullptr && gonePublic) {
        subscriber->welcome(name,activity);
    }
    return true;
}

bool NameServiceOnTriples::cmdUnregister(NameTripleState& act)
{
    std::string port = act.cmd.get(1).asString();
    //printf(" - unregister %s\n", port.c_str());
    announce(port,-1);
    lock();
    Contact contact = query(port,act,"",true);
    alloc->freePortResources(contact);
    act.reply.addString("old");
    Triple t;
    t.setNameValue("port",port.c_str());
    int result = act.mem.find(t, nullptr);
    TripleContext context;
    context.setRid(result);
    if (result!=-1) {
        t.setNameValue("owns","*");
        list<Triple> lst = act.mem.query(t,&context);
        unlock();
        for (auto& it : lst) {
            act.cmd.clear();
            act.cmd.addString("unregister");
            act.cmd.addString(it.value.c_str());
            cmdUnregister(act);
        }
        lock();
        t.setNsNameValue("*","*","*");
        act.mem.remove_query(t,&context);

        t.setNameValue("port",port.c_str());
        act.mem.remove_query(t, nullptr);
        // now, query to report that there is nothing there

        if (contact.getCarrier()!="mcast") {
            Bottle& event = act.event.addList();
            event.addVocab32("del");
            event.addString(port);
        }
    }

    act.mem.reset();
    unlock();

    return cmdQuery(act);
}

bool NameServiceOnTriples::cmdListRunners(NameTripleState& act)
{ // this is a combination of cmdList and cmdCheck codes
    if (!act.bottleMode)
    {
        act.reply.addString("old");
    } else
    {
        act.reply.addString("ports");
    }
    lock();
    Triple t;
    t.setNameValue("port","*");

    // obtain all ports names
    list<Triple> lst = act.mem.query(t, nullptr);
    act.nestedMode = true;

    for (auto& it : lst)
    { // check yarprun property for each port
        std::string port    = it.value;
        act.mem.reset();

        Triple t;
        t.setNameValue("port",port.c_str());
        int rid = act.mem.find(t, nullptr);
        if (rid == -1) {
            unlock();
            return false;
        }

        // find all triples with yarprun = true for the specified RID (at most one)
        TripleContext context;
        context.setRid(rid);
        t.setNameValue("yarprun","true");
        list<Triple> lst = act.mem.query(t,&context);

        if (!lst.empty())
        { // if the port is a runner, do a classic query to build the reply with complete information about the port
            act.cmd.clear();
            act.cmd.addString("query");
            act.cmd.addString(port);
            act.mem.reset();
            cmdQuery(act, true);
        }
    }
    unlock();
    return true;
}

bool NameServiceOnTriples::cmdList(NameTripleState& act)
{
    if (!act.bottleMode) {
        act.reply.addString("old");
    } else {
        act.reply.addString("ports");
    }
    lock();
    Triple t;
    t.setNameValue("port","*");
    std::string prefix;
    if (act.cmd.size()>1) {
        prefix = act.cmd.get(1).asString();
    }
    list<Triple> lst = act.mem.query(t, nullptr);
    act.nestedMode = true;
    for (auto& it : lst) {
        if (prefix.empty()) {
            act.cmd.clear();
            act.cmd.addString("query");
            act.cmd.addString(it.value.c_str());
            act.mem.reset();
            cmdQuery(act,true);
        } else {
            std::string iname = it.value;
            if (iname.find(prefix)==0) {
                if (iname==prefix || iname[prefix.length()]=='/' ||
                    prefix[prefix.length()-1]=='/') {
                    act.cmd.clear();
                    act.cmd.addString("query");
                    act.cmd.addString(iname);
                    act.mem.reset();
                    cmdQuery(act,true);
                }
            }
        }
    }
    unlock();
    return true;
}


bool NameServiceOnTriples::cmdSet(NameTripleState& act)
{
    lock();
    if (!act.bottleMode) {
        act.reply.addString("old");
    }
    std::string port = act.cmd.get(1).asString();
    std::string key = act.cmd.get(2).toString();
    int at = 3;
    int n = act.cmd.size() - at;
    Triple t;
    t.setNameValue("port", port.c_str());
    int result = act.mem.find(t, nullptr);
    if (result==-1) {
        unlock();
        return false;
    }
    TripleContext context;
    context.setRid(result);
    t.setNameValue(key.c_str(),"*");
    act.mem.remove_query(t,&context);
    for (int i=0; i<n; i++) {
        t.setNameValue(key.c_str(),act.cmd.get(at).toString().c_str());
        at++;
        act.mem.insert(t,&context);
    }
    act.mem.reset();
    act.cmd.clear();
    act.cmd.addString("get");
    act.cmd.addString(port);
    act.cmd.addString(key);
    unlock();
    return cmdGet(act);
}


bool NameServiceOnTriples::cmdGet(NameTripleState& act)
{
    lock();
    if (!act.bottleMode) {
        if (act.reply.size()==0) {
            act.reply.addString("old");
        }
    }
    std::string port = act.cmd.get(1).asString();
    std::string key = act.cmd.get(2).toString();
    Triple t;
    t.setNameValue("port",port.c_str());
    int result = act.mem.find(t, nullptr);
    if (result==-1) {
        unlock();
        return false;
    }
    TripleContext context;
    context.setRid(result);
    t.setNameValue(key.c_str(),"*");
    list<Triple> lst = act.mem.query(t,&context);
    Bottle& q = (act.bottleMode?act.reply:act.reply.addList());
    if (!act.bottleMode) {
        q.addString("port");
        q.addString(port);
        q.addString("property");
        q.addString(key);
        q.addString("=");
        for (auto& it : lst) {
            q.addString(it.value.c_str());
        }
    } else {
        for (auto& it : lst) {
            Value v;
            v.fromString(it.value.c_str());
            q.add(v);
        }
    }
    unlock();
    return true;
}


bool NameServiceOnTriples::cmdCheck(NameTripleState& act)
{
    lock();
    if (act.reply.size()==0) {
        act.reply.addString("old");
    }
    std::string port = act.cmd.get(1).asString();
    std::string key = act.cmd.get(2).toString();
    std::string val = act.cmd.get(3).toString();
    Triple t;
    t.setNameValue("port",port.c_str());
    int result = act.mem.find(t, nullptr);
    if (result==-1) {
        unlock();
        return false;
    }
    TripleContext context;
    context.setRid(result);
    t.setNameValue(key.c_str(),"*");
    list<Triple> lst = act.mem.query(t,&context);
    Bottle& q = act.reply.addList();
    q.addString("port");
    q.addString(port);
    q.addString("property");
    q.addString(key);
    q.addString("value");
    q.addString(val);
    q.addString("present");
    std::string present = "false";
    for (auto& it : lst) {
        if (val == it.value) {
            present = "true";
        }
    }
    q.addString(present);
    unlock();
    return true;
}


bool NameServiceOnTriples::cmdRoute(NameTripleState& act)
{
    if (act.reply.size()==0) {
        act.reply.addString("old");
    }
    std::string port1 = act.cmd.get(1).asString();
    std::string port2 = act.cmd.get(2).asString();
    Bottle& q = act.reply.addList();
    q.addString("port");
    q.addString(port1);
    q.addString("route");
    q.addString(port2);
    q.addString("=");
    q.addString(std::string("tcp:/") + port2);
    return true;
}

bool NameServiceOnTriples::cmdGc(NameTripleState& act)
{
    act.reply.addString("old");
    Bottle& q = act.reply.addList();
    // nothing needed
    q.addString("garbage collection done.");
    return true;
}


bool NameServiceOnTriples::cmdHelp(NameTripleState& act)
{
    Bottle& bot = act.reply;
    if (!act.bottleMode) {
        bot.addString("old");
        bot.addString("Here are some ways to use the name server:");
    }
    bot.addString("+ help");
    bot.addString("+ list [$prefix]");
    bot.addString("+ register $portname");
    bot.addString("+ register $portname $carrier $ipAddress $portNumber");
    bot.addString("  (if you want a field set automatically, write '...')");
    bot.addString("+ unregister $portname");
    bot.addString("+ query $portname");
    bot.addString("+ set $portname $property $value");
    bot.addString("+ get $portname $property");
    bot.addString("+ check $portname $property");
    bot.addString("+ route $port1 $port2");
    bot.addString("+ runners");
    bot.addString("  (to get a list of the yarprun ports)");
    return true;
}

bool NameServiceOnTriples::apply(yarp::os::Bottle& cmd,
                                 yarp::os::Bottle& reply,
                                 yarp::os::Bottle& event,
                                 const yarp::os::Contact& remote)
{
    std::string key = cmd.get(0).toString();
    std::string prefix = " * ";

    access.wait();
    if (key=="register") {
        lastRegister = cmd.get(1).asString();
    } else if (key=="set") {
        if (cmd.get(1).asString()==lastRegister) {
            prefix = "   + ";
        }
    } else {
        lastRegister = "";
    }
    if (!silent) {
        yCInfo(NAMESERVICEONTRIPLES, "%s%s", prefix.c_str(), cmd.toString().c_str());
    }
    access.post();

    TripleSource& mem = *db;
    //mem.begin();
    mem.reset();
    reply.clear();
    NameTripleState act(cmd,reply,event,remote,mem);

    if (cmd.check("format")) {
        if (cmd.find("format").asString()=="json") {
            act.bottleMode = true;
        }
    }

    if (key == "NAME_SERVER") {
        cmd = cmd.tail();
        key = cmd.get(0).asString();
    }
    if (key == "bot") {
        act.bottleMode = true;
        cmd = cmd.tail();
        key = cmd.get(0).asString();
    }

    if (key=="register") {
        return cmdRegister(act);
    }
    if (key=="unregister") {
        return cmdUnregister(act);
    }
    if (key=="query") {
        return cmdQuery(act);
    }
    if (key=="list") {
        return cmdList(act);
    }
    if (key=="runners") {
        return cmdListRunners(act);
    }
    if (key=="set") {
        return cmdSet(act);
    }
    if (key=="get") {
        return cmdGet(act);
    }
    if (key=="check") {
        return cmdCheck(act);
    }
    if (key=="route") {
        return cmdRoute(act);
    }
    if (key=="gc") {
        return cmdGc(act);
    }
    if (key=="help") {
        return cmdHelp(act);
    }

    // not understood
    act.reply.addString("old");

    //mem.end();

    return true;
}



void NameServiceOnTriples::lock()
{
    mutex.lock();
    db->begin(nullptr);
}

void NameServiceOnTriples::unlock()
{
    db->end(nullptr);
    mutex.unlock();
}
