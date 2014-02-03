// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include <yarp/os/Vocab.h>
#include <yarp/os/Network.h>
#include <yarp/os/Time.h>
#include "NameServiceOnTriples.h"
#include "ParseName.h"

using namespace yarp::os;
using namespace std;

//#define mutex printf("mutex %s %d\n", __FILE__, __LINE__), mutex


Contact NameServiceOnTriples::query(const yarp::os::ConstString& portName, 
                                    NameTripleState& act,
                                    const yarp::os::ConstString& prefix,
                                    bool nested) {
    if (!nested) lock();
    Triple t;
    t.setNameValue("port",portName.c_str());
    int result = act.mem.find(t,NULL);
    TripleContext context;
    context.setRid(result);
    if (result!=-1) {
        string host = "";
        if (string(prefix)!="") {
            printf("LOOKING AT IPS FOR %s\n", prefix.c_str());
            t.setNameValue("ips","*");
            list<Triple> lst = act.mem.query(t,&context);
            for (list<Triple>::iterator it=lst.begin();it!=lst.end();it++) {
                printf("LOOKING AT IPS %s\n", it->value.c_str());
                if (it->value.find(prefix)==0) {
                    host = it->value;
                    break;
                }
            }
        }
        if (host=="") {
            t.setNameValue("host","*");
            list<Triple> lst = act.mem.query(t,&context);
            if (lst.size()>0) {
                host = lst.begin()->value.c_str();
            }
        }
        if (host=="") {
            host = "localhost";
        }
        t.setNameValue("socket","*");
        list<Triple> lst = act.mem.query(t,&context);
        int sock = 10000;
        if (lst.size()>0) {
            sock = atoi(lst.begin()->value.c_str());
        }
        t.setNameValue("carrier","*");
        string carrier = "tcp";
        lst = act.mem.query(t,&context);
        if (lst.size()>0) {
            carrier = lst.begin()->value.c_str();
        }
        if (!nested) unlock();
        return Contact::byName(portName).addSocket(carrier.c_str(),
                                                   host.c_str(),
                                                   sock);
    }
    if (!nested) unlock();
    if (delegate && !nested) {
        return delegate->queryName(portName);
    }
    return Contact::invalid();
}


yarp::os::Contact NameServiceOnTriples::query(const ConstString& port) {
    Contact check = Contact::fromString(port);
    if (check.getHost()!="") return check;
    Bottle cmd,reply,event;
    Contact remote;
    TripleSource& mem = *db;
    NameTripleState act(cmd,reply,event,remote,mem);
    return query(port,act,"");
}


bool NameServiceOnTriples::cmdQuery(NameTripleState& act, bool nested) {
    ConstString port = act.cmd.get(1).asString();

    ParseName parser;
    parser.apply(port.c_str());
    port = parser.getPortName();

    /*
    // port names may be prefixed - sort that out
    string base = port.c_str();
    string pat = "";
    if (base.find("/net=") == 0 || base.find("/NET=") == 0) {
        int patStart = 5;
        int patEnd = base.find('/',patStart);
        if (patEnd>=patStart) {
            pat = base.substr(patStart,patEnd-patStart);
            base = base.substr(patEnd);
        }
        port = base.c_str();
    }
    */

    if (act.reply.size()==0 && !act.bottleMode) {
        act.reply.addString("old");
    }
    Bottle& q=(act.bottleMode&&!act.nestedMode)?
        act.reply :
        act.reply.addList();
    Contact c = query(port.c_str(),act,parser.getNetworkChoice(),nested);
    string host = c.getHost().c_str();
    string carrier = c.getCarrier().c_str();
    int sock = c.getPort();
    if (c.isValid()) {
        if (!act.bottleMode) {
            q.addString("registration");
            q.addString("name");
            q.addString(port.c_str());
            q.addString("ip");
            q.addString(host.c_str());
            q.addString("port");
            q.addInt(sock);
            q.addString("type");
            q.addString(carrier.c_str());
        } else {
            Bottle bname;
            bname.addString("name");
            bname.addString(port.c_str());
            Bottle bip;
            bip.addString("ip");
            bip.addString(host.c_str());
            Bottle bnum;
            bnum.addString("port_number");
            bnum.addInt(sock);
            Bottle bcarrier;
            bcarrier.addString("carrier");
            bcarrier.addString(carrier.c_str());
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
            bstate.addInt(-2);
            bstate.addString("port not known");
            q.addString("port");
            q.addList() = bstate;
        }
    }
    return true;
}

bool NameServiceOnTriples::cmdRegister(NameTripleState& act) {
    ConstString port = act.cmd.get(1).asString();

    lock();
    Triple t;
    t.setNameValue("port",port.c_str());
    int result = act.mem.find(t,NULL);
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
                double pre = Time::now();
                bool ok = Network::write(c,cmd,reply,true,true,timeout);
                double post = Time::now();
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
    
    int at = 2;
    int sock = -1;
    ConstString carrier = "...";
    ConstString machine = "...";
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
            machine = serverContact.getHost().c_str();
        } else if (carrier!="mcast") {
            string remote = act.remote.getHost().c_str();
            if (remote==""||remote=="...") {
                //fprintf(stderr,"Not detecting real remote machine name, guessing local\n");
                machine = "localhost";
            } else {
                machine = remote.c_str();
            }
        }
    }
    if (act.cmd.size()>at) {
        sock = act.cmd.get(at).asInt();
        at++;
    } else {
        if (carrier=="topic") {
            sock = serverContact.getPort();
        }
    }
    lock();
    if (port=="..." || (port.length()>0 && port[0]=='=')) {
        Contact c = Contact::byName(port.c_str()).addSocket(carrier.c_str(),machine.c_str(),sock);
        c = alloc->completePortName(c);
        if (port =="...") {
            port = c.getName();
        } else {
            port = c.getName() + port;
        }
    }
    t.setNameValue("port",port.c_str());
    act.mem.remove_query(t,NULL);
    act.mem.insert(t,NULL);
    result = act.mem.find(t,NULL);
    TripleContext context;
    context.setRid(result);
    t.setNameValue("carrier",carrier.c_str());
    act.mem.update(t,&context);
    char buf[100];
    Contact c = Contact::byName(port.c_str()).addSocket(carrier.c_str(),machine.c_str(),sock);
    c = alloc->completeSocket(c);
    sock = c.getPort();
    machine = c.getHost().c_str();
    t.setNameValue("host",machine.c_str());
    act.mem.update(t,&context);
    sprintf(buf,"%d",sock);
    t.setNameValue("socket",buf);
    act.mem.update(t,&context);
    // now, query to report that it worked
    act.mem.reset();
    act.cmd.clear();
    act.cmd.addString("query");
    act.cmd.addString(port.c_str());

    if (carrier!="mcast") {
        Bottle& event = act.event.addList();
        event.addVocab(Vocab::encode("add"));
        event.addString(port.c_str());
    }
    unlock();

    return cmdQuery(act);
}


bool NameServiceOnTriples::announce(const ConstString& name, int activity) {
    if (subscriber!=NULL&&gonePublic) {
        subscriber->welcome(name,activity);
    }
    return true;
}

bool NameServiceOnTriples::cmdUnregister(NameTripleState& act) {
    ConstString port = act.cmd.get(1).asString();
    //printf(" - unregister %s\n", port.c_str());
    announce(port.c_str(),-1);
    lock();
    Contact contact = query(port.c_str(),act,"",true);
    alloc->freePortResources(contact);
    act.reply.addString("old");
    Triple t;
    t.setNameValue("port",port.c_str());
    int result = act.mem.find(t,NULL);
    TripleContext context;
    context.setRid(result);
    if (result!=-1) {
        t.setNameValue("owns","*");
        list<Triple> lst = act.mem.query(t,&context);
        unlock();
        for (list<Triple>::iterator it=lst.begin();it!=lst.end();it++) {
            act.cmd.clear();
            act.cmd.addString("unregister");
            act.cmd.addString(it->value.c_str());
            cmdUnregister(act);
        }
        lock();
        t.setNsNameValue("*","*","*");
        act.mem.remove_query(t,&context);

        t.setNameValue("port",port.c_str());
        act.mem.remove_query(t,NULL);
        // now, query to report that there is nothing there
 
        if (contact.getCarrier()!="mcast") {
            Bottle& event = act.event.addList();
            event.addVocab(Vocab::encode("del"));
            event.addString(port.c_str());
        }
    }

    act.mem.reset();
    unlock();

    return cmdQuery(act);
}


bool NameServiceOnTriples::cmdList(NameTripleState& act) {
    if (!act.bottleMode) {
        act.reply.addString("old");
    } else {
        act.reply.addString("ports");
    }
    lock();
    Triple t;
    t.setNameValue("port","*");
    string prefix = "";
    if (act.cmd.size()>1) {
        prefix = act.cmd.get(1).asString().c_str();
    }
    list<Triple> lst = act.mem.query(t,NULL);
    act.nestedMode = true;
    for (list<Triple>::iterator it=lst.begin(); it!=lst.end(); it++) {
        if (prefix=="") {
            act.cmd.clear();
            act.cmd.addString("query");
            act.cmd.addString(it->value.c_str());
            act.mem.reset();
            cmdQuery(act,true);
        } else {
            string iname = it->value.c_str();
            if (iname.find(prefix)==0) {
                if (iname==prefix || iname[prefix.length()]=='/' || 
                    prefix[prefix.length()-1]=='/') {
                    act.cmd.clear();
                    act.cmd.addString("query");
                    act.cmd.addString(iname.c_str());
                    act.mem.reset();
                    cmdQuery(act,true);
                }
            }
        }
    }
    unlock();
    return true;
}


bool NameServiceOnTriples::cmdSet(NameTripleState& act) {
    lock();
    if (!act.bottleMode) {
        act.reply.addString("old");
    }
    ConstString port = act.cmd.get(1).asString();
    ConstString key = act.cmd.get(2).toString();
    int at = 3;
    int n = act.cmd.size()-at;
    Triple t;
    t.setNameValue("port",port.c_str());
    int result = act.mem.find(t,NULL);
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
    act.cmd.addString(port.c_str());
    act.cmd.addString(key.c_str());
    unlock();
    return cmdGet(act);
}


bool NameServiceOnTriples::cmdGet(NameTripleState& act) {
    lock();
    if (!act.bottleMode) {
        if (act.reply.size()==0) {
            act.reply.addString("old");
        }
    }
    ConstString port = act.cmd.get(1).asString();
    ConstString key = act.cmd.get(2).toString();
    Triple t;
    t.setNameValue("port",port.c_str());
    int result = act.mem.find(t,NULL);
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
        q.addString(port.c_str());
        q.addString("property");
        q.addString(key.c_str());
        q.addString("=");
        for (list<Triple>::iterator it=lst.begin(); it!=lst.end(); it++) {
            q.addString(it->value.c_str());
        }
    } else {
        for (list<Triple>::iterator it=lst.begin(); it!=lst.end(); it++) {
            Value v;
            v.fromString(it->value.c_str());
            q.add(v);
        }
    }
    unlock();
    return true;
}


bool NameServiceOnTriples::cmdCheck(NameTripleState& act) {
    lock();
    if (act.reply.size()==0) {
        act.reply.addString("old");
    }
    ConstString port = act.cmd.get(1).asString();
    ConstString key = act.cmd.get(2).toString();
    ConstString val = act.cmd.get(3).toString();
    Triple t;
    t.setNameValue("port",port.c_str());
    int result = act.mem.find(t,NULL);
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
    q.addString(port.c_str());
    q.addString("property");
    q.addString(key.c_str());
    q.addString("value");
    q.addString(val.c_str());
    q.addString("present");
    string present = "false";
    for (list<Triple>::iterator it=lst.begin(); it!=lst.end(); it++) {
        if (val == it->value.c_str()) {
            present = "true";
        }
    }
    q.addString(present.c_str());
    unlock();
    return true;
}


bool NameServiceOnTriples::cmdRoute(NameTripleState& act) {
    if (act.reply.size()==0) {
        act.reply.addString("old");
    }
    ConstString port1 = act.cmd.get(1).asString();
    ConstString port2 = act.cmd.get(2).asString();
    Bottle& q = act.reply.addList();
    q.addString("port");
    q.addString(port1.c_str());
    q.addString("route");
    q.addString(port2.c_str());
    q.addString("=");
    q.addString((string("tcp:/")+port2.c_str()).c_str());
    return true;
}

bool NameServiceOnTriples::cmdGc(NameTripleState& act) {
    act.reply.addString("old");
    Bottle& q = act.reply.addList();
    // nothing needed
    q.addString("garbage collection done.");
    return true;
}


bool NameServiceOnTriples::cmdHelp(NameTripleState& act) {
    Bottle& bot = act.reply;
    if (!act.bottleMode) {
        bot.addString("old");
        bot.addString("Here are some ways to use the name server:");
    }
    bot.addString("+ help");
    bot.addString("+ list");
    bot.addString("+ register $portname");
    bot.addString("+ register $portname $carrier $ipAddress $portNumber");
    bot.addString("  (if you want a field set automatically, write '...')");
    bot.addString("+ unregister $portname");
    bot.addString("+ query $portname");
    bot.addString("+ set $portname $property $value");
    bot.addString("+ get $portname $property");
    bot.addString("+ check $portname $property");
    bot.addString("+ match $portname $property $prefix");
    bot.addString("+ route $port1 $port2");
    return true;
}

bool NameServiceOnTriples::apply(yarp::os::Bottle& cmd, 
                                 yarp::os::Bottle& reply, 
                                 yarp::os::Bottle& event,
                                 const yarp::os::Contact& remote) {
    ConstString key = cmd.get(0).toString();
    ConstString prefix = " * ";
    
    if (key=="register") {
        lastRegister = cmd.get(1).asString().c_str();
    } else if (key=="set") {
        if (cmd.get(1).asString()==lastRegister.c_str()) {
            prefix = "   + ";
        }
    } else {
        lastRegister = "";
    }
    if (!silent) {
        printf("%s%s\n", 
               prefix.c_str(),
               cmd.toString().c_str());
    }

    TripleSource& mem = *db;
    //mem.begin();
    mem.reset();
    reply.clear();
    NameTripleState act(cmd,reply,event,remote,mem);

    if (cmd.check("format")) {
        if (cmd.find("format")=="json") {
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
    } else if (key=="unregister") {
        return cmdUnregister(act);
    } else if (key=="query") {
        return cmdQuery(act);
    } else if (key=="list") {
        return cmdList(act);
    } else if (key=="set") {
        return cmdSet(act);
    } else if (key=="get") {
        return cmdGet(act);
    } else if (key=="check") {
        return cmdCheck(act);
    } else if (key=="route") {
        return cmdRoute(act);
    } else if (key=="gc") {
        return cmdGc(act);
    } else if (key=="help") {
        return cmdHelp(act);
    } else {
        // not understood
        act.reply.addString("old");
    }
    //mem.end();

    return true;
}



void NameServiceOnTriples::lock() {
    mutex.wait();
    db->begin(NULL);
}

void NameServiceOnTriples::unlock() {
    db->end(NULL);
    mutex.post();
}


