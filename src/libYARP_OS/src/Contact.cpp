/*
 * Copyright (C) 2006, 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia, Anne van Rossum
 * Copyright (C) 2016 iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Paul Fitzpatrick <paulfitz@alum.mit.edu>
 *          Anne van Rossum <anne@almende.com>
 *          Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <yarp/os/Contact.h>
#include <yarp/os/NetType.h>
#include <yarp/os/Searchable.h>
#include <yarp/os/Value.h>
#include <yarp/os/impl/NameConfig.h>
#include <yarp/os/impl/PlatformStdlib.h>


#ifdef YARP_HAS_ACE
#  include <ace/INET_Addr.h>
#else
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <netdb.h>
#  include <arpa/inet.h>
#  include <cstdio>
#endif


using yarp::os::Contact;
using yarp::os::ConstString;
using yarp::os::NetType;
using yarp::os::NestedContact;
using yarp::os::Searchable;
using yarp::os::Value;
using yarp::os::impl::NameConfig;


#ifndef DOXYGEN_SHOULD_SKIP_THIS

class Contact::Private
{
public:
    Private(const ConstString& regName,
            const ConstString& carrier,
            const ConstString& hostname,
            int port) :
        regName(regName),
        carrier(carrier),
        hostname(hostname),
        port(port),
        timeout(-1)
    {
    }

    ConstString regName;
    ConstString carrier;
    ConstString hostname;
    NestedContact nestedContact;

    int port;
    float timeout;
};

#endif // DOXYGEN_SHOULD_SKIP_THIS


Contact::Contact(const ConstString& hostname,
                 int port) :
        mPriv(new Private(ConstString(), ConstString(), hostname, port))
{
}

Contact::Contact(const ConstString& carrier,
                 const ConstString& hostname,
                 int port) :
        mPriv(new Private(ConstString(), carrier, hostname, port))
{
}

Contact::Contact(const ConstString& name,
                 const ConstString& carrier,
                 const ConstString& hostname,
                 int port) :
        mPriv(new Private(name, carrier, hostname, port))
{
}

Contact::Contact(const Contact& rhs) :
        mPriv(new Private(*(rhs.mPriv)))
{
}

#if YARP_COMPILER_CXX_RVALUE_REFERENCES
Contact::Contact(Contact&& rhs) :
        mPriv(new Private(std::move(*(rhs.mPriv))))
{
}
#endif // YARP_COMPILER_CXX_RVALUE_REFERENCES

Contact::~Contact()
{
    delete mPriv;
}

Contact& Contact::operator=(const Contact& rhs)
{
    if(&rhs != this) {
        *mPriv = *(rhs.mPriv);
    }
    return *this;
}

#if YARP_COMPILER_CXX_RVALUE_REFERENCES
Contact& Contact::operator=(Contact&& rhs)
{
    if(&rhs != this) {
        std::swap(mPriv, rhs.mPriv);
    }
    return *this;
}
#endif // YARP_COMPILER_CXX_RVALUE_REFERENCES


Contact Contact::fromConfig(const Searchable& config)
{
    Contact result;
    result.mPriv->port = config.check("port_number",Value(-1)).asInt();
    result.mPriv->hostname = config.check("ip",Value("")).asString().c_str();
    result.mPriv->regName = config.check("name",Value("")).asString().c_str();
    result.mPriv->carrier = config.check("carrier",Value("tcp")).asString().c_str();
    return result;
}

Contact Contact::fromString(const ConstString& txt)
{
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
        c.mPriv->carrier = str.substr(0,base);
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
            if (c.mPriv->carrier.empty()) {
                c.mPriv->carrier = "tcp";
            }
            c.mPriv->hostname = str.substr(start+1,colon-start-1);
            c.mPriv->port = atoi(str.substr(colon+1, nums).c_str());
            start = i;
        }
    }
    ConstString rname = str.substr(start);
    if (rname!="/") {
        c.mPriv->regName = rname.c_str();
    }
    return c;
}



ConstString Contact::getName() const
{
    if (!mPriv->regName.empty()) {
        return mPriv->regName;
    }
    if (mPriv->hostname!="" && mPriv->port>=0) {
        ConstString name = ConstString("/") + mPriv->hostname + ":" +
            NetType::toString(mPriv->port);
        return name;
    }
    return ConstString();
}

ConstString Contact::getRegName() const
{
    return mPriv->regName;
}

void Contact::setName(const ConstString& name)
{
    mPriv->regName = name;
}



ConstString Contact::getHost() const
{
    return mPriv->hostname;
}

void Contact::setHost(const ConstString& hostname)
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



ConstString Contact::getCarrier() const
{
    return mPriv->carrier;
}

void Contact::setCarrier(const ConstString& carrier)
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



void Contact::setSocket(const ConstString& carrier,
                        const ConstString& hostname,
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

ConstString Contact::toString() const
{
    ConstString name = getName();
    if (mPriv->carrier!="") {
        return mPriv->carrier + ":/" + name;
    }
    return name;
}


ConstString Contact::toURI(bool includeCarrier) const
{
    ConstString result = "";
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


ConstString Contact::convertHostToIp(const char *name)
{
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

    for(p = res; p != YARP_NULLPTR; p = p->ai_next) {
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


#ifndef YARP_NO_DEPRECATED // since YARP 2.3.68

Contact Contact::empty() {
    return Contact();
}

Contact Contact::invalid() {
    return Contact("","",-1);
}

Contact Contact::byName(const ConstString& name)
{
    Contact result;
    result.mPriv->regName = name;
    return result;
}

Contact Contact::byCarrier(const ConstString& carrier)
{
    Contact result;
    result.mPriv->carrier = carrier;
    return result;
}

Contact Contact::bySocket(const ConstString& carrier,
                          const ConstString& hostname,
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

Contact Contact::addCarrier(const ConstString& carrier) const
{
    Contact result(*this);
    result.mPriv->carrier = carrier;
    return result;
}


Contact Contact::addHost(const ConstString& hostname) const
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

Contact Contact::addSocket(const ConstString& carrier,
                           const ConstString& hostname,
                           int port) const
{
    Contact result(*this);
    result.mPriv->carrier = carrier;
    result.mPriv->hostname = hostname;
    result.mPriv->port = port;
    return result;
}

Contact Contact::addName(const ConstString& name) const
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
