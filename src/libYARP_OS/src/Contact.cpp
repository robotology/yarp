// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia, Anne van Rossum
 * Authors: Paul Fitzpatrick, Anne van Rossum
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/Contact.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/NetType.h>
#include <yarp/os/impl/NameConfig.h>
#include <yarp/os/Value.h>
#include <yarp/os/impl/PlatformStdlib.h>

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


Contact::Contact() {
    port = -1;
    timeout = -1;
}

Contact::Contact(const ConstString& hostName, int port) {
    this->hostName = hostName;
    this->port = port;
    timeout = -1;
}

Contact::~Contact() {
}


Contact::Contact(const Contact& alt) {
    regName = alt.regName;
    hostName = alt.hostName;
    carrier = alt.carrier;
    flavor = alt.flavor;
    port = alt.port;
    timeout = alt.timeout;
}


const Contact& Contact::operator = (const Contact& alt) {
    regName = alt.regName;
    hostName = alt.hostName;
    carrier = alt.carrier;
    flavor = alt.flavor;
    port = alt.port;
    timeout = alt.timeout;
    return *this;
}


Contact Contact::empty() {
    return Contact();
}


Contact Contact::invalid() {
    return Contact::bySocket("","",-1);
}


Contact Contact::byName(const ConstString& name) {
    Contact result;
    result.regName = name;
    return result;
}


Contact Contact::byCarrier(const ConstString& carrier) {
    Contact result;
    result.carrier = carrier;
    return result;
}

Contact Contact::addCarrier(const ConstString& carrier) const {
    Contact result(*this);
    result.carrier = carrier;
    return result;
}


Contact Contact::addHost(const ConstString& host) const {
    Contact result(*this);
    result.hostName = host;
    return result;
}


Contact Contact::bySocket(const ConstString& carrier, 
                          const ConstString& machineName,
                          int portNumber) {
    Contact result;
    result.carrier = carrier;
    result.hostName = machineName;
    result.port = portNumber;
    return result;
}


Contact Contact::addSocket(const ConstString& carrier, 
                           const ConstString& machineName,
                           int portNumber) const {
    Contact result(*this);
    result.carrier = carrier;
    result.hostName = machineName;
    result.port = portNumber;
    return result;
}

Contact Contact::addName(const ConstString& name) const {
    Contact result(*this);
    result.regName = name;
    return result;
}

Contact Contact::addNested(const NestedContact& nc) const {
    Contact result(*this);
    result.setNested(nc);
    return result;
}

ConstString Contact::getName() const {
    ConstString name = regName;
    if (regName == "") {
        if (hostName!="" && port>=0) {
            name = ConstString("/") + hostName + ":" + 
                NetType::toString(port);
        }
    }
    return name;
}


ConstString Contact::getHost() const {
    return hostName;
}


ConstString Contact::getCarrier() const {
    return carrier;
}


int Contact::getPort() const {
    return port;
}

const NestedContact& Contact::getNested() const {
    return flavor;
}


ConstString Contact::toString() const {
    ConstString name = getName();
    if (carrier!="") {
        return carrier + ":/" + name;
    }
    return name;
}


ConstString Contact::toURI() const {
    ConstString result = "";
    if (carrier!="") {
        result += carrier;
        result += ":/";
    }
    if (hostName!="" && port>=0) {
        result += "/";
        result += hostName;
        result += ":";
        result += NetType::toString(port);
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
        if (str.length()>0 && str[0] == '/') {
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
        if (mode==1 && nums>=1) {
            // yes, machine:nnn
            ConstString machine = str.substr(start+1,colon-start-1);
            ConstString portnum = str.substr(colon+1, nums);
            c = c.addSocket(c.getCarrier()==""?"tcp":c.getCarrier().c_str(),
                            machine,
                            atoi(portnum.c_str()));
            start = i;
        }
    }
    ConstString rname = str.substr(start);
    if (rname!="/") {
        c = c.addName(rname.c_str());
    }
    return c;
}



bool Contact::isValid() const {
    return port>=0;
}


Contact Contact::byConfig(Searchable& config) {
    Contact result;
    result.port = config.check("port_number",Value(-1)).asInt();
    result.hostName = config.check("ip",Value("")).asString().c_str();
    result.regName = config.check("name",Value("")).asString().c_str();
    result.carrier = config.check("carrier",Value("tcp")).asString().c_str();
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
    }
#endif

    if (NameConfig::isLocalName(ipstr)) {
        return NameConfig::getHostName();
    }
    return ipstr;
}


bool Contact::hasTimeout() const {
    return timeout >= 0;
}

void Contact::setTimeout(float timeout) {
    this->timeout = timeout;
}

void Contact::setNested(const yarp::os::NestedContact& flavor) {
    this->flavor = flavor;
}

float Contact::getTimeout() const {
    return timeout;
}

ConstString Contact::getRegName() const {
    return regName;
}
