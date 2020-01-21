/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <cstdio>
#include <cstdlib>

#include <yarp/serversql/impl/AllocatorOnTriples.h>

using namespace yarp::os;
using namespace yarp::serversql::impl;
using namespace std;

Contact AllocatorOnTriples::completePortName(const Contact& c) {
    string name;
    Triple t;
    t.setNsNameValue("alloc","tmpid","*");
    TripleContext context;
    context.setRid(db->find(t, nullptr));
    if (context.rid>=0) {
        t.setNsNameValue("alloc","*","free");
        list<Triple> match = db->query(t,&context);
        if (match.size()>0) {
            name = match.begin()->name;
        }
    }

    if (name=="") {
        if (tmpid==-1) {
            t.setNsNameValue("alloc","tmpid","*");
            list<Triple> lst = db->query(t, nullptr);
            if (lst.size()>0) {
                tmpid = atoi(lst.begin()->value.c_str());
            }
            if (tmpid==-1) {
                tmpid = 0;
            }
        }
        tmpid++;
        char buf[256];
        sprintf(buf,"%d",tmpid);
        t.setNsNameValue("alloc","tmpid",buf);
        db->update(t, nullptr);
        t.setNsNameValue("alloc","tmpid","*");
        context.setRid(db->find(t, nullptr));
        sprintf(buf,"/tmp/port/%u", tmpid);
        name = buf;
    }

    t.setNsNameValue("alloc",name.c_str(),"in_use");
    db->update(t,&context);

    return Contact(name,
                   c.getCarrier(),
                   c.getHost(),
                   c.getPort());
}


Contact AllocatorOnTriples::completeSocket(const Contact& c) {
    Contact tmp = completeHost(c);
    return completePortNumber(tmp);
}

Contact AllocatorOnTriples::completePortNumber(const Contact& c) {
    if (c.getPort()!=-1 && c.getPort()!=0) {
        return c;
    }

    // unlike standard yarp name server, port number allocation
    // is global across the network, rather than per machine.

    // we also try to keep port numbers stable for port names,
    // when possible.

    string npref;
    int pref = -1;
    string nstring;
    int number = -1;
    Triple t;
    t.setNsNameValue("alloc","regid","*");
    TripleContext context;
    context.setRid(db->find(t, nullptr));
    if (context.rid>=0) {
        t.setNsNameValue("prefer","*",c.getName().c_str());
        list<Triple> match = db->query(t,&context);
        if (match.size()>0) {
            npref = match.begin()->name;
            pref = atoi(npref.c_str());
            t.setNsNameValue("alloc",npref.c_str(),"in_use");
            match = db->query(t,&context);
            if (match.size()==0) {
                nstring = npref;
                number = pref;
            }
        }
    }

    if (nstring=="") {
        if (regid==-1) {
            Triple t;
            t.setNsNameValue("alloc","regid","*");
            list<Triple> lst = db->query(t, nullptr);
            if (lst.size()>0) {
                regid = atoi(lst.begin()->value.c_str());
            }
            if (regid==-1) {
                regid = config.minPortNumber-1;
            }
        }
        if (regid>=config.maxPortNumber && config.maxPortNumber!=0) {
            if (nstring == "") {
                t.setNsNameValue("alloc","*","free");
                list<Triple> match = db->query(t,&context);
                if (match.size()>0) {
                    nstring = match.begin()->name;
                    number = atoi(nstring.c_str());
                }
            }
            if (nstring=="") {
                fprintf(stderr,"Ran out of port numbers\n");
                fprintf(stderr,"* Make sure ports/programs get closed properly.\n");
                fprintf(stderr,"* If programs terminate without closing ports, run \"yarp clean\" from time to time..\n");
                std::exit(1);
            }
        } else {
            regid++;
            Triple t;
            char buf[256];
            sprintf(buf,"%d",regid);
            t.setNsNameValue("alloc","regid",buf);
            db->update(t, nullptr);
            t.setNsNameValue("alloc","regid","*");
            context.setRid(db->find(t, nullptr));
            nstring = buf;
            number = regid;
        }
    }
    t.setNsNameValue("alloc",nstring.c_str(),"in_use");
    db->update(t,&context);
    t.setNsNameValue("prefer",nstring.c_str(),c.getName().c_str());
    db->update(t,&context);

    Contact contact = c;
    contact.setPort(number);
    return contact;
}


Contact AllocatorOnTriples::completeHost(const yarp::os::Contact& c) {
    // deal with allocating multicast ips

    if (c.getCarrier()!="mcast") {
        return c;
    }
    if (c.getHost()!="...") {
        return c;
    }

    string name;
    Triple t;
    t.setNsNameValue("alloc","mcastCursor","*");
    TripleContext context;
    context.setRid(db->find(t, nullptr));
    if (context.rid>=0) {
        t.setNsNameValue("alloc","*","free");
        list<Triple> match = db->query(t,&context);
        if (match.size()>0) {
            name = match.begin()->name;
        }
    }

    if (name=="") {
        if (mcastCursor==-1) {
            t.setNsNameValue("alloc","mcastCursor","*");
            list<Triple> lst = db->query(t, nullptr);
            if (lst.size()>0) {
                mcastCursor = atoi(lst.begin()->value.c_str());
            }
            if (mcastCursor==-1) {
                mcastCursor = 1;
            }
        }
        mcastCursor++;
        char buf[256];
        sprintf(buf,"%d",mcastCursor);
        t.setNsNameValue("alloc","mcastCursor",buf);
        db->update(t, nullptr);
        t.setNsNameValue("alloc","mcastCursor","*");
        context.setRid(db->find(t, nullptr));

        int v1 = mcastCursor%255;
        int v2 = mcastCursor/255;
        if (v2>=255) {
            fprintf(stderr,"Ran out of mcast addresses\n");
            std::exit(1);
        }
        sprintf(buf,"224.1.%d.%d", v2+1,v1+1);
        name = buf;
    }

    t.setNsNameValue("alloc",name.c_str(),"in_use");
    db->update(t,&context);

    Contact contact = c;
    contact.setHost(name);
    return contact;
}


bool AllocatorOnTriples::freePortResources(const yarp::os::Contact& c) {
    string portName = c.getName();
    int portNumber = c.getPort();
    string hostName = c.getHost();

    // free up automatic name for port, if one was allocated
    Triple t;
    t.setNsNameValue("alloc","tmpid","*");
    TripleContext context;
    context.setRid(db->find(t, nullptr));
    t.setNsNameValue("alloc",portName.c_str(),"in_use");
    if (db->find(t,&context)>=0) {
        t.setNsNameValue("alloc",portName.c_str(),"free");
        db->update(t,&context);
    }

    t.setNsNameValue("alloc","regid","*");
    context.setRid(db->find(t, nullptr));
    char buf[256];
    sprintf(buf,"%d",portNumber);
    t.setNsNameValue("alloc",buf,"in_use");
    if (db->find(t,&context)>=0) {
        t.setNsNameValue("alloc",buf,"free");
        db->update(t,&context);
    }

    t.setNsNameValue("alloc","mcastCursor","*");
    context.setRid(db->find(t, nullptr));
    t.setNsNameValue("alloc",hostName.c_str(),"in_use");
    if (db->find(t,&context)>=0) {
        t.setNsNameValue("alloc",hostName.c_str(),"free");
        db->update(t,&context);
    }

    return true;
}
