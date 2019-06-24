/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006, 2011 Anne van Rossum <anne@almende.com>
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */


#include <yarp/os/impl/NameConfig.h>

#include <yarp/conf/system.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/NetType.h>
#include <yarp/os/Network.h>
#include <yarp/os/Os.h>
#include <yarp/os/Property.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/PlatformIfaddrs.h>
#include <yarp/os/impl/PlatformLimits.h>
#include <yarp/os/impl/PlatformNetdb.h>
#include <yarp/os/impl/PlatformSysStat.h>
#include <yarp/os/impl/PlatformUnistd.h>
#include <yarp/os/impl/SplitString.h>

#include <cstdio>
#include <cstdlib>

#if defined(YARP_HAS_ACE)
#    include <ace/INET_Addr.h>
#    include <ace/Sock_Connect.h>
// In one the ACE headers there is a definition of "main" for WIN32
#    ifdef main
#        undef main
#    endif
#elif defined(__unix__)
#    include <arpa/inet.h>
#    include <cstring>
#    include <sys/socket.h>
#    include <unistd.h>
#endif

using namespace yarp::os::impl;
using namespace yarp::os;

#define CONF_FILENAME YARP_CONFIG_FILENAME

bool NameConfig::fromString(const std::string& txt)
{
    address = Contact();
    SplitString ss(txt.c_str());
    if (ss.size() >= 1) {
        if (ss.get(0)[0] == '[') {
            // use Property format
            Property config;
            config.fromConfig(txt.c_str());

            Bottle& b = config.findGroup("name");
            if (b.isNull()) {
                fprintf(stderr, "Cannot find yarp group in config file\n");
                std::exit(1);
            }
            address = Contact(b.find("host").asString(),
                              b.find("port").asInt32());
            mode = b.check("mode", Value("yarp")).asString();
            return (address.getPort() != 0);
        }
    }

    if (ss.size() >= 2) {
        address = Contact(ss.get(0), NetType::toInt(ss.get(1)));
        if (ss.size() >= 3) {
            mode = ss.get(2);
        } else {
            mode = "yarp";
        }
        if (mode == "ros") {
            address.setCarrier("xmlrpc");
        }
        return true;
    }
    return false;
}

std::string NameConfig::expandFilename(const char* fname)
{
    std::string root = ResourceFinder::getConfigHome();
    std::string conf;
    if (!root.empty()) {
        conf = root + NetworkBase::getDirectorySeparator() + fname;
    } else {
        conf = fname;
    }

    YARP_DEBUG(Logger::get(), std::string("Configuration file: ") + conf);
    return conf;
}

std::string NameConfig::getSafeString(const std::string& txt)
{
    std::string result = txt;
    for (char& i : result) {
        char ch = i;
        if (!((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))) {
            i = '_';
        }
    }
    return result;
}

std::string NameConfig::getConfigFileName(const char* stem, const char* ns)
{
    std::string fname = (stem != nullptr) ? stem : CONF_FILENAME;
    if (stem == nullptr) {
        std::string space;
        if (ns != nullptr) {
            space = ns;
        } else {
            space = getNamespace();
        }
        if (space != "/root") {
            // for non-default namespace, need a separate cache file
            std::string base = getSafeString(space);
            base += ".conf";
            fname = base;
        }
    }
    return expandFilename(fname.c_str());
}

std::string NameConfig::readConfig(const std::string& fileName)
{
    char buf[25600];
    FILE* fin = fopen(fileName.c_str(), "r");
    if (fin == nullptr) {
        return {};
    }
    std::string result;
    while (fgets(buf, sizeof(buf) - 1, fin) != nullptr) {
        result += buf;
    }
    fclose(fin);
    fin = nullptr;
    return result;
}


bool NameConfig::fromFile(const char* ns)
{
    std::string fname = getConfigFileName(nullptr, ns);
    if (!fname.empty()) {
        std::string txt = readConfig(fname);
        if (!txt.empty()) {
            return fromString(txt);
        }
    }
    return false;
}


bool NameConfig::toFile(bool clean)
{
    std::string fname = getConfigFileName();
    if (!fname.empty()) {
        std::string txt;
        if (!clean) {
            std::string m = (!mode.empty()) ? mode : "yarp";
            txt += address.getHost() + " " + NetType::toString(address.getPort()) + " " + m + "\n";
        }
        return writeConfig(fname, txt);
    }
    return false;
}


Contact NameConfig::getAddress()
{
    return address;
}


bool NameConfig::writeConfig(const std::string& fileName, const std::string& text)
{
    if (yarp::os::mkdir_p(fileName.c_str(), 1) != 0) {
        return false;
    }
    FILE* fout = fopen(fileName.c_str(), "w");
    if (fout == nullptr) {
        return false;
    }
    fprintf(fout, "%s", text.c_str());
    fclose(fout);
    fout = nullptr;
    return true;
}


std::string NameConfig::getHostName(bool prefer_loopback, const std::string& seed)
{
    // try to pick a good host identifier

    std::string result = "127.0.0.1";
    bool loopback = true;
    bool found = false;

    // Pick an IPv4 address.
    // Prefer non-local addresses, then seed, then shorter addresses.
    // Avoid IPv6.
#ifdef YARP_HAS_ACE
    ACE_INET_Addr* ips = nullptr;
    size_t count = 0;
    char hostAddress[256];
    if (ACE::get_ip_interfaces(count, ips) >= 0) {
        for (size_t i = 0; i < count; i++) {
            std::string ip = ips[i].get_host_addr(hostAddress, 256);
#else
    int family, s;
    char hostname[NI_MAXHOST];
    std::string ip;
    struct ifaddrs *ifaddr, *ifa;
    if (yarp::os::impl::getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs in getIps");
        std::exit(EXIT_FAILURE);
    }
    for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == nullptr) {
            continue;
        }
        family = ifa->ifa_addr->sa_family;
        if (family == AF_INET || family == AF_INET6) {
            s = yarp::os::impl::getnameinfo(ifa->ifa_addr,
                                            (family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6),
                                            hostname,
                                            NI_MAXHOST,
                                            nullptr,
                                            0,
                                            NI_NUMERICHOST);
            if (s != 0) {
                printf("getnameinfo() failed: %s\n", yarp::os::impl::gai_strerror(s));
                std::exit(EXIT_FAILURE);
            }
            ip = std::string(hostname);
#endif

            YARP_DEBUG(Logger::get(), std::string("scanning network interface ") + ip);

            if (ip.find(':') != std::string::npos) {
                continue;
            }

#if defined YARP_HAS_ACE
            bool would_be_loopback = ips[i].is_loopback();
#else
            bool would_be_loopback = (ip == "127.0.0.1" || ip == "127.1.0.1" || ip == "127.0.1.1");
#endif

            // If we haven't any interface yet, take this one
            if (!found) {
                result = ip;
                loopback = would_be_loopback;
                found = true;
                continue;
            }

            // We have an interface

            // If this isn't the right kind of interface, skip it
            if (would_be_loopback != prefer_loopback) {
                continue;
            }

            // This is the right kind of interface

            // If we haven't the right kind of interface yet, take it
            if (prefer_loopback != loopback) {
                result = ip;
                loopback = would_be_loopback;
                continue;
            }

            // If it matches the seed interface, take it
            if (ip == seed) {
                result = ip;
                loopback = would_be_loopback;
                continue;
            }

            // If it is shorter, and what we have isn't the seed, take it
            if (ip.length() < result.length() && result != seed) {
                result = ip;
                loopback = would_be_loopback;
                continue;
            }
        }
    }
#ifdef YARP_HAS_ACE
    delete[] ips;
#else
    freeifaddrs(ifaddr);
#endif

    return result;
}


bool NameConfig::isLocalName(const std::string& name)
{
    bool result = false;

#if defined(YARP_HAS_ACE)
    ACE_INET_Addr* ips = nullptr;
    size_t count = 0;
    if (ACE::get_ip_interfaces(count, ips) >= 0) {
        for (size_t i = 0; i < count; i++) {
            std::string ip = ips[i].get_host_addr();
            if (ip == name) {
                result = true;
                break;
            }
        }
        delete[] ips;
    }
#elif defined(__unix__)
    /**
     * If this does not work properly, use a more sophisticated way
     * instead of just gethostname.
     */
    char hostname[HOST_NAME_MAX];
    yarp::os::impl::gethostname(hostname, HOST_NAME_MAX);
    if (strcmp(hostname, name.c_str()) == 0) {
        result = true;
    }
    if (!result) {
        Bottle lst = getIpsAsBottle();
        for (size_t i = 0; i < lst.size(); i++) {
            if (lst.get(i).asString() == name) {
                result = true;
                break;
            }
        }
    }
#endif

    // just in case
    if (name == "localhost" || name == "127.0.0.1") {
        result = true;
    }

    return result;
}

yarp::os::Bottle NameConfig::getIpsAsBottle()
{
    yarp::os::Bottle result;

#if defined(YARP_HAS_ACE)
    ACE_INET_Addr* ips = nullptr;
    size_t count = 0;
    if (ACE::get_ip_interfaces(count, ips) >= 0) {
        for (size_t i = 0; i < count; i++) {
            std::string ip = ips[i].get_host_addr();
            result.addString(ip.c_str());
        }
        delete[] ips;
    }
#else
    int family, s;
    char host[NI_MAXHOST];
    struct ifaddrs *ifaddr, *ifa;
    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs in getIpsAsBottle");
        std::exit(EXIT_FAILURE);
    }
    for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == nullptr) {
            continue;
        }
        family = ifa->ifa_addr->sa_family;
        if (family == AF_INET || family == AF_INET6) {
            s = yarp::os::impl::getnameinfo(ifa->ifa_addr,
                                            (family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6),
                                            host,
                                            NI_MAXHOST,
                                            nullptr,
                                            0,
                                            NI_NUMERICHOST);
            if (s != 0) {
                printf("getnameinfo() failed: %s\n", yarp::os::impl::gai_strerror(s));
                std::exit(EXIT_FAILURE);
            }
            result.addString(host);
        }
    }
    freeifaddrs(ifaddr);
#endif

    return result;
}


std::string NameConfig::getIps()
{
    yarp::os::Bottle bot = getIpsAsBottle();
    std::string result;
    for (size_t i = 0; i < bot.size(); i++) {
        std::string ip = bot.get(i).asString();
        if (i > 0) {
            result += " ";
        }
        result += ip;
    }
    return result;
}


void NameConfig::setAddress(const Contact& address)
{
    this->address = address;
}


void NameConfig::setNamespace(const std::string& ns)
{
    space = ns;
}

std::string NameConfig::getNamespace(bool refresh)
{
    if (space.empty() || refresh) {
        std::string senv = NetworkBase::getEnvironment("YARP_NAMESPACE");
        if (!senv.empty()) {
            spaces.fromString(senv);
        } else {
            std::string fname = getConfigFileName(YARP_CONFIG_NAMESPACE_FILENAME);
            spaces.fromString(readConfig(fname));
        }
        space = spaces.get(0).asString();
        if (space.empty()) {
            space = "/root";
        }
        if (spaces.size() == 0) {
            spaces.addString("/root");
        }
    }
    return space;
}

Bottle NameConfig::getNamespaces(bool refresh)
{
    getNamespace(refresh);
    return spaces;
}
