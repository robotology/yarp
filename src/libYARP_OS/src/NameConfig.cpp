// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006, 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia, Anne van Rossum
 * Authors: Paul Fitzpatrick, Anne van Rossum
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/impl/NameConfig.h>
#include <yarp/os/impl/SplitString.h>
#include <yarp/os/NetType.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Os.h>
#include <yarp/os/Property.h>
#include <yarp/os/Network.h>
#include <yarp/os/ResourceFinder.h>

#include <yarp/os/impl/PlatformStdlib.h>

#ifdef YARP_HAS_ACE
#  include <ace/OS_NS_sys_stat.h>
#  include <ace/OS_NS_netdb.h>
#  include <ace/INET_Addr.h>
#  include <ace/Sock_Connect.h>
#else
#  include <cstring>
#  include <arpa/inet.h>
#  include <sys/socket.h>
#  include <netdb.h>
#  include <ifaddrs.h>
#endif

#include <stdio.h>
#include <yarp/conf/system.h>

using namespace yarp::os::impl;
using namespace yarp::os;

#define CONF_FILENAME YARP_CONFIG_FILENAME

bool NameConfig::fromString(const String& txt) {
    address = Contact();
    SplitString ss(txt.c_str());
    if (ss.size()>=1) {
        if (ss.get(0)[0]=='[') {
            // use Property format
            Property config;
            config.fromConfig(txt.c_str());
            
            Bottle& b = config.findGroup("name");
            if (b.isNull()) {
                fprintf(stderr,"Cannot find yarp group in config file\n");
                exit(1);
            }
            address = Contact(b.find("host").asString().c_str(),
                              b.find("port").asInt());
            mode = b.check("mode",Value("yarp")).asString().c_str();
            return (address.getPort()!=0);
        }
    }

    if (ss.size()>=2) {
        address = Contact(ss.get(0),NetType::toInt(ss.get(1)));
        if (ss.size()>=3) {
            mode = ss.get(2);
        } else {
            mode = "yarp";
        }
        if (mode=="ros") {
            address = address.addCarrier("xmlrpc");
        }
        return true;
    }
    return false;
}

String NameConfig::expandFilename(const char *fname) {
    ConstString root = NetworkBase::getEnvironment("YARP_CONF");

    // yarp 2.4 modifications begin
    // We should now be using YARP_CONFIG_HOME.
    // We still respect the YARP_CONF variable if defined, but it
    // is deprecated.
    if (root=="") {
        root = ResourceFinder::getConfigHome();
    }
    // yarp 2.4 modifications end

    ConstString home = NetworkBase::getEnvironment("HOME");
    ConstString homepath = NetworkBase::getEnvironment("HOMEPATH");
    ConstString conf = "";
    if (root!="") {
        //conf = new File(new File(root,"conf"),"namer.conf");
        //conf = root + "/conf/" + fname;
        // users of YARP_CONF want /conf postfix removed
        conf = root + NetworkBase::getDirectorySeparator() + fname;
    } else if (homepath!="") {
        conf = NetworkBase::getEnvironment("HOMEDRIVE") + homepath + "\\yarp\\conf\\" + fname;
    } else if (home!="") {
        conf = home + "/.yarp/conf/" + fname;
    } else {
        YARP_ERROR(Logger::get(),"Cannot read configuration - please set YARP_CONF or HOME or HOMEPATH");
        ACE_OS::exit(1);
    }
    YARP_DEBUG(Logger::get(),String("Configuration file: ") + conf.c_str());
    return conf.c_str();
}


String NameConfig::getConfigFileName(const char *stem, const char *ns) {
    String fname = (stem!=NULL)?stem:CONF_FILENAME;
    if (stem==NULL) {
        String space;
        if (ns) {
            space = ns;
        } else {
            space = getNamespace();
        }
        if (space!="/root") {
            // for non-default namespace, need a separate cache file
            String base = "";
            for (unsigned int i=0; i<space.length(); i++) {
                if (space[i]!='/') {
                    base += space[i];
                } else {
                    base += "_";
                }
            }
            base += ".conf";
            fname = base;
        }
    }
    return expandFilename(fname.c_str());
}


bool NameConfig::createPath(const String& fileName, int ignoreLevel) {
    size_t index = fileName.rfind('/');
    if (index==String::npos) {
        index = fileName.rfind('\\');
        if (index==String::npos) {
            return false;
        }
    }
    String base = fileName.substr(0,index);
    ACE_stat sb;
    if (ACE_OS::stat((char*)base.c_str(),&sb)<0) {
        bool result = createPath(base,ignoreLevel-1);
        if (result==false) {
            return false;
        }
    }
    if (ignoreLevel<=0) {
        if (ACE_OS::stat(fileName.c_str(),&sb)<0) {
            if (yarp::os::mkdir(fileName.c_str())>=0) {
                return true;
            }
            return false;
        }
    }
    return true;
}

String NameConfig::readConfig(const String& fileName) {
    char buf[25600];
    FILE *fin = fopen(fileName.c_str(),"r");
    if (fin==NULL) return "";
    String result = "";
    while(fgets(buf, sizeof(buf)-1, fin) != NULL) {
        result += buf;
    }
    fclose(fin);
    fin = NULL;
    return result;
}


bool NameConfig::fromFile(const char *ns) {
    String fname = getConfigFileName(NULL,ns);
    if (fname!="") {
        String txt = readConfig(fname);
        if (txt!="") {
            return fromString(txt);
        }
    }
    return false;
}


bool NameConfig::toFile(bool clean) {
    String fname = getConfigFileName();
    if (fname!="") {
        String txt = "";
        if (!clean) {
            String m = (mode!="")?mode:"yarp";
            txt += address.getHost() + " " + NetType::toString(address.getPort()) + " " + m + "\n";
        }
        return writeConfig(fname,txt);
    }
    return false;
}


Contact NameConfig::getAddress() {
    return address;
}


bool NameConfig::writeConfig(const String& fileName, const String& text) {
    if (!createPath(fileName)) {
        return false;
    }
    FILE *fout = fopen(fileName.c_str(),"w");
    if (fout==NULL) return false;
    fprintf(fout,"%s",text.c_str());
    fclose(fout);
    fout = NULL;
    return true;
}



String NameConfig::getHostName(bool prefer_loopback) {
    // try to pick a good host identifier

    ConstString result = "127.0.0.1";
    bool loopback = true;

    // Pick an IPv4 address.
    // Prefer non-local addresses, then shorter addresses.
    // Avoid IPv6.
#ifdef YARP_HAS_ACE
    ACE_INET_Addr *ips = NULL;
    size_t count = 0;
    if (ACE::get_ip_interfaces(count,ips)>=0) {
        for (size_t i=0; i<count; i++) {
            ConstString ip = ips[i].get_host_addr();
#else
    int family, s;
    char hostname[NI_MAXHOST]; hostname[NI_MAXHOST-1] = '\0';
    ConstString ip;
    struct ifaddrs *ifaddr, *ifa;
    if (getifaddrs(&ifaddr) == -1) {
    	perror("getifaddrs in getIps");
    	exit(EXIT_FAILURE);
    }
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
    	if (ifa->ifa_addr == NULL) continue;
    	family = ifa->ifa_addr->sa_family;
    	if (family == AF_INET || family == AF_INET6) {
    		s = getnameinfo(ifa->ifa_addr,
    				(family == AF_INET) ? sizeof(struct sockaddr_in) :
    						sizeof(struct sockaddr_in6),
    						hostname, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
    		if (s != 0) {
    			printf("getnameinfo() failed: %s\n", gai_strerror(s));
    			exit(EXIT_FAILURE);
    		}
    		ip = ConstString(hostname);
#endif

            YARP_DEBUG(Logger::get(), String("scanning network interface ") +
                       ip.c_str());
            bool take = prefer_loopback;
            if (ip.find(":")!=ConstString::npos) continue;
            if (!take) {
                if (result=="localhost") {
                    take = true; // can't be worse
                }
                if (loopback) {
                    take = true; // can't be worse
                } else if (ip.length()<result.length()) {
                    take = true;
                }
            }
            if (take) {
                if (!prefer_loopback) result = ip;
                loopback = false;
                if (ip == "127.0.0.1" || ip == "127.1.0.1" ||
                    ip == "127.0.1.1") {
                    loopback = true;
                }
#ifdef YARP_HAS_ACE
#ifdef ACE_ADDR_HAS_LOOPBACK_METHOD
                loopback = ips[i].is_loopback();
#endif
#endif

                if (prefer_loopback && loopback) {
                    result = ip;
                }
            }
        }
#ifdef YARP_HAS_ACE
        delete[] ips;
#endif
    }

    return result.c_str();
}


bool NameConfig::isLocalName(const String& name) {
    bool result = false;

#ifdef YARP_HAS_ACE
    ACE_INET_Addr *ips = NULL;
    size_t count = 0;
    if (ACE::get_ip_interfaces(count,ips)>=0) {
        for (size_t i=0; i<count; i++) {
            String ip = ips[i].get_host_addr();
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
    char hostname[NI_MAXHOST]; hostname[NI_MAXHOST-1] = '\0';
    gethostname(hostname, NI_MAXHOST);
    if (strcmp(hostname, name.c_str()) == 0) result = true;
#endif

    // just in case
    if (name=="localhost"||name=="127.0.0.1") { result = true; }

    return result;
}

yarp::os::Bottle NameConfig::getIpsAsBottle() {
    yarp::os::Bottle result;

#ifdef YARP_HAS_ACE
    ACE_INET_Addr *ips = NULL;
    size_t count = 0;
    if (ACE::get_ip_interfaces(count,ips)>=0) {
        for (size_t i=0; i<count; i++) {
            String ip = ips[i].get_host_addr();
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
    	exit(EXIT_FAILURE);
    }
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
    	if (ifa->ifa_addr == NULL) continue;
    	family = ifa->ifa_addr->sa_family;
    	if (family == AF_INET || family == AF_INET6) {
    		s = getnameinfo(ifa->ifa_addr,
    				(family == AF_INET) ? sizeof(struct sockaddr_in) :
    						sizeof(struct sockaddr_in6),
    						host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
    		if (s != 0) {
    			printf("getnameinfo() failed: %s\n", gai_strerror(s));
    			exit(EXIT_FAILURE);
    		}
    		result.addString(host);
    	}
    }
#endif

    return result;
}


String NameConfig::getIps() {
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


void NameConfig::setNamespace(const String& ns) {
    space = ns;
}

String NameConfig::getNamespace(bool refresh) {
    if (space==""||refresh) {
        ConstString senv = NetworkBase::getEnvironment("YARP_NAMESPACE");
        if (senv!="") {
            spaces.fromString(senv.c_str());
        } else {
            String fname = getConfigFileName(YARP_CONFIG_NAMESPACE_FILENAME);
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

