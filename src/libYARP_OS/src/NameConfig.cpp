// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/NameConfig.h>
#include <yarp/SplitString.h>
#include <yarp/NetType.h>

#include <ace/OS_NS_stdlib.h>
#include <ace/OS_NS_sys_stat.h>
#include <ace/OS_NS_netdb.h>
#include <ace/INET_Addr.h>
#include <ace/Sock_Connect.h>

// does ACE require new c++ header files or not?
#if ACE_HAS_STANDARD_CPP_LIBRARY
#include <fstream>
using namespace std;
#else
#include <fstream.h>
#endif

using namespace yarp;

#define CONF_FILENAME "yarp.conf"


String NameConfig::getEnv(const String& key) {
    const char *result = ACE_OS::getenv(key.c_str());
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


String NameConfig::getConfigFileName() {
    String root = getEnv("YARP_CONF");
    String home = getEnv("HOME");
    String homepath = getEnv("HOMEPATH");
    String conf = "";
    if (root!="") {
        //conf = new File(new File(root,"conf"),"namer.conf");
        conf = root + "/conf/" + CONF_FILENAME;
    } else if (home!="") {
        conf = home + "/.yarp/conf/" + CONF_FILENAME;
    } else if (homepath!="") {
        conf = getEnv("HOMEDRIVE") + homepath + "\\yarp\\conf\\" + CONF_FILENAME;
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
    ifstream fin(fileName.c_str());
    if (fin.eof()||fin.fail()) {
        return "";
    }
    String result = "";
    while (!(fin.eof()||fin.bad())) {
        char buf[25600];
        fin.getline(buf,sizeof(buf),'\n');
        if (!(fin.eof()||fin.bad())) {
            result += buf;
            result += "\n";
        }
    }
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


bool NameConfig::toFile() {
    String fname = getConfigFileName();
    if (fname!="") {
        String txt = "";
        txt += address.getName() + " " + NetType::toString(address.getPort()) + 
            "\n\n";
        txt += "// start network description, don't forget to separate \"Node=\" and names with space\n";
        txt += "[NETWORK_DESCRIPTION]\n";
        txt += "[END]\n";
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
    ofstream fout(fileName.c_str());
    if (fout.fail()) {
        return false;
    }
    fout << text.c_str();
    fout.close();
    return !fout.fail();
}



String NameConfig::getHostName() {
    // try to pick a good host identifier

    String result = "localhost";

    ACE_INET_Addr *ips = NULL;
    size_t count = 0;
    if (ACE::get_ip_interfaces(count,ips)>=0) {
        for (size_t i=0; i<count; i++) {
            String ip = ips[i].get_host_addr();
            YARP_DEBUG(Logger::get(), String("scanning network interface ") +
                       ip);
            if (result=="localhost") {
                result = ip; // can't be worse
            }
            if (result=="127.0.0.1") {
                result = ip; // can't be worse
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
    if (name=="localhost") { result = true; }

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
