// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/impl/NameConfig.h>
#include <yarp/os/impl/SplitString.h>
#include <yarp/os/impl/NetType.h>
#include <yarp/os/Bottle.h>

#include <ace/OS_NS_stdlib.h>
#include <ace/OS_NS_sys_stat.h>
#include <ace/OS_NS_netdb.h>
#include <ace/INET_Addr.h>
#include <ace/Sock_Connect.h>

#include <stdio.h>
#include <yarp/conf/system.h>

using namespace yarp::os::impl;
using namespace yarp::os;

#define CONF_FILENAME YARP_CONFIG_FILENAME


String NameConfig::getEnv(const String& key, bool *found) {
    const char *result = ACE_OS::getenv(key.c_str());
    if (found != NULL) {
        *found = (result!=NULL);
    }
    if (result == NULL) {
        return "";
    }
    return result;
}


bool NameConfig::fromString(const String& txt) {
    address = Address();
    SplitString ss(txt.c_str());
    if (ss.size()>=2) {
        address = Address(ss.get(0),NetType::toInt(ss.get(1)));
        return true;
    }
    return false;
}


String NameConfig::getConfigFileName(const char *stem) {
    String root = getEnv("YARP_CONF");
    String home = getEnv("HOME");
    String homepath = getEnv("HOMEPATH");
    String conf = "";
    String fname = (stem!=NULL)?stem:CONF_FILENAME;
    if (stem==NULL) {
        String space = getNamespace();
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
    if (root!="") {
        //conf = new File(new File(root,"conf"),"namer.conf");
        //conf = root + "/conf/" + fname;
        // users of YARP_CONF want /conf postfix removed
        conf = root + "/" + fname;
    } else if (homepath!="") {
        conf = getEnv("HOMEDRIVE") + homepath + "\\yarp\\conf\\" + fname;
    } else if (home!="") {
        conf = home + "/.yarp/conf/" + fname;
    } else {
        YARP_ERROR(Logger::get(),"Cannot read configuration - please set YARP_CONF or HOME or HOMEPATH");
        ACE_OS::exit(1);
    }
    YARP_DEBUG(Logger::get(),String("Configuration file: ") + conf);
    return conf;
}


bool NameConfig::createPath(const String& fileName, int ignoreLevel) {
    int index = fileName.rfind('/');
    if (index==-1) {
		index = fileName.rfind('\\');
	    if (index==-1) {
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
            if (ACE_OS::mkdir(fileName.c_str())>=0) {
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


bool NameConfig::fromFile() {
    String fname = getConfigFileName();
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
            txt += address.getName() + " " + NetType::toString(address.getPort()) + 
                "\n\n";
            txt += "// start network description, don't forget to separate \"Node=\" and names with space\n";
            txt += "[NETWORK_DESCRIPTION]\n";
            txt += "[END]\n";
        }
        return writeConfig(fname,txt);
    }
    return false;
}


Address NameConfig::getAddress() {
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



String NameConfig::getHostName() {
    // try to pick a good host identifier

    String result = "127.0.0.1";
    bool loopback = true;

    ACE_INET_Addr *ips = NULL;
    size_t count = 0;
    // Pick an IP address.
    // Prefer non-local addresses, then shorter addresses.
    // Avoid ::1 and the like.
    if (ACE::get_ip_interfaces(count,ips)>=0) {
        for (size_t i=0; i<count; i++) {
            String ip = ips[i].get_host_addr();
            YARP_DEBUG(Logger::get(), String("scanning network interface ") +
                       ip);
            bool take = false;
            if (ip[0]!=':') {
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
                result = ip;
#ifdef YARP_ACE_ADDR_HAS_LOOPBACK_METHOD
                loopback = ips[i].is_loopback();
#else
                loopback = false;
                if (ip == "127.0.0.1" || ip == "127.1.0.1" ||
                    ip == "127.0.1.1") {
                    loopback = true;
                }
#endif
            }
        }
        delete[] ips;
    }

    return result;
}


bool NameConfig::isLocalName(const String& name) {
    bool result = false;
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

    // just in case
    if (name=="localhost"||name=="127.0.0.1") { result = true; }

    return result;
}

yarp::os::Bottle NameConfig::getIpsAsBottle() {
    yarp::os::Bottle result;
    ACE_INET_Addr *ips = NULL;
    size_t count = 0;
    if (ACE::get_ip_interfaces(count,ips)>=0) {
        for (size_t i=0; i<count; i++) {
            String ip = ips[i].get_host_addr();
            result.addString(ip.c_str());
        }
        delete[] ips;
    }
    return result;
}


String NameConfig::getIps() {
    String result = "";
    ACE_INET_Addr *ips = NULL;
    size_t count = 0;
    if (ACE::get_ip_interfaces(count,ips)>=0) {
        for (size_t i=0; i<count; i++) {
            String ip = ips[i].get_host_addr();
            if (i>0) {
                result += " ";
            }
            result += ip;
        }
        delete[] ips;
    }
    return result;
}



void NameConfig::setAddress(const Address& address) {
    this->address = address;
}


String NameConfig::getNamespace(bool refresh) {
    if (space==""||refresh) {
        String fname = getConfigFileName(YARP_CONFIG_NAMESPACE_FILENAME);
        Bottle bot(readConfig(fname).c_str());
        space = bot.get(0).asString().c_str();
        if (space=="") {
            space = "/root";
        }
    }
    return space;
}

