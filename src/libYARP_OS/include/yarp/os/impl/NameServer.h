/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef YARP2_NAMESERVER
#define YARP2_NAMESERVER

#include <yarp/os/ConstString.h>
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
    virtual ConstString apply(const ConstString& txt, const Contact& remote) = 0;
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
    Contact registerName(const ConstString& name,
                         const Contact& address) {
        return registerName(name,address,"...");
    }

    Contact registerName(const ConstString& name,
                         const Contact& address,
                         const ConstString& remote);

    Contact registerName(const ConstString& name) {
        return registerName(name,Contact());
    }

    Contact queryName(const ConstString& name);

    Contact unregisterName(const ConstString& name);

    static int main(int argc, char *argv[]);

    virtual ConstString apply(const ConstString& txt, const Contact& remote);

    bool apply(const yarp::os::Bottle& cmd, yarp::os::Bottle& result,
               const Contact& remote);

    ConstString apply(const ConstString& txt) {
        return apply(txt,Contact());
    }

    virtual void onEvent(yarp::os::Bottle& event) {
    }

    static ConstString textify(const Contact& address);
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
        ConstString prefix;
    public:
        DisposableNameRecord() {
            base = 1;
            prefix = "/tmp/port/";
        }

        ConstString get() {
            return prefix + NetType::toString(getFree());
        }

        virtual int fresh() {
            int result = base;
            base++;
            return result;
        }

        bool release(const ConstString& name) {
            if (name.find(prefix)==0) {
                ConstString num = name.substr(prefix.length());
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
            //YARP_DEBUG(Logger::get(), ConstString("host record says ") +
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

        ConstString get() {
            int x = getFree();
            last = x;
            int v1 = x%255;
            int v2 = x/255;
            yAssert(v2<255);
            return ConstString("224.1.") + NetType::toString(v2+1) + "." +
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
        PlatformVector<ConstString> prop;
    public:
        PropertyRecord() {
        }

        void clear() {
            prop.clear();
        }

        void add(const ConstString& p) {
            prop.push_back(p);
        }

        bool check(const ConstString& p) {
            for (unsigned int i=0; i<prop.size(); i++) {
                if (prop[i]==p) {
                    return true;
                }
            }
            return false;
        }

        ConstString match(const ConstString& str) {
            ConstString base = "";
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

        ConstString toString() {
            ConstString base = "";
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
        PLATFORM_MAP(ConstString,PropertyRecord) propMap;
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


        PropertyRecord *getPR(const ConstString& key, bool create = true) {
            PLATFORM_MAP_ITERATOR(ConstString,PropertyRecord,entry);
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

        void clearProp(const ConstString& key) {
            getPR(key)->clear();
        }

        void addProp(const ConstString& key, const ConstString& val) {
            getPR(key)->add(val);
        }

        ConstString getProp(const ConstString& key) {
            PropertyRecord *rec = getPR(key,false);
            if (rec!=NULL) {
                return rec->toString();
            }
            return "";
        }

        bool checkProp(const ConstString& key, const ConstString& val) {
            PropertyRecord *rec = getPR(key,false);
            if (rec!=NULL) {
                return rec->check(val);
            }
            return false;
        }

        ConstString matchProp(const ConstString& key, const ConstString& val) {
            PropertyRecord *rec = getPR(key,false);
            if (rec!=NULL) {
                return rec->match(val);
            }
            return "";
        }

    };






    ConstString cmdRegister(int argc, char *argv[]);
    ConstString cmdQuery(int argc, char *argv[]);
    ConstString cmdUnregister(int argc, char *argv[]);
    ConstString cmdAnnounce(int argc, char *argv[]);
    ConstString cmdHelp(int argc, char *argv[]);
    ConstString cmdSet(int argc, char *argv[]);
    ConstString cmdGet(int argc, char *argv[]);
    ConstString cmdCheck(int argc, char *argv[]);
    ConstString cmdMatch(int argc, char *argv[]);
    ConstString cmdList(int argc, char *argv[]);
    ConstString cmdRoute(int argc, char *argv[]);
    ConstString cmdGarbageCollect(int argc, char *argv[]);
    ConstString cmdBot(int argc, char *argv[]);

    // making a more easy to parse interface
    yarp::os::Bottle ncmdList(int argc, char *argv[]);
    yarp::os::Bottle ncmdQuery(int argc, char *argv[]);
    yarp::os::Bottle ncmdVersion(int argc, char *argv[]);
    yarp::os::Bottle ncmdSet(int argc, char *argv[]);
    yarp::os::Bottle ncmdGet(int argc, char *argv[]);

    PLATFORM_MAP(ConstString,NameRecord) nameMap;
    PLATFORM_MAP(ConstString,HostRecord) hostMap;

    McastRecord mcastRecord;
    DisposableNameRecord tmpNames;

    NameRecord *getNameRecord(const ConstString& name, bool create);

    NameRecord &getNameRecord(const ConstString& name) {
        NameRecord *result = getNameRecord(name,true);
        yAssert(result!=NULL);
        return *result;
    }

    HostRecord *getHostRecord(const ConstString& name, bool create);

    HostRecord &getHostRecord(const ConstString& name) {
        HostRecord *result = getHostRecord(name,true);
        yAssert(result!=NULL);
        return *result;
    }

    Dispatcher<NameServer,ConstString> dispatcher;
    Dispatcher<NameServer,yarp::os::Bottle> ndispatcher;

protected:

    ConstString terminate(const ConstString& str);

    int basePort;

private:
    yarp::os::Semaphore mutex;
};

#endif
