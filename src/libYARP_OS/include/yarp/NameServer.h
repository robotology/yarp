// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef _YARP2_NAMESERVER_
#define _YARP2_NAMESERVER_

#include <yarp/String.h>
#include <yarp/Address.h>
#include <yarp/Logger.h>
#include <yarp/Dispatcher.h>
#include <yarp/NetType.h>
#include <yarp/SplitString.h>
#include <yarp/os/Time.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Bottle.h>

#include <ace/Hash_Map_Manager.h>
#include <ace/Vector_T.h>
#include <ace/Null_Mutex.h>

// ACE headers may fiddle with main
#ifdef main
#undef main
#endif

namespace yarp {
    class NameServer;
}

/**
 * Implementation of a YARP2-conforming name server.
 */
class yarp::NameServer {
public:

    NameServer() : nameMap(17), hostMap(17), mutex(1) {
        setup();
    }

    virtual ~NameServer() {}

    // address may be partial - partial information gets filled in
    // (not YARP2 compliant yet, won't do fill-in)
    Address registerName(const String& name, 
                         const Address& address) {
        return registerName(name,address,"...");
    }

    Address registerName(const String& name, 
                         const Address& address,
                         const String& remote);

    Address registerName(const String& name) {
        return registerName(name,Address());
    }

    Address queryName(const String& name);

    Address unregisterName(const String& name);

    static int main(int argc, char *argv[]);

    String apply(const String& txt, const Address& remote);

    String apply(const String& txt) {
        return apply(txt,Address());
    }

    virtual void onEvent(yarp::os::Bottle& event) {
    }
private:

    void setup();

    template <class T>
    class ReusableRecord {
    private:
        ACE_Vector<T> reuse;
    public:
        virtual ~ReusableRecord() {}

        virtual T fresh() = 0;

        void release(const T& o) {
            reuse.push_back(o);
        }

        T getFree() {
            if (reuse.size()>=1) {
                T result = reuse[reuse.size()-1];
                reuse.pop_back();
                return result;
            }
            return fresh();
        }
    };


    class DisposableNameRecord : public ReusableRecord<int> {
    private:
        int base;
        String prefix;
    public:
        DisposableNameRecord() {
            base = 1;
            prefix = "/tmp/port/";
        }

        String get() {
            return prefix + NetType::toString(getFree());
        }

        virtual int fresh() {
            int result = base;
            base++;
            return result;
        }

        bool release(const String& name) {
            if (name.strstr(prefix)==0) {
                String num = name.substr(prefix.length());
                int x = NetType::toInt(num);
                ReusableRecord<int>::release(x);
                return true;
            }
            return false;
        }
    };


    class HostRecord : public ReusableRecord<int> {
    private:
        int base;
        int legacyStep; // this is for YARP1 compatibility

    public:
        HostRecord() {
            //YARP_DEBUG(Logger::get(),"FIXME: HostRecord has hardcoded base");
            base = 0;
            legacyStep = 10;
        }

        void setBase(int base) {
            this->base = base;
        }

        int get() {
            int result = ReusableRecord<int>::getFree();
            //YARP_DEBUG(Logger::get(), String("host record says ") +
            //NetType::toString(result) + " is free");
            return result;
        }

        virtual int fresh() {
            int result = base;
            base += legacyStep;
            return result;
        }
    };


    class McastRecord : public ReusableRecord<int> {
    private:
        int base;
    public:
        McastRecord() {
            //YARP_DEBUG(Logger::get(),"FIXME: mcast records are never reused");
            base = 0;
        }

        virtual int fresh() {
            int result = base;
            base++;
            return result;
        }

        String get() {
            int x = getFree();
            int v1 = x%255;
            int v2 = x/255;
            YARP_ASSERT(v2<255);
            return String("224.1.") + NetType::toString(v2+1) + "." + 
                NetType::toString(v1+1);
        }

        void releaseAddress(const char *addr) {
            SplitString ss(addr,'.');
            int ip[] = { 224, 3, 1, 1 };
            YARP_ASSERT(ss.size()==4);
            for (int i=0; i<4; i++) {
                ip[i] = NetType::toInt(ss.get(i));
            }
            int v2 = ip[2]-1;
            int v1 = ip[3]-1;
            int x = v2*255+v1;
            printf("Releasing %s %d  %d:%d\n", addr, x, v2, v1);
            release(x);
        }
    };


    class PropertyRecord {
    private:
        ACE_Vector<String> prop;
    public:
        PropertyRecord() {
        }
    
        void clear() {
            prop.clear();
        }

        void add(const String& p) {
            prop.push_back(p);
        }

        bool check(const String& p) {
            for (unsigned int i=0; i<prop.size(); i++) {
                if (prop[i]==p) {
                    return true;
                }
            }
            return false;
        }

        String match(const String& str) {
            String base = "";
            bool needSpace = false;
            for (unsigned int i=0; i<prop.size(); i++) {
                if (prop[i].strstr(str)==0) {
                    if (needSpace) base += " ";
                    base += prop[i];
                    needSpace = true;
                }
            }
            return base;
        }

        String toString() {
            String base = "";
            for (unsigned int i=0; i<prop.size(); i++) {
                if (i>0) {
                    base += " ";
                }
                base += prop[i];
            }
            return base;
        }
    };

    class NameRecord {
    private:
        Address address;
        bool reusablePort;
        bool reusableIp;
        ACE_Hash_Map_Manager<String,PropertyRecord,ACE_Null_Mutex> propMap;
    public:
        NameRecord() : propMap(5) {
            reusableIp = false;
            reusablePort = false;
        }

        NameRecord(const NameRecord& alt) : propMap(5) {
            reusableIp = false;
            reusablePort = false;
        }

        bool isReusablePort() {
            return reusablePort;
        }

        bool isReusableIp() {
            return reusableIp;
        }

        void clear() {
            propMap.unbind_all();
            address = Address();
            reusableIp = false;
            reusablePort = false;
        }

        void setAddress(const Address& address, 
                        bool reusablePort=false,
                        bool reusableIp=false) {
            this->address = address;
            this->reusablePort = reusablePort;
            this->reusableIp = reusableIp;
        }

        Address getAddress() {
            return address;
        }


        PropertyRecord *getPR(const String& key, bool create = true) {
            ACE_Hash_Map_Entry<String,PropertyRecord> *entry = NULL;
            int result = propMap.find(key,entry);
            if (result==-1 && create) {
                PropertyRecord blank;
                propMap.bind(key,blank);
                result = propMap.find(key,entry);
                YARP_ASSERT(result!=-1);
            }
            if (result==-1) {
                return NULL;
            }
            return &(entry->int_id_);
        }

        void clearProp(const String& key) {
            getPR(key)->clear();
        }

        void addProp(const String& key, const String& val) {
            getPR(key)->add(val);
        }

        String getProp(const String& key) {
            PropertyRecord *rec = getPR(key,false);
            if (rec!=NULL) {
                return rec->toString();
            }
            return "";
        }

        bool checkProp(const String& key, const String& val) {
            PropertyRecord *rec = getPR(key,false);
            if (rec!=NULL) {
                return rec->check(val);
            }
            return false;
        }

        String matchProp(const String& key, const String& val) {
            PropertyRecord *rec = getPR(key,false);
            if (rec!=NULL) {
                return rec->match(val);
            }
            return "";
        }

    };


  



    String cmdRegister(int argc, char *argv[]);
    String cmdQuery(int argc, char *argv[]);
    String cmdUnregister(int argc, char *argv[]);
    String cmdHelp(int argc, char *argv[]);
    String cmdSet(int argc, char *argv[]);
    String cmdGet(int argc, char *argv[]);
    String cmdCheck(int argc, char *argv[]);
    String cmdMatch(int argc, char *argv[]);
    String cmdList(int argc, char *argv[]);
    String cmdRoute(int argc, char *argv[]);
    String cmdGarbageCollect(int argc, char *argv[]);


    typedef ACE_Hash_Map_Manager<String,NameRecord,ACE_Null_Mutex> NameMapHash;

    NameMapHash nameMap;
    ACE_Hash_Map_Manager<String,HostRecord,ACE_Null_Mutex> hostMap;
    McastRecord mcastRecord;
    DisposableNameRecord tmpNames;
  
    NameRecord *getNameRecord(const String& name, bool create);

    NameRecord& getNameRecord(const String& name) {
        NameRecord *result = getNameRecord(name,true);
        YARP_ASSERT(result!=NULL);
        return *result;
    }

    HostRecord *getHostRecord(const String& name, bool create);

    HostRecord& getHostRecord(const String& name) {
        HostRecord *result = getHostRecord(name,true);
        YARP_ASSERT(result!=NULL);
        return *result;
    }

    Dispatcher<NameServer,String> dispatcher;

protected:

    String textify(const Address& addr);
    String terminate(const String& str);

    int basePort;

private:
    yarp::os::Semaphore mutex;
};

#endif

