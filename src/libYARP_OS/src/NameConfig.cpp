/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006, 2011 Anne van Rossum <anne@almende.com>
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/impl/NameConfig.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/NetType.h>
#include <yarp/os/Network.h>
#include <yarp/os/Os.h>
#include <yarp/os/Property.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/conf/system.h>

#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/PlatformSysStat.h>
#include <yarp/os/impl/PlatformNetdb.h>
#include <yarp/os/impl/PlatformUnistd.h>
#include <yarp/os/impl/PlatformLimits.h>
#include <yarp/os/impl/PlatformIfaddrs.h>
#include <yarp/os/impl/SplitString.h>

#include <cstdlib>
#include <cstdio>

#ifdef YARP_HAS_ACE
# include <ace/INET_Addr.h>
# include <ace/Sock_Connect.h>
#else
# include <cstring>
# include <arpa/inet.h>
# include <sys/socket.h>
#endif

using namespace yarp::os::impl;
using namespace yarp::os;

#define CONF_FILENAME YARP_CONFIG_FILENAME

bool NameConfig::fromString(const ConstString& txt) {
    address = Contact();
    SplitString ss(txt.c_str());
    if (ss.size()>=1) {
        if (ss.get(0)[0]=='[') {
            // use Property format
            Property config;
            config.fromConfig(txt.c_str());

            Bottle& b = config.findGroup("name");
            if (b.isNull()) {
                fprintf(stderr, "Cannot find yarp group in config file\n");
                std::exit(1);
            }
            address = Contact(b.find("host").asString().c_str(),
                              b.find("port").asInt());
            mode = b.check("mode", Value("yarp")).asString().c_str();
            return (address.getPort()!=0);
        }
    }

    if (ss.size()>=2) {
        address = Contact(ss.get(0), NetType::toInt(ss.get(1)));
        if (ss.size()>=3) {
            mode = ss.get(2);
        } else {
            mode = "yarp";
        }
        if (mode=="ros") {
            address.setCarrier("xmlrpc");
        }
        return true;
    }
    return false;
}

ConstString NameConfig::expandFilename(const char *fname) {
#ifndef YARP_NO_DEPRECATED // Since YARP 2.3.70
    ConstString yarp_conf = NetworkBase::getEnvironment("YARP_CONF");
    if (!yarp_conf.empty()) {
        YARP_WARN(Logger::get(), "The YARP_CONF variable is deprecated and it is no longer used. "
                                 "Please check the documentation for yarp::os::ResourceFinder::getConfigHome()");
    }
#endif

    ConstString root = ResourceFinder::getConfigHome();
    ConstString conf;
    if (!root.empty()) {
        conf = root + NetworkBase::getDirectorySeparator() + fname;
    } else {
        conf = fname;
    }

    YARP_DEBUG(Logger::get(), ConstString("Configuration file: ") + conf.c_str());
    return conf.c_str();
}

ConstString NameConfig::getSafeString(const ConstString& txt) {
    ConstString result = txt;
    for (unsigned int i=0; i<result.length(); i++) {
        char ch = result[i];
        if (!((ch>='0'&&ch<='9')||(ch>='A'&&ch<='Z')||(ch>='a'&&ch<='z'))) {
            result[i] = '_';
        }
    }
    return result;
}

ConstString NameConfig::getConfigFileName(const char *stem, const char *ns) {
    ConstString fname = (stem!=nullptr)?stem:CONF_FILENAME;
    if (stem==nullptr) {
        ConstString space;
        if (ns) {
            space = ns;
        } else {
            space = getNamespace();
        }
        if (space!="/root") {
            // for non-default namespace, need a separate cache file
            ConstString base = getSafeString(space);
            base += ".conf";
            fname = base;
        }
    }
    return expandFilename(fname.c_str());
}


bool NameConfig::createPath(const ConstString& fileName, int ignoreLevel) {
    size_t index = fileName.rfind('/');
    if (index==ConstString::npos) {
        index = fileName.rfind('\\');
        if (index==ConstString::npos) {
            return false;
        }
    }
    ConstString base = fileName.substr(0, index);
    if (yarp::os::stat((char*)base.c_str())<0) {
        bool result = createPath(base, ignoreLevel-1);
        if (result==false) {
            return false;
        }
    }
    if (ignoreLevel<=0) {
        if (yarp::os::stat(fileName.c_str())<0) {
            if (yarp::os::mkdir(fileName.c_str())>=0) {
                return true;
            }
            return false;
        }
    }
    return true;
}

ConstString NameConfig::readConfig(const ConstString& fileName) {
    char buf[25600];
    FILE *fin = fopen(fileName.c_str(), "r");
    if (!fin) {
        return "";
    }
    ConstString result = "";
    while(fgets(buf, sizeof(buf)-1, fin) != nullptr) {
        result += buf;
    }
    fclose(fin);
    fin = nullptr;
    return result;
}


bool NameConfig::fromFile(const char *ns) {
    ConstString fname = getConfigFileName(nullptr, ns);
    if (fname!="") {
        ConstString txt = readConfig(fname);
        if (txt!="") {
            return fromString(txt);
        }
    }
    return false;
}


bool NameConfig::toFile(bool clean) {
    ConstString fname = getConfigFileName();
    if (fname!="") {
        ConstString txt = "";
        if (!clean) {
            ConstString m = (mode!="")?mode:"yarp";
            txt += address.getHost() + " " + NetType::toString(address.getPort()) + " " + m + "\n";
        }
        return writeConfig(fname, txt);
    }
    return false;
}


Contact NameConfig::getAddress() {
    return address;
}


bool NameConfig::writeConfig(const ConstString& fileName, const ConstString& text) {
    if (!createPath(fileName)) {
        return false;
    }
    FILE *fout = fopen(fileName.c_str(), "w");
    if (!fout) {
        return false;
    }
    fprintf(fout, "%s", text.c_str());
    fclose(fout);
    fout = nullptr;
    return true;
}



ConstString NameConfig::getHostName(bool prefer_loopback, const ConstString& seed) {
    // try to pick a good host identifier

    ConstString result = "127.0.0.1";
    bool loopback = true;
    bool found = false;

    // Pick an IPv4 address.
    // Prefer non-local addresses, then seed, then shorter addresses.
    // Avoid IPv6.
#ifdef YARP_HAS_ACE
    ACE_INET_Addr *ips = nullptr;
    size_t count = 0;
    char hostAddress[256];
    if (ACE::get_ip_interfaces(count, ips)>=0) {
        for (size_t i=0; i<count; i++) {
            ConstString ip = ips[i].get_host_addr(hostAddress, 256);
#else
    int family, s;
    char hostname[NI_MAXHOST];
    ConstString ip;
    struct ifaddrs *ifaddr, *ifa;
    if (yarp::os::impl::getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs in getIps");
        std::exit(EXIT_FAILURE);
    }
    for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == nullptr) continue;
        family = ifa->ifa_addr->sa_family;
        if (family == AF_INET || family == AF_INET6) {
            s = yarp::os::impl::getnameinfo(ifa->ifa_addr,
                            (family == AF_INET) ?
                                sizeof(struct sockaddr_in) :
                                sizeof(struct sockaddr_in6),
                            hostname,
                            NI_MAXHOST,
                            nullptr,
                            0,
                            NI_NUMERICHOST);
            if (s != 0) {
                printf("getnameinfo() failed: %s\n", yarp::os::impl::gai_strerror(s));
                std::exit(EXIT_FAILURE);
            }
            ip = ConstString(hostname);
#endif

            YARP_DEBUG(Logger::get(), ConstString("scanning network interface ") +
                       ip.c_str());

            if (ip.find(':')!=ConstString::npos) continue;

            bool would_be_loopback = false;
            if (ip == "127.0.0.1" || ip == "127.1.0.1" ||
                ip == "127.0.1.1") {
                would_be_loopback = true;
            }
#ifdef YARP_HAS_ACE
#ifdef ACE_ADDR_HAS_LOOPBACK_METHOD
            would_be_loopback = ips[i].is_loopback();
#endif
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
            if (would_be_loopback != prefer_loopback) continue;

            // This is the right kind of interface

            // If we haven't the right kind of interface yet, take it
            if (prefer_loopback != loopback) {
                result = ip;
                loopback = would_be_loopback;
                continue;
            }

            // If it matches the seed interface, take it
            if (ip==seed) {
                result = ip;
                loopback = would_be_loopback;
                continue;
            }

            // If it is shorter, and what we have isn't the seed, take it
            if (ip.length()<result.length() && result!=seed) {
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

    return result.c_str();
}


bool NameConfig::isLocalName(const ConstString& name) {
    bool result = false;

#ifdef YARP_HAS_ACE
    ACE_INET_Addr *ips = nullptr;
    size_t count = 0;
    if (ACE::get_ip_interfaces(count, ips)>=0) {
        for (size_t i=0; i<count; i++) {
            ConstString ip = ips[i].get_host_addr();
            if (ip==name) {
                result = true;
                break;
            }
        }
        delete[] ips;
    }
#else
    /**
     * If this does not work properly, use a more sophisticated way
     * instead of just gethostname.
     */
    char hostname[HOST_NAME_MAX];
    yarp::os::impl::gethostname(hostname, HOST_NAME_MAX);
    if (strcmp(hostname, name.c_str()) == 0) result = true;
    if (!result) {
        Bottle lst = getIpsAsBottle();
        for (int i=0; i<lst.size(); i++) {
            if (lst.get(i).asString()==name) {
                result = true;
                break;
            }
        }
    }
#endif

    // just in case
    if (name=="localhost"||name=="127.0.0.1") {
        result = true;
    }

    return result;
}

yarp::os::Bottle NameConfig::getIpsAsBottle() {
    yarp::os::Bottle result;

#if defined(YARP_HAS_ACE)
    ACE_INET_Addr *ips = nullptr;
    size_t count = 0;
    if (ACE::get_ip_interfaces(count, ips)>=0) {
        for (size_t i=0; i<count; i++) {
            ConstString ip = ips[i].get_host_addr();
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
        if (ifa->ifa_addr == nullptr) continue;
        family = ifa->ifa_addr->sa_family;
        if (family == AF_INET || family == AF_INET6) {
            s = yarp::os::impl::getnameinfo(ifa->ifa_addr,
                                            (family == AF_INET) ?
                                                sizeof(struct sockaddr_in) :
                                                sizeof(struct sockaddr_in6),
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


ConstString NameConfig::getIps() {
    yarp::os::Bottle bot = getIpsAsBottle();
    ConstString result = "";
    for (int i=0; i<bot.size(); i++) {
        ConstString ip = bot.get(i).asString();
        if (i>0) {
            result += " ";
        }
        result += ip;
    }
    return result.c_str();
}



void NameConfig::setAddress(const Contact& address) {
    this->address = address;
}


void NameConfig::setNamespace(const ConstString& ns) {
    space = ns;
}

ConstString NameConfig::getNamespace(bool refresh) {
    if (space==""||refresh) {
        ConstString senv = NetworkBase::getEnvironment("YARP_NAMESPACE");
        if (senv!="") {
            spaces.fromString(senv.c_str());
        } else {
            ConstString fname = getConfigFileName(YARP_CONFIG_NAMESPACE_FILENAME);
            spaces.fromString(readConfig(fname).c_str());
        }
        space = spaces.get(0).asString().c_str();
        if (space=="") {
            space = "/root";
        }
        if (spaces.size()==0) {
            spaces.addString("/root");
        }
    }
    return space;
}

Bottle NameConfig::getNamespaces(bool refresh) {
    getNamespace(refresh);
    return spaces;
}
