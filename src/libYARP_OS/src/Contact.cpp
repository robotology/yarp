/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * Copyright (C) 2006, 2011 Anne van Rossum <anne@almende.com>
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Contact.h>

#include <yarp/os/NetType.h>
#include <yarp/os/Searchable.h>
#include <yarp/os/Value.h>

#include <yarp/os/impl/PlatformNetdb.h>
#include <yarp/os/impl/NameConfig.h>

#include <cstdlib>
#include <cstdio>
#include <cstring>

#if defined(YARP_HAS_ACE)
# include <ace/INET_Addr.h>
#else
# include <sys/types.h>
# include <sys/socket.h>
# include <arpa/inet.h>
#endif


using yarp::os::Contact;
using std::string;
using yarp::os::NetType;
using yarp::os::NestedContact;
using yarp::os::Searchable;
using yarp::os::Value;
using yarp::os::impl::NameConfig;


#ifndef DOXYGEN_SHOULD_SKIP_THIS

class Contact::Private
{
public:
    Private(const std::string& regName,
            const std::string& carrier,
            const std::string& hostname,
            int port) :
        regName(regName),
        carrier(carrier),
        hostname(hostname),
        port(port),
        timeout(-1)
    {
    }

    std::string regName;
    std::string carrier;
    std::string hostname;
    NestedContact nestedContact;

    int port;
    float timeout;
};

#endif // DOXYGEN_SHOULD_SKIP_THIS


Contact::Contact(const std::string& hostname,
                 int port) :
        mPriv(new Private(std::string(), std::string(), hostname, port))
{
}

Contact::Contact(const std::string& carrier,
                 const std::string& hostname,
                 int port) :
        mPriv(new Private(std::string(), carrier, hostname, port))
{
}

Contact::Contact(const std::string& name,
                 const std::string& carrier,
                 const std::string& hostname,
                 int port) :
        mPriv(new Private(name, carrier, hostname, port))
{
}

Contact::Contact(const Contact& rhs) :
        mPriv(new Private(*(rhs.mPriv)))
{
}

Contact::Contact(Contact&& rhs) :
        mPriv(rhs.mPriv)
{
    rhs.mPriv = nullptr;
}

Contact::~Contact()
{
    delete mPriv;
}

Contact& Contact::operator=(const Contact& rhs)
{
    if (&rhs != this) {
        *mPriv = *(rhs.mPriv);
    }
    return *this;
}

Contact& Contact::operator=(Contact&& rhs)
{
    if (&rhs != this) {
        std::swap(mPriv, rhs.mPriv);
    }
    return *this;
}

Contact Contact::fromConfig(const Searchable& config)
{
    Contact result;
    result.mPriv->port = config.check("port_number", Value(-1)).asInt();
    result.mPriv->hostname = config.check("ip", Value("")).asString().c_str();
    result.mPriv->regName = config.check("name", Value("")).asString().c_str();
    result.mPriv->carrier = config.check("carrier", Value("tcp")).asString().c_str();
    return result;
}

Contact Contact::fromString(const std::string& txt)
{
    std::string str(txt);
    Contact c;
    std::string::size_type start = 0;
    std::string::size_type base = str.find("://");
    std::string::size_type offset = 2;
    if (base==std::string::npos) {
        base = str.find(":/");
        offset = 1;
    }
    if (base==std::string::npos) {
        if (str.length()>0 && str[0] == '/') {
            base = 0;
            offset = 0;
        }
    }
    if (base!=std::string::npos) {
        c.mPriv->carrier = str.substr(0, base);
        start = base+offset;
        // check if we have a direct machine:NNN syntax
        std::string::size_type colon = std::string::npos;
        int mode = 0;
        int nums = 0;
        std::string::size_type i;
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
            if (c.mPriv->carrier.empty()) {
                c.mPriv->carrier = "tcp";
            }
            c.mPriv->hostname = str.substr(start+1, colon-start-1);
            c.mPriv->port = atoi(str.substr(colon+1, nums).c_str());
            start = i;
        }
    }
    std::string rname = str.substr(start);
    if (rname!="/") {
        c.mPriv->regName = rname.c_str();
    }
    return c;
}



std::string Contact::getName() const
{
    if (!mPriv->regName.empty()) {
        return mPriv->regName;
    }
    if (mPriv->hostname!="" && mPriv->port>=0) {
        std::string name = std::string("/") + mPriv->hostname + ":" +
            NetType::toString(mPriv->port);
        return name;
    }
    return std::string();
}

std::string Contact::getRegName() const
{
    return mPriv->regName;
}

void Contact::setName(const std::string& name)
{
    mPriv->regName = name;
}



std::string Contact::getHost() const
{
    return mPriv->hostname;
}

void Contact::setHost(const std::string& hostname)
{
    this->mPriv->hostname = hostname;
}



int Contact::getPort() const
{
    return mPriv->port;
}

void Contact::setPort(int port)
{
    this->mPriv->port = port;
}



std::string Contact::getCarrier() const
{
    return mPriv->carrier;
}

void Contact::setCarrier(const std::string& carrier)
{
    mPriv->carrier = carrier;
}



const NestedContact& Contact::getNested() const
{
    return mPriv->nestedContact;
}

void Contact::setNestedContact(const yarp::os::NestedContact& nestedContact)
{
    this->mPriv->nestedContact = nestedContact;
}



bool Contact::hasTimeout() const
{
    return mPriv->timeout >= 0;
}

float Contact::getTimeout() const
{
    return mPriv->timeout;
}

void Contact::setTimeout(float timeout)
{
    this->mPriv->timeout = timeout;
}



void Contact::setSocket(const std::string& carrier,
                        const std::string& hostname,
                        int port)
{
    mPriv->carrier = carrier;
    mPriv->hostname = hostname;
    mPriv->port = port;
}



bool Contact::isValid() const
{
    return mPriv->port>=0;
}

std::string Contact::toString() const
{
    std::string name = getName();
    if (mPriv->carrier!="") {
        return mPriv->carrier + ":/" + name;
    }
    return name;
}


std::string Contact::toURI(bool includeCarrier) const
{
    std::string result = "";
    if (includeCarrier && mPriv->carrier!="") {
        result += mPriv->carrier;
        result += ":/";
    }
    if (mPriv->hostname!="" && mPriv->port>=0) {
        result += "/";
        result += mPriv->hostname;
        result += ":";
        result += NetType::toString(mPriv->port);
        result += "/";
    }
    return result;
}


std::string Contact::convertHostToIp(const char *name)
{
#if defined(YARP_HAS_ACE)
    ACE_INET_Addr addr((u_short)0, name);
    char ipstr[256];
    addr.get_host_addr(ipstr, sizeof(ipstr));

#else
    char ipstr[INET6_ADDRSTRLEN];
    int status;
    struct addrinfo hints, *res, *p;

    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

    if ((status = yarp::os::impl::getaddrinfo(name, "http", &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", yarp::os::impl::gai_strerror(status));
        std::exit(1);
    }

    for(p = res; p != nullptr; p = p->ai_next) {
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
    yarp::os::impl::freeaddrinfo(res);
#endif

    if (NameConfig::isLocalName(ipstr)) {
        return NameConfig::getHostName();
    }
    return ipstr;
}


#ifndef YARP_NO_DEPRECATED // since YARP 2.3.68

Contact Contact::empty() {
    return Contact();
}

Contact Contact::invalid() {
    return Contact("", "", -1);
}

Contact Contact::byName(const std::string& name)
{
    Contact result;
    result.mPriv->regName = name;
    return result;
}

Contact Contact::byCarrier(const std::string& carrier)
{
    Contact result;
    result.mPriv->carrier = carrier;
    return result;
}

Contact Contact::bySocket(const std::string& carrier,
                          const std::string& hostname,
                          int port)
{
    Contact result;
    result.mPriv->carrier = carrier;
    result.mPriv->hostname = hostname;
    result.mPriv->port = port;
    return result;
}

Contact Contact::byConfig(Searchable& config) {
    return fromConfig(config);
}

Contact Contact::addCarrier(const std::string& carrier) const
{
    Contact result(*this);
    result.mPriv->carrier = carrier;
    return result;
}


Contact Contact::addHost(const std::string& hostname) const
{
    Contact result(*this);
    result.mPriv->hostname = hostname;
    return result;
}


Contact Contact::addPort(int port) const
{
    Contact result(*this);
    result.mPriv->port = port;
    return result;
}

Contact Contact::addSocket(const std::string& carrier,
                           const std::string& hostname,
                           int port) const
{
    Contact result(*this);
    result.mPriv->carrier = carrier;
    result.mPriv->hostname = hostname;
    result.mPriv->port = port;
    return result;
}

Contact Contact::addName(const std::string& name) const
{
    Contact result(*this);
    result.mPriv->regName = name;
    return result;
}

Contact Contact::addNested(const NestedContact& nestedContact) const
{
    Contact result(*this);
    result.setNestedContact(nestedContact);
    return result;
}

#endif // YARP_NO_DEPRECATED
