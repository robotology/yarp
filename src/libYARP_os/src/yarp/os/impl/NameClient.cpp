/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/impl/NameClient.h>

#include <yarp/conf/environment.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/Carriers.h>
#include <yarp/os/NameStore.h>
#include <yarp/os/NetType.h>
#include <yarp/os/Network.h>
#include <yarp/os/Os.h>
#include <yarp/os/impl/FallbackNameClient.h>
#include <yarp/os/impl/LogComponent.h>
#include <yarp/os/impl/NameConfig.h>
#include <yarp/os/impl/NameServer.h>
#include <yarp/os/impl/TcpFace.h>

#include <cstdio>
#include <mutex>

using namespace yarp::os::impl;
using namespace yarp::os;

namespace {
YARP_OS_LOG_COMPONENT(NAMECLIENT, "yarp.os.impl.NameClient")

/*
  Old class for splitting string based on spaces
*/
class Params
{
private:
    static constexpr size_t MAX_ARG_CT =20;
    static constexpr size_t MAX_ARG_LEN = 256;

    int argc;
    const char* argv[MAX_ARG_CT];
    char buf[MAX_ARG_CT][MAX_ARG_LEN];

public:
    Params()
    {
        argc = 0;
        for (auto& i : argv) {
            i = nullptr;
        }
    }

    Params(const char* command)
    {
        apply(command);
    }

    int size()
    {
        return argc;
    }

    const char* get(int idx)
    {
        return buf[idx];
    }

    void apply(const char* command)
    {
        size_t at = 0;
        size_t sub_at = 0;
        unsigned int i;
        for (i = 0; i < strlen(command) + 1; i++) {
            if (at < MAX_ARG_CT) {
                char ch = command[i];
                if (ch >= 32 || ch == '\0' || ch == '\n') {
                    if (ch == ' ' || ch == '\n') {
                        ch = '\0';
                    }
                    if (sub_at < MAX_ARG_LEN) {
                        buf[at][sub_at] = ch;
                        sub_at++;
                    }
                }
                if (ch == '\0') {
                    if (sub_at > 1) {
                        at++;
                    }
                    sub_at = 0;
                }
            }
        }
        for (i = 0; i < MAX_ARG_CT; i++) {
            argv[i] = buf[i];
            buf[i][MAX_ARG_LEN - 1] = '\0';
        }

        argc = at;
    }
};
} // namespace


NameClient::NameClient() :
        fake(false),
        fakeServer(nullptr),
        allowScan(false),
        allowSaveScan(false),
        reportScan(false),
        reportSaveScan(false),
        isSetup(false),
        altStore(nullptr)
{
}

NameClient::~NameClient()
{
    if (fakeServer != nullptr) {
        delete fakeServer;
        fakeServer = nullptr;
    }
}

NameClient& NameClient::getNameClient()
{
    static NameClient instance;
    return instance;
}

NameClient* NameClient::create()
{
    return new NameClient();
}


Contact NameClient::getAddress()
{
    setup();
    return address;
}

Contact NameClient::queryName(const std::string& name)
{
    size_t i1 = name.find(':');
    if (i1 != std::string::npos) {
        Contact c = c.fromString(name);
        if (c.isValid() && c.getPort() > 0) {
            return c;
        }
    }

    if (altStore != nullptr) {
        Contact c = altStore->query(name);
        return c;
    }

    std::string q("NAME_SERVER query ");
    q += name;
    return probe(q);
}

Contact NameClient::registerName(const std::string& name)
{
    return registerName(name, Contact());
}

Contact NameClient::registerName(const std::string& name, const Contact& suggest)
{
    Bottle cmd;
    cmd.addString("register");
    if (!name.empty()) {
        cmd.addString(name);
    } else {
        cmd.addString("...");
    }
    std::string prefix = yarp::conf::environment::get_string("YARP_IP");
    const NestedContact& nc = suggest.getNested();
    std::string typ = nc.getTypeNameStar();
    if (suggest.isValid() || !prefix.empty() || typ != "*") {
        if (!suggest.getCarrier().empty()) {
            cmd.addString(suggest.getCarrier().c_str());
        } else {
            cmd.addString("...");
        }
        if (!suggest.getHost().empty()) {
            cmd.addString(suggest.getHost().c_str());
        } else {
            if (!prefix.empty()) {
                Bottle ips = NameConfig::getIpsAsBottle();
                for (size_t i = 0; i < ips.size(); i++) {
                    std::string ip = ips.get(i).asString();
                    if (ip.find(prefix) == 0) {
                        prefix = ip;
                        break;
                    }
                }
            }
            cmd.addString((!prefix.empty()) ? prefix : "...");
        }
        if (suggest.getPort() != 0) {
            cmd.addInt32(suggest.getPort());
        } else {
            cmd.addString("...");
        }
        if (typ != "*") {
            cmd.addString(typ);
        }
    } else {
        if (!suggest.getCarrier().empty()) {
            cmd.addString(suggest.getCarrier().c_str());
        }
    }
    Bottle reply;

    yCDebug(NAMECLIENT, "Sending command: %s", cmd.toString().c_str());
    send(cmd, reply);
    yCDebug(NAMECLIENT, "Received reply: %s", reply.toString().c_str());

    Contact address = extractAddress(reply);
    if (address.isValid()) {
        std::string reg = address.getRegName();


        std::string cmdOffers = "set /port offers ";
        yarp::os::Bottle lst = yarp::os::Carriers::listCarriers();
        for (size_t i = 0; i < lst.size(); i++) {
            cmdOffers.append(" ").append(lst.get(i).asString());
        }


        cmd.fromString(cmdOffers);
        cmd.get(1) = Value(reg);
        send(cmd, reply);

        // accept the same set of carriers
        cmd.get(2) = Value("accepts");
        send(cmd, reply);

        cmd.clear();
        cmd.addString("set");
        cmd.addString(reg.c_str());
        cmd.addString("ips");
        cmd.append(NameConfig::getIpsAsBottle());
        send(cmd, reply);

        cmd.clear();
        cmd.addString("set");
        cmd.addString(reg.c_str());
        cmd.addString("process");
        cmd.addInt32(yarp::os::getpid());
        send(cmd, reply);
    }
    return address;
}

Contact NameClient::unregisterName(const std::string& name)
{
    std::string q("NAME_SERVER unregister ");
    q += name;
    return probe(q);
}

Contact NameClient::probe(const std::string& cmd)
{
    std::string result = send(cmd);
    return extractAddress(result);
}

Contact NameClient::extractAddress(const std::string& txt)
{
    Params p(txt.c_str());
    if (p.size() >= 9) {
        // registration name /bozo ip 5.255.112.225 port 10002 type tcp
        if (std::string(p.get(0)) == "registration") {
            const char* regName = p.get(2);
            const char* ip = p.get(4);
            int port = atoi(p.get(6));
            const char* carrier = p.get(8);
            return Contact(regName, carrier, ip, port);
        }
    }
    return Contact();
}

Contact NameClient::extractAddress(const Bottle& bot)
{
    if (bot.size() >= 9) {
        if (bot.get(0).asString() == "registration") {
            return Contact(bot.get(2).asString(), // regname
                           bot.get(8).asString(), // carrier
                           bot.get(4).asString(), // ip
                           bot.get(6).asInt32()); // port number
        }
    }
    return Contact();
}

std::string NameClient::send(const std::string& cmd, bool multi, const ContactStyle& style)
{
    yCTrace(NAMECLIENT, "*** OLD YARP command %s", cmd.c_str());
    setup();

    if (NetworkBase::getQueryBypass() != nullptr) {
        ContactStyle style;
        Bottle bcmd(cmd);
        Bottle reply;
        NetworkBase::writeToNameServer(bcmd, reply, style);
        std::string si = reply.toString();
        std::string so;
        for (char i : si) {
            if (i != '\"') {
                so += i;
            }
        }
        return so;
    }
    bool retried = false;
    bool retry = false;
    std::string result;
    Contact server = getAddress();
    float timeout = 10;
    if (style.timeout > 0) {
        timeout = style.timeout;
    }
    server.setTimeout(timeout);

    do {

        yCDebug(NAMECLIENT, "sending to nameserver: %s", cmd.c_str());

        if (isFakeMode()) {
            yCDebug(NAMECLIENT, "fake mode nameserver");
            return getServer().apply(cmd, Contact("tcp", "127.0.0.1", NetworkBase::getDefaultPortRange())) + "\n";
        }

        TcpFace face;
        yCDebug(NAMECLIENT, "connecting to %s", getAddress().toURI().c_str());
        OutputProtocol* ip = nullptr;
        if (!retry) {
            ip = face.write(server);
        } else {
            retried = true;
        }
        if (ip == nullptr) {
            yCInfo(NAMECLIENT, "No connection to nameserver");
            if (!allowScan) {
                yCInfo(NAMECLIENT, "*** try running: yarp detect ***");
            }
            Contact alt;
            if (!isFakeMode()) {
                if (allowScan) {
                    yCInfo(NAMECLIENT, "no connection to nameserver, scanning mcast");
                    reportScan = true;
                    alt = FallbackNameClient::seek();
                }
            }
            if (alt.isValid()) {
                address = alt;
                if (allowSaveScan) {
                    reportSaveScan = true;
                    NameConfig nc;
                    nc.setAddress(alt);
                    nc.toFile();
                }
                server = getAddress();
                server.setTimeout(timeout);
                ip = face.write(server);
                if (ip == nullptr) {
                    yCError(NAMECLIENT, "no connection to nameserver, scanning mcast");
                    return {};
                }
            } else {
                return {};
            }
        }
        std::string cmdn = cmd + "\n";
        Bytes b((char*)cmdn.c_str(), cmdn.length());
        ip->getOutputStream().write(b);
        ip->getOutputStream().flush();
        bool more = multi;
        while (more) {
            std::string line;
            line = ip->getInputStream().readLine();
            if (!(ip->isOk())) {
                retry = true;
                break;
            }
            if (line.length() > 1) {
                if (line[0] == '*' || line[0] == '[') {
                    more = false;
                }
            }
            result += line + "\n";
        }
        ip->close();
        delete ip;
        yCDebug(NAMECLIENT, "<<< received from nameserver: %s", result.c_str());
    } while (retry && !retried);

    return result;
}

bool NameClient::send(Bottle& cmd, Bottle& reply)
{
    setup();
    if (NetworkBase::getQueryBypass() != nullptr) {
        ContactStyle style;
        NetworkBase::writeToNameServer(cmd, reply, style);
        return true;
    }
    if (isFakeMode()) {
        yCDebug(NAMECLIENT, "fake mode nameserver");
        return getServer().apply(cmd, reply, Contact("tcp", "127.0.0.1", NetworkBase::getDefaultPortRange()));
    }
    Contact server = getAddress();
    ContactStyle style;
    style.carrier = "name_ser";
    return NetworkBase::write(server, cmd, reply, style);
}

void NameClient::setFakeMode(bool fake)
{
    this->fake = fake;
}

bool NameClient::isFakeMode() const
{
    return fake;
}

void NameClient::setScan(bool allow)
{
    allowScan = allow;
}

void NameClient::setSave(bool allow)
{
    allowSaveScan = allow;
}

bool NameClient::didScan()
{
    return reportScan;
}

bool NameClient::didSave()
{
    return reportSaveScan;
}

bool NameClient::updateAddress()
{
    NameConfig conf;
    address = Contact();
    mode = "yarp";
    if (conf.fromFile()) {
        address = conf.getAddress();
        mode = conf.getMode();
        return true;
    }
    return false;
}

bool NameClient::setContact(const yarp::os::Contact& contact)
{
    if (!contact.isValid()) {
        fake = true;
    }
    address = contact;
    mode = "yarp";
    isSetup = true;
    return true;
}

void NameClient::queryBypass(NameStore* store)
{
    altStore = store;
}

NameStore* NameClient::getQueryBypass()
{
    return altStore;
}

std::string NameClient::getMode()
{
    return mode;
}

yarp::os::Nodes& NameClient::getNodes()
{
    return nodes;
}

NameServer& NameClient::getServer()
{
    if (fakeServer == nullptr) {
        fakeServer = new NameServer;
    }
    yCAssert(NAMECLIENT, fakeServer != nullptr);
    return *fakeServer;
}

void NameClient::setup()
{
    static std::mutex mutex;
    mutex.lock();
    if ((!fake) && (!isSetup)) {
        if (!updateAddress()) {
            yCError(NAMECLIENT, "Cannot find name server");
        }

        yCDebug(NAMECLIENT, "name server address is %s", address.toURI().c_str());
        isSetup = true;
    }
    mutex.unlock();
}
