// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia, Anne van Rossum
 * Authors: Paul Fitzpatrick, Anne van Rossum
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/Contact.h>
#include <yarp/os/impl/Address.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/NetType.h>
#include <yarp/os/impl/NameConfig.h>
#include <yarp/os/Value.h>

#ifdef YARP_HAS_ACE
#  include <ace/INET_Addr.h>
#else
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <netdb.h>
#  include <arpa/inet.h>
#endif

using namespace yarp::os::impl;
using namespace yarp::os;


// implementation is an Address
#define HELPER(x) (*((Address*)(x)))

Contact::Contact() {
    implementation = new Address();
    YARP_ASSERT(implementation!=NULL);
}

Contact::~Contact() {
    if (implementation!=NULL) {
        delete (Address*)implementation;
        implementation = NULL;
    }
}


Contact::Contact(const Contact& alt) {
    implementation = new Address();
    YARP_ASSERT(implementation!=NULL);
    HELPER(implementation) = HELPER(alt.implementation);
}


const Contact& Contact::operator = (const Contact& alt) {
    HELPER(implementation) = HELPER(alt.implementation);
    return (*this);
}


Contact Contact::empty() {
    return Contact();
}


Contact Contact::invalid() {
    return Contact().addSocket("","",-1);
}


Contact Contact::byName(const ConstString& name) {
    Contact result;
    HELPER(result.implementation) = Address().addRegName(name);
    return result;
}


Contact Contact::byCarrier(const ConstString& carrier) {
    Contact result;
    HELPER(result.implementation) = Address("",0,carrier);
    return result;
}

Contact Contact::addCarrier(const ConstString& carrier) const {
    Contact result;
    HELPER(result.implementation) = HELPER(implementation).addCarrier(carrier);
    return result;
}


Contact Contact::bySocket(const ConstString& carrier, 
                          const ConstString& machineName,
                          int portNumber) {
    Contact result;
    HELPER(result.implementation) = Address(machineName,portNumber,carrier);
    return result;
}


Contact Contact::addSocket(const ConstString& carrier, 
                           const ConstString& machineName,
                           int portNumber) const {
    Contact result;
    HELPER(result.implementation) = HELPER(implementation).addSocket(carrier,
                                                                     machineName,
                                                                     portNumber);
    return result;
}

Contact Contact::addName(const ConstString& name) const {
    Contact result;
    HELPER(result.implementation) = HELPER(implementation).addRegName(name);
    return result;
}


ConstString Contact::getName() const {
    Address& addr = HELPER(implementation);
    String name = addr.getRegName();
    if (name == "") {
        String host = addr.getName();
        if (host!="") {
            name = String("/") + host + ":" + 
                NetType::toString(addr.getPort());
        }
    }
    return name.c_str();
}


ConstString Contact::getHost() const {
    return HELPER(implementation).getName().c_str();
}


ConstString Contact::getCarrier() const {
    return HELPER(implementation).getCarrierName().c_str();
}


int Contact::getPort() const {
    return HELPER(implementation).getPort();
}


ConstString Contact::toString() const {
    Address& addr = HELPER(implementation);
    ConstString result = getName();
    if (addr.getCarrierName()!="") {
        return ConstString((addr.getCarrierName() + ":/" + result.c_str()).c_str());
    }
    return result;
}


Contact Contact::fromString(const ConstString& txt) {
    ConstString str(txt);
    Contact c;
    ConstString::size_type start = 0;
    ConstString::size_type base = str.find("://");
    ConstString::size_type offset = 2;
    if (base==ConstString::npos) {
        base = str.find(":/");
        offset = 1;
    }
    if (base==ConstString::npos) {
        if (str[0] == '/') {
            base = 0;
            offset = 0;
        }
    }
    if (base!=ConstString::npos) {
        c = Contact::byCarrier(str.substr(0,base));
        start = base+offset;
        // check if we have a direct machine:NNN syntax
        ConstString::size_type colon = ConstString::npos;
        int mode = 0;
        int nums = 0;
        ConstString::size_type i;
        for (i=start+1; i<str.length(); i++) {
            char ch = str[i];
            if (ch==':') {
                if (mode == 0) {
                    colon = i;
                    mode = 1;
                    continue;
                } else {
                    mode = -1;
                    break;
                }
            }
            if (ch=='/') {
                break;
            }
            if (mode==1) {
                if (ch>='0'&&ch<='9') {
                    nums++;
                    continue;
                } else {
                    mode = -1;
                    break;
                }
            }
        }
        //printf("Mode %d nums %d\n", mode, nums);
        if (mode==1 && nums>=1) {
            // yes, machine:nnn
            ConstString machine = str.substr(start+1,colon-start-1);
            ConstString portnum = str.substr(colon+1, nums);
            c = c.addSocket(c.getCarrier()==""?"tcp":c.getCarrier().c_str(),
                            machine,
                            atoi(portnum.c_str()));
            start = i;
        } /* else if (mode==0) {
            ConstString machine = str.substr(start+1,i-start-1);
            c = c.addSocket(c.getCarrier().c_str(),
                            machine,
                            -1);
            start = i;
            }*/
    }
    ConstString rname = str.substr(start);
    if (rname!="/") {
        c = c.addName(rname.c_str());
    }
    return c;
}



bool Contact::isValid() const {
    Address& addr = HELPER(implementation);
    return addr.getPort()>=0;
}


Contact Contact::byConfig(Searchable& config) {
    Contact result;
    Address& addr = HELPER(result.implementation);
    int port = config.check("port_number",Value(-1)).asInt();
    String name = config.check("ip",Value("")).asString().c_str();
    String regName = config.check("name",Value("")).asString().c_str();
    String carrier = config.check("carrier",Value("tcp")).asString().c_str();
    addr = Address(name,port,carrier,regName);
    return result;
}


ConstString Contact::convertHostToIp(const char *name) {
#ifdef YARP_HAS_ACE
    ACE_INET_Addr addr((u_short)0,name);
    char ipstr[256];
    addr.get_host_addr(ipstr,sizeof(ipstr));

#else
	char ipstr[INET6_ADDRSTRLEN];
	int status;
	struct addrinfo hints, *res, *p;

	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

	if ((status = getaddrinfo(name, "http", &hints, &res)) != 0) {
	    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
	    exit(1);
	}

    for(p = res;p != NULL; p = p->ai_next) {
        void *addr;

        if (p->ai_family == AF_INET) { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
        } else { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
        }

        // convert the IP to a string and print it:
        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);

        //printf("DEBUG: Host %s becomes ip address %s", name, ipstr);
    }
#endif

    if (NameConfig::isLocalName(ipstr)) {
        return NameConfig::getHostName().c_str();
    }
    return ipstr;
}


