// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_NAMESERVER
#define YARP2_NAMESERVER

#include <yarp/os/impl/String.h>
#include <yarp/os/Contact.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/Dispatcher.h>
#include <yarp/os/NetType.h>
#include <yarp/os/impl/SplitString.h>
#include <yarp/os/Time.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/Bottle.h>

#include <yarp/os/impl/PlatformMap.h>
#include <yarp/os/impl/PlatformVector.h>

// ACE headers may fiddle with main
#ifdef main
#undef main
#endif


namespace yarp {
    namespace os {
        namespace impl {
            class NameServer;
            class NameServerStub;
        }
    }
}


/**
 * Stub for a YARP2-conforming name server.
 */
class YARP_OS_impl_API yarp::os::impl::NameServerStub {
public:
    virtual ~NameServerStub() {}
    virtual String apply(const String& txt, const Contact& remote) = 0;
};

/**
 * Implementation of a YARP2-conforming name server.
 */
class YARP_OS_impl_API yarp::os::impl::NameServer : public NameServerStub {
public:

    NameServer() :
#ifndef YARP_USE_STL
        nameMap(17), hostMap(17),
#endif
        mutex(1) {
        setup();
    }

    virtual ~NameServer() {}

    // address may be partial - partial information gets filled in
    // (not YARP2 compliant yet, won't do fill-in)
    Contact registerName(const String& name,
                         const Contact& address) {
        return registerName(name,address,"...");
    }

    Contact registerName(const String& name,
                         const Contact& address,
                         const String& remote);

    Contact registerName(const String& name) {
        return registerName(name,Contact());
    }

    Contact queryName(const String& name);

    Contact unregisterName(const String& name);

    static int main(int argc, char *argv[]);

    virtual String apply(const String& txt, const Contact& remote);

    bool apply(const yarp::os::Bottle& cmd, yarp::os::Bottle& result,
               const Contact& remote);

    String apply(const String& txt) {
        return apply(txt,Contact());
    }

    virtual void onEvent(yarp::os::Bottle& event) {
    }

    static String textify(const Contact& address);
    static yarp::os::Bottle botify(const Contact& address);

    void setBasePort(int basePort) {
        this->basePort = basePort;
        mcastRecord.setBasePort(basePort);
    }


private:

    void setup();

    template <class T>
    class ReusableRecord {
    private:
        PlatformVector<T> reuse;
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
            if (name.find(prefix)==0) {
                String num = name.substr(prefix.length());
                int x = NetType::toInt(num.c_str());
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
        int last;
        int basePort;
    public:

        McastRecord() {
            //YARP_DEBUG(Logger::get(),"FIXME: mcast records are never reused");
            base = 0;
            basePort = 0;
            last = 0;
        }

        void setBasePort(int basePort) {
            this->basePort = basePort;
        }

        virtual int fresh() {
            int result = base;
            base++;
            return result;
        }

        String get() {
            int x = getFree();
            last = x;
            int v1 = x%255;
            int v2 = x/255;
            yAssert(v2<255);
            return String("224.1.") + NetType::toString(v2+1) + "." +
                NetType::toString(v1+1);
        }

        int lastPortNumber() {
            return basePort+last;
        }

        void releaseAddress(const char *addr) {
            SplitString ss(addr,'.');
            int ip[] = { 224, 3, 1, 1 };
            yAssert(ss.size()==4);
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
        PlatformVector<String> prop;
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
                if (prop[i].find(str)==0) {
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
        bool reusablePort;
        bool reusableIp;
        PLATFORM_MAP(String,PropertyRecord) propMap;
        Contact address;
    public:
        NameRecord() :
#ifndef YARP_USE_STL
            propMap(5),
#endif
            address()
        {
            reusableIp = false;
            reusablePort = false;
        }

        NameRecord(const NameRecord& alt) :
#ifndef YARP_USE_STL
            propMap(5),
#endif
            address()
        {
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
            PLATFORM_MAP_CLEAR(propMap);
            address = Contact();
            reusableIp = false;
            reusablePort = false;
        }

        void setAddress(const Contact& address,
                        bool reusablePort=false,
                        bool reusableIp=false) {
            this->address = address;
            this->reusablePort = reusablePort;
            this->reusableIp = reusableIp;
        }

        Contact getAddress() {
            return address;
        }


        PropertyRecord *getPR(const String& key, bool create = true) {
            PLATFORM_MAP_ITERATOR(String,PropertyRecord,entry);
            int result = PLATFORM_MAP_FIND(propMap,key,entry);
            if (result==-1 && create) {
                PropertyRecord blank;
                PLATFORM_MAP_SET(propMap,key,blank);
                result = PLATFORM_MAP_FIND(propMap,key,entry);
                yAssert(result!=-1);
            }
            if (result==-1) {
                return NULL;
            }
            return &(PLATFORM_MAP_ITERATOR_SECOND(entry));
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
    String cmdAnnounce(int argc, char *argv[]);
    String cmdHelp(int argc, char *argv[]);
    String cmdSet(int argc, char *argv[]);
    String cmdGet(int argc, char *argv[]);
    String cmdCheck(int argc, char *argv[]);
    String cmdMatch(int argc, char *argv[]);
    String cmdList(int argc, char *argv[]);
    String cmdRoute(int argc, char *argv[]);
    String cmdGarbageCollect(int argc, char *argv[]);
    String cmdBot(int argc, char *argv[]);

    // making a more easy to parse interface
    yarp::os::Bottle ncmdList(int argc, char *argv[]);
    yarp::os::Bottle ncmdQuery(int argc, char *argv[]);
    yarp::os::Bottle ncmdVersion(int argc, char *argv[]);
    yarp::os::Bottle ncmdSet(int argc, char *argv[]);
    yarp::os::Bottle ncmdGet(int argc, char *argv[]);

    PLATFORM_MAP(String,NameRecord) nameMap;
    PLATFORM_MAP(String,HostRecord) hostMap;

    McastRecord mcastRecord;
    DisposableNameRecord tmpNames;

    NameRecord *getNameRecord(const String& name, bool create);

    NameRecord &getNameRecord(const String& name) {
        NameRecord *result = getNameRecord(name,true);
        yAssert(result!=NULL);
        return *result;
    }

    HostRecord *getHostRecord(const String& name, bool create);

    HostRecord &getHostRecord(const String& name) {
        HostRecord *result = getHostRecord(name,true);
        yAssert(result!=NULL);
        return *result;
    }

    Dispatcher<NameServer,String> dispatcher;
    Dispatcher<NameServer,yarp::os::Bottle> ndispatcher;

protected:

    String terminate(const String& str);

    int basePort;

private:
    yarp::os::Semaphore mutex;
};

#endif
