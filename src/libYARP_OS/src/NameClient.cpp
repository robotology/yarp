// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <yarp/os/impl/NameClient.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/TcpFace.h>
#include <yarp/os/impl/NetType.h>
#include <yarp/os/impl/NameServer.h>
#include <yarp/os/impl/NameConfig.h>
#include <yarp/os/impl/FallbackNameClient.h>
#include <yarp/os/Network.h>

using namespace yarp::os::impl;
using namespace yarp::os;

/**
 * The NameClient singleton
 */


NameClient *NameClient::instance = NULL;



/*
  Old class for splitting string based on spaces
*/

#define MAX_ARG_CT (20)
#define MAX_ARG_LEN (256)


#ifndef DOXYGEN_SHOULD_SKIP_THIS

class Params {
private:
    int argc;
    const char *argv[MAX_ARG_CT];
    char buf[MAX_ARG_CT][MAX_ARG_LEN];

public:
    Params() {
        argc = 0;
    }

    Params(const char *command) {
        apply(command);
    }

    int size() {
        return argc;
    }

    const char *get(int idx) {
        return buf[idx];
    }

    void apply(const char *command) {
        int at = 0;
        int sub_at = 0;
        unsigned int i;
        for (i=0; i<strlen(command)+1; i++) {
            if (at<MAX_ARG_CT) {
                char ch = command[i];
                if (ch>=32||ch=='\0'||ch=='\n') {
                    if (ch==' '||ch=='\n') {
                        ch = '\0';
                    }
                    if (sub_at<MAX_ARG_LEN) {
                        buf[at][sub_at] = ch;
                        sub_at++;
                    }
                }
                if (ch == '\0') {
                    if (sub_at>1) {
                        at++;
                    }
                    sub_at = 0;
                } 
            }
        }
        for (i=0; i<MAX_ARG_CT; i++) {
            argv[i] = buf[i];
            buf[i][MAX_ARG_LEN-1] = '\0';
        }

        argc = at;
    }
};

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/





Address NameClient::extractAddress(const String& txt) {
    Address result;
    Params p(txt.c_str());
    if (p.size()>=9) {
        // registration name /bozo ip 5.255.112.225 port 10002 type tcp
        if (String(p.get(0))=="registration") {
            const char *regName = p.get(2);
            const char *ip = p.get(4);
            int port = atoi(p.get(6));
            const char *carrier = p.get(8);
            result = Address(ip,port,carrier,regName);
        }
    }
    return result;
}


String NameClient::send(const String& cmd, bool multi) {
    bool retried = false;
    bool retry = false;
    String result;

    do {

        YARP_DEBUG(Logger::get(),String("sending to nameserver: ") + cmd);

        if (isFakeMode()) {
            //YARP_DEBUG(Logger::get(),"fake mode nameserver");
            return getServer().apply(cmd,Address("localhost",10000,"tcp")) + "\n";
        }
        
        TcpFace face;
        YARP_DEBUG(Logger::get(),String("connecting to ") + getAddress().toString());
        OutputProtocol *ip = NULL;
        if (!retry) {
            ip = face.write(getAddress());
        } else {
            retried = true;
        }
        if (ip==NULL) {
            YARP_INFO(Logger::get(),"No connection to nameserver");
            if (!allowScan) {
                YARP_INFO(Logger::get(),"*** try running: yarp detect ***");
            }
            Address alt;
            if (!isFakeMode()) {
                if (allowScan) {
                    YARP_INFO(Logger::get(),"no connection to nameserver, scanning mcast");
                    reportScan = true;
                    alt = FallbackNameClient::seek();
                }
            }
            if (alt.isValid()) {
                address = alt;
                if (allowSaveScan) {
                    reportSaveScan = true;
                    NameConfig nc;
                    nc.setAddress(alt);
                    nc.toFile();
                }
                ip = face.write(getAddress());
                if (ip==NULL) {
                    YARP_ERROR(Logger::get(),
                               "no connection to nameserver, scanning mcast");
                    return "";
                }
            } else {
                return "";
            }
        }
        String cmdn = cmd + "\n";
        Bytes b((char*)cmdn.c_str(),cmdn.length());
        ip->getOutputStream().write(b);
        bool more = multi;
        while (more) {
            String line = "";
            line = NetType::readLine(ip->getInputStream());
            if (!(ip->checkStreams())) {
                more = false;
                //YARP_DEBUG(Logger::get(), e.toString() + " <<< exception from name server");
                retry = true;
                break;
            }
            if (line.length()>1) {
                if (line[0] == '*') {
                    more = false;
                }
            }
            result += line + "\n";
        }
        ip->close();
        delete ip;
        ACE_DEBUG((LM_DEBUG,"<<< received from nameserver: %s",result.c_str()));
    } while (retry&&!retried);

    return result;
}


bool NameClient::send(Bottle& cmd, Bottle& reply) {
    if (isFakeMode()) {
        YARP_DEBUG(Logger::get(),"fake mode nameserver");
        return getServer().apply(cmd,reply,
                                 Address("localhost",10000,"tcp"));
    } else {
        ConstString server = Network::getNameServerName();
        return Network::write(server,cmd,reply);
    }
}



Address NameClient::queryName(const String& name) {
    String np = getNamePart(name);
    //if (isFakeMode()) {
    //return getServer().queryName(np);
    //}
    String q("NAME_SERVER query ");
    q += np;
    return probe(q);
}

Address NameClient::registerName(const String& name) {
    return registerName(name,Address());
}

Address NameClient::registerName(const String& name, const Address& suggest) {
    String np = getNamePart(name);
    //if (isFakeMode()) {
    //return getServer().registerName(np,suggest);
    //}
    String q("NAME_SERVER register ");
    q += (np!="")?np:"...";
    if (suggest.isValid()) {
        q += " ";
        q += (suggest.getCarrierName()!="")?suggest.getCarrierName():"...";
        q += " ";
        q += (suggest.getName()!="")?suggest.getName():"...";
        q += " ";
        if (suggest.getPort()==0) {
            q += "...";
        } else {
            q += NetType::toString(suggest.getPort());
        }
    }
    Address address = probe(q);
    if (address.isValid()) {
        String reg = address.getRegName();
        send(String("NAME_SERVER set ") + reg + " offers tcp text text_ack udp mcast shmem name_ser",
             false);
        send(String("NAME_SERVER set ") + reg + " accepts tcp text text_ack udp mcast shmem name_ser",
             false);
        String ips = NameConfig::getIps();
        send(String("NAME_SERVER set ") + reg + " ips " + ips,
             false);
        send(String("NAME_SERVER set ") + reg + " process " + process,
             false);
    }
    return address;
}

Address NameClient::unregisterName(const String& name) {
    String np = getNamePart(name);
    //if (isFakeMode()) {
    //return getServer().unregisterName(np);
    //}
    String q("NAME_SERVER unregister ");
    q += np;
    return probe(q);
}


NameClient::~NameClient() {
    if (fakeServer!=NULL) {
        delete fakeServer;
        fakeServer = NULL;
    }
}

NameServer& NameClient::getServer() {
    if (fakeServer==NULL) {
        fakeServer = new NameServer;
    }
    YARP_ASSERT(fakeServer!=NULL);
    return *fakeServer;
}



NameClient::NameClient() {
    allowScan = false;
    allowSaveScan = false;
    reportScan = false;
    reportSaveScan = false;
    NameConfig conf;
    address = Address();
    process = NetType::toString(ACE_OS::getpid());
    if (conf.fromFile()) {
        address = conf.getAddress();
    } else {
        YARP_ERROR(Logger::get(),"Cannot find name server");
        //address = Address("localhost",10000);
    }
    YARP_DEBUG(Logger::get(),String("name server address is ") + 
               address.toString());
    fake = false;
    fakeServer = NULL;
}
