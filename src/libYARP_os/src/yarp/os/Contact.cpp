/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-FileCopyrightText: 2006, 2011 Anne van Rossum <anne@almende.com>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Contact.h>
#include <yarp/os/NetType.h>
#include <yarp/os/Searchable.h>
#include <yarp/os/Value.h>
#include <yarp/os/impl/LogComponent.h>
#include <yarp/os/impl/NameConfig.h>
#include <yarp/os/impl/PlatformNetdb.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <utility>

#if defined(YARP_HAS_ACE)
#    include <ace/INET_Addr.h>
// In one the ACE headers there is a definition of "main" for WIN32
#    ifdef main
#        undef main
#    endif
#else
#    include <arpa/inet.h>
#    include <sys/socket.h>
#    include <sys/types.h>
#endif


using yarp::os::Contact;
using yarp::os::NestedContact;
using yarp::os::NetType;
using yarp::os::Searchable;
using yarp::os::Value;
using yarp::os::impl::NameConfig;

#if !defined(YARP_HAS_ACE)
namespace {
YARP_OS_LOG_COMPONENT(CONTACT, "yarp.os.Contact" )
}
#endif

#ifndef DOXYGEN_SHOULD_SKIP_THIS

class Contact::Private
{
public:
    Private(std::string regName,
            std::string carrier,
            std::string hostname,
            int port) :
            regName(std::move(regName)),
            carrier(std::move(carrier)),
            hostname(std::move(hostname)),
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

Contact::Contact(Contact&& rhs) noexcept :
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

Contact& Contact::operator=(Contact&& rhs) noexcept
{
    if (&rhs != this) {
        std::swap(mPriv, rhs.mPriv);
    }
    return *this;
}

Contact Contact::fromConfig(const Searchable& config)
{
    Contact result;
    result.mPriv->port = config.check("port_number", Value(-1)).asInt32();
    result.mPriv->hostname = config.check("ip", Value("")).asString();
    result.mPriv->regName = config.check("name", Value("")).asString();
    result.mPriv->carrier = config.check("carrier", Value("tcp")).asString();
    return result;
}

Contact Contact::fromString(const std::string& txt)
{
    std::string str(txt);
    Contact c;
    std::string::size_type start = 0;
    std::string::size_type base = str.find("://");
    std::string::size_type offset = 2;
    if (base == std::string::npos) {
        base = str.find(":/");
        offset = 1;
    }
    if (base == std::string::npos) {
        if (str.length() > 0 && str[0] == '/') {
            base = 0;
            offset = 0;
        }
    }
    if (base != std::string::npos) {
        c.mPriv->carrier = str.substr(0, base);
        start = base + offset;
        // check if we have a direct machine:NNN syntax
        std::string::size_type colon = std::string::npos;
        int mode = 0;
        int nums = 0;
        std::string::size_type i;
        for (i = start + 1; i < str.length(); i++) {
            char ch = str[i];
            if (ch == ':') {
                if (mode == 0) {
                    colon = i;
                    mode = 1;
                    continue;
                }
                mode = -1;
                break;
            }
            if (ch == '/') {
                break;
            }
            if (mode == 1) {
                if (ch >= '0' && ch <= '9') {
                    nums++;
                    continue;
                }
                mode = -1;
                break;
            }
        }
        if (mode == 1 && nums >= 1) {
            // yes, machine:nnn
            if (c.mPriv->carrier.empty()) {
                c.mPriv->carrier = "tcp";
            }
            c.mPriv->hostname = str.substr(start + 1, colon - start - 1);
            c.mPriv->port = atoi(str.substr(colon + 1, nums).c_str());
            start = i;
        }
    }
    std::string rname = str.substr(start);
    if (rname != "/") {
        c.mPriv->regName = rname;
    }
    return c;
}


std::string Contact::getName() const
{
    if (!mPriv->regName.empty()) {
        return mPriv->regName;
    }
    if (!mPriv->hostname.empty() && mPriv->port >= 0) {
        std::string name = std::string("/") + mPriv->hostname + ":" + yarp::conf::numeric::to_string(mPriv->port);
        return name;
    }
    return {};
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
    mPriv->port = port;
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
    return mPriv->port >= 0;
}

std::string Contact::toString() const
{
    std::string name = getName();
    if (!mPriv->carrier.empty()) {
        return mPriv->carrier + ":/" + name;
    }
    return name;
}


std::string Contact::toURI(bool includeCarrier) const
{
    std::string result;
    if (includeCarrier && !mPriv->carrier.empty()) {
        result += mPriv->carrier;
        result += ":/";
    }
    if (!mPriv->hostname.empty() && mPriv->port >= 0) {
        result += "/";
        result += mPriv->hostname;
        result += ":";
        result += yarp::conf::numeric::to_string(mPriv->port);
        result += "/";
    }
    return result;
}


std::string Contact::convertHostToIp(const char* name)
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
        yCError(CONTACT, "getaddrinfo error: %s\n", yarp::os::impl::gai_strerror(status));
        std::exit(1);
    }

    for (p = res; p != nullptr; p = p->ai_next) {
        void* addr;

        if (p->ai_family == AF_INET) { // IPv4
            auto* ipv4 = reinterpret_cast<struct sockaddr_in*>(p->ai_addr);
            addr = &(ipv4->sin_addr);
        } else { // IPv6
            auto* ipv6 = reinterpret_cast<struct sockaddr_in6*>(p->ai_addr);
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
