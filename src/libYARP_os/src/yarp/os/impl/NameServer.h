/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IMPL_NAMESERVER_H
#define YARP_OS_IMPL_NAMESERVER_H

#include <yarp/conf/string.h>
#include <yarp/conf/numeric.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/Contact.h>
#include <yarp/os/LogComponent.h>
#include <yarp/os/NetType.h>
#include <yarp/os/Time.h>
#include <yarp/os/impl/Dispatcher.h>

#include <map>
#include <mutex>
#include <string>
#include <vector>

YARP_DECLARE_LOG_COMPONENT(NAMESERVER)

namespace yarp {
namespace os {
namespace impl {

/**
 * Stub for a YARP2-conforming name server.
 */
class YARP_os_impl_API NameServerStub
{
public:
    virtual ~NameServerStub() = default;
    virtual std::string apply(const std::string& txt, const Contact& remote) = 0;
};

/**
 * Implementation of a YARP2-conforming name server.
 */
class YARP_os_impl_API NameServer : public NameServerStub
{
public:
    NameServer() :
            mutex()
    {
        setup();
    }

    virtual ~NameServer() = default;

    // address may be partial - partial information gets filled in
    // (not YARP2 compliant yet, won't do fill-in)
    Contact registerName(const std::string& name,
                         const Contact& address)
    {
        return registerName(name, address, "...");
    }

    Contact registerName(const std::string& name,
                         const Contact& address,
                         const std::string& remote);

    Contact registerName(const std::string& name)
    {
        return registerName(name, Contact());
    }

    Contact queryName(const std::string& name);

    Contact unregisterName(const std::string& name);

    std::string apply(const std::string& txt, const Contact& remote) override;

    bool apply(const yarp::os::Bottle& cmd, yarp::os::Bottle& result, const Contact& remote);

    std::string apply(const std::string& txt)
    {
        return apply(txt, Contact());
    }

    virtual void onEvent(yarp::os::Bottle& event)
    {
        YARP_UNUSED(event);
    }

    static std::string textify(const Contact& address);
    static yarp::os::Bottle botify(const Contact& address);

    void setBasePort(int basePort)
    {
        this->basePort = basePort;
        mcastRecord.setBasePort(basePort);
    }


private:
    void setup();

    template <class T>
    class ReusableRecord
    {
    private:
        std::vector<T> reuse;

    public:
        virtual ~ReusableRecord() = default;

        virtual T fresh() = 0;

        void release(const T& o)
        {
            reuse.push_back(o);
        }

        T getFree()
        {
            if (reuse.size() >= 1) {
                T result = reuse[reuse.size() - 1];
                reuse.pop_back();
                return result;
            }
            return fresh();
        }
    };


    class DisposableNameRecord : public ReusableRecord<int>
    {
    private:
        int base;
        std::string prefix;

    public:
        DisposableNameRecord()
        {
            base = 1;
            prefix = "/tmp/port/";
        }

        std::string get()
        {
            return prefix + yarp::conf::numeric::to_string(getFree());
        }

        int fresh() override
        {
            int result = base;
            base++;
            return result;
        }

        bool release(const std::string& name)
        {
            if (name.find(prefix) == 0) {
                std::string num = name.substr(prefix.length());
                int x = yarp::conf::numeric::from_string<int>(num);
                ReusableRecord<int>::release(x);
                return true;
            }
            return false;
        }
    };


    class HostRecord : public ReusableRecord<int>
    {
    private:
        int base;

    public:
        HostRecord()
        {
            // FIXME HostRecord has hardcoded base
            base = 0;
        }

        void setBase(int base)
        {
            this->base = base;
        }

        int get()
        {
            int result = ReusableRecord<int>::getFree();
            yCTrace(NAMESERVER, "host record says %d is free", result);
            return result;
        }

        int fresh() override
        {
            int result = base++;
            return result;
        }
    };


    class McastRecord : public ReusableRecord<int>
    {
    private:
        int base;
        int last;
        int basePort;

    public:
        McastRecord()
        {
            // FIXME: mcast records are never reused
            base = 0;
            basePort = 0;
            last = 0;
        }

        void setBasePort(int basePort)
        {
            this->basePort = basePort;
        }

        int fresh() override
        {
            int result = base;
            base++;
            return result;
        }

        std::string get()
        {
            int x = getFree();
            last = x;
            int v1 = x % 255;
            int v2 = x / 255;
            yCAssert(NAMESERVER, v2 < 255);
            return std::string("224.1.") + yarp::conf::numeric::to_string(v2 + 1) + "." + yarp::conf::numeric::to_string(v1 + 1);
        }

        int lastPortNumber()
        {
            return basePort + last;
        }

        void releaseAddress(const char* addr)
        {
            auto ss = yarp::conf::string::split(addr, '.');
            constexpr size_t ipv4_size = 4;
            int ip[] = {224, 3, 1, 1};
            yCAssert(NAMESERVER, ss.size() == ipv4_size);
            for (size_t i = 0; i < ipv4_size; ++i) {
                ip[i] = yarp::conf::numeric::from_string<int>(ss[i]);
            }

            int v2 = ip[2] - 1;
            int v1 = ip[3] - 1;
            int x = v2 * 255 + v1;
            yCInfo(NAMESERVER, "Releasing %s %d  %d:%d\n", addr, x, v2, v1);
            release(x);
        }
    };


    class PropertyRecord
    {
    private:
        std::vector<std::string> prop;

    public:
        PropertyRecord()
        {
        }

        void clear()
        {
            prop.clear();
        }

        void add(const std::string& p)
        {
            prop.push_back(p);
        }

        bool check(const std::string& p)
        {
            for (unsigned int i = 0; i < prop.size(); i++) {
                if (prop[i] == p) {
                    return true;
                }
            }
            return false;
        }

        std::string match(const std::string& str)
        {
            std::string base = "";
            bool needSpace = false;
            for (unsigned int i = 0; i < prop.size(); i++) {
                if (prop[i].find(str) == 0) {
                    if (needSpace) {
                        base += " ";
                    }
                    base += prop[i];
                    needSpace = true;
                }
            }
            return base;
        }

        std::string toString() const
        {
            std::string base = "";
            for (unsigned int i = 0; i < prop.size(); i++) {
                if (i > 0) {
                    base += " ";
                }
                base += prop[i];
            }
            return base;
        }
    };

    class NameRecord
    {
    private:
        bool reusablePort;
        bool reusableIp;
        std::map<std::string, PropertyRecord> propMap;
        Contact address;

    public:
        NameRecord() :
                address()
        {
            reusableIp = false;
            reusablePort = false;
        }

        NameRecord(const NameRecord& alt) :
                address()
        {
            YARP_UNUSED(alt);
            reusableIp = false;
            reusablePort = false;
        }

        bool isReusablePort()
        {
            return reusablePort;
        }

        bool isReusableIp()
        {
            return reusableIp;
        }

        void clear()
        {
            propMap.clear();
            address = Contact();
            reusableIp = false;
            reusablePort = false;
        }

        void setAddress(const Contact& address,
                        bool reusablePort = false,
                        bool reusableIp = false)
        {
            this->address = address;
            this->reusablePort = reusablePort;
            this->reusableIp = reusableIp;
        }

        Contact getAddress()
        {
            return address;
        }


        PropertyRecord* getPR(const std::string& key, bool create = true)
        {
            std::map<std::string, PropertyRecord>::iterator entry = propMap.find(key);
            if (entry == propMap.end()) {
                if (!create) {
                    return nullptr;
                }
                propMap[key] = PropertyRecord();
                entry = propMap.find(key);
            }
            yCAssert(NAMESERVER, entry != propMap.end());
            return &(entry->second);
        }

        void clearProp(const std::string& key)
        {
            getPR(key)->clear();
        }

        void addProp(const std::string& key, const std::string& val)
        {
            getPR(key)->add(val);
        }

        std::string getProp(const std::string& key)
        {
            PropertyRecord* rec = getPR(key, false);
            if (rec != nullptr) {
                return rec->toString();
            }
            return {};
        }

        bool checkProp(const std::string& key, const std::string& val)
        {
            PropertyRecord* rec = getPR(key, false);
            if (rec != nullptr) {
                return rec->check(val);
            }
            return false;
        }

        std::string matchProp(const std::string& key, const std::string& val)
        {
            PropertyRecord* rec = getPR(key, false);
            if (rec != nullptr) {
                return rec->match(val);
            }
            return {};
        }
    };


    std::string cmdRegister(int argc, char* argv[]);
    std::string cmdQuery(int argc, char* argv[]);
    std::string cmdUnregister(int argc, char* argv[]);
    std::string cmdAnnounce(int argc, char* argv[]);
    std::string cmdHelp(int argc, char* argv[]);
    std::string cmdSet(int argc, char* argv[]);
    std::string cmdGet(int argc, char* argv[]);
    std::string cmdCheck(int argc, char* argv[]);
    std::string cmdMatch(int argc, char* argv[]);
    std::string cmdList(int argc, char* argv[]);
    std::string cmdRoute(int argc, char* argv[]);
    std::string cmdGarbageCollect(int argc, char* argv[]);
    std::string cmdBot(int argc, char* argv[]);

    // making a more easy to parse interface
    yarp::os::Bottle ncmdList(int argc, char* argv[]);
    yarp::os::Bottle ncmdQuery(int argc, char* argv[]);
    yarp::os::Bottle ncmdVersion(int argc, char* argv[]);
    yarp::os::Bottle ncmdSet(int argc, char* argv[]);
    yarp::os::Bottle ncmdGet(int argc, char* argv[]);

    std::map<std::string, NameRecord> nameMap;
    std::map<std::string, HostRecord> hostMap;

    McastRecord mcastRecord;
    DisposableNameRecord tmpNames;

    NameRecord* getNameRecord(const std::string& name, bool create);

    NameRecord& getNameRecord(const std::string& name)
    {
        NameRecord* result = getNameRecord(name, true);
        yCAssert(NAMESERVER, result != nullptr);
        return *result;
    }

    HostRecord* getHostRecord(const std::string& name, bool create);

    HostRecord& getHostRecord(const std::string& name)
    {
        HostRecord* result = getHostRecord(name, true);
        yCAssert(NAMESERVER, result != nullptr);
        return *result;
    }

    Dispatcher<NameServer, std::string> dispatcher;
    Dispatcher<NameServer, yarp::os::Bottle> ndispatcher;

protected:
    std::string terminate(const std::string& str);

    int basePort;

private:
    std::mutex mutex;
};

} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_NAMESERVER_H
