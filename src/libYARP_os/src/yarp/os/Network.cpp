/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>

#include <yarp/conf/environment.h>
#include <yarp/conf/filesystem.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Carriers.h>
#include <yarp/os/Face.h>
#include <yarp/os/MultiNameSpace.h>
#include <yarp/os/NameSpace.h>
#include <yarp/os/NetType.h>
#include <yarp/os/OutputProtocol.h>
#include <yarp/os/Port.h>
#include <yarp/os/Route.h>
#include <yarp/os/Time.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/YarpPlugin.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/impl/LogComponent.h>
#include <yarp/os/impl/LogForwarder.h>
#include <yarp/os/impl/NameConfig.h>
#include <yarp/os/impl/PlatformSignal.h>
#include <yarp/os/impl/PlatformStdio.h>
#include <yarp/os/impl/PlatformUnistd.h>
#include <yarp/os/impl/PortCommand.h>
#include <yarp/os/impl/Terminal.h>
#include <yarp/os/impl/TimeImpl.h>

#ifdef YARP_HAS_ACE
#    include <ace/Init_ACE.h>
#    include <ace/config.h>
// In one the ACE headers there is a definition of "main" for WIN32
#    ifdef main
#        undef main
#    endif
#endif

#include <cstdio>
#include <cstdlib>
#include <mutex>
#include <string>

using namespace yarp::os::impl;
using namespace yarp::os;

namespace {
YARP_OS_LOG_COMPONENT(NETWORK, "yarp.os.Network")
}

static int __yarp_is_initialized = 0;
static bool __yarp_auto_init_active = false; // was yarp auto-initialized?


/**
 * A single-use class to shut down the yarp library if it was
 * initialized automatically.
 */
class YarpAutoInit
{
public:
    /**
     * Shut down the yarp library if it was automatically initialized.
     * The library is automatically initialized if
     * NetworkBase::autoInitMinimum() is called before any of the
     * manual ways of initializing the library (calling Network::init,
     * creating a Network object, etc).  yarp::os::ResourceFinder
     * calls autoInitMinimum() since it needs to be sure that
     * YARP+ACE is initialized (but a user might not expect that).
     */
    ~YarpAutoInit()
    {
        if (__yarp_auto_init_active) {
            NetworkBase::finiMinimum();
            __yarp_auto_init_active = false;
        }
    }
};
static YarpAutoInit yarp_auto_init; ///< destructor is called on shutdown.

static MultiNameSpace& getNameSpace()
{
    static MultiNameSpace __multi_name_space;
    return __multi_name_space;
}

static bool needsLookup(const Contact& contact)
{
    if (!contact.getHost().empty()) {
        return false;
    }
    if (contact.getCarrier() == "topic") {
        return false;
    }
    return true;
}

static int noteDud(const Contact& src)
{
    NameStore* store = getNameSpace().getQueryBypass();
    if (store != nullptr) {
        return store->announce(src.getName(), 0);
    }
    Bottle cmd;
    Bottle reply;
    cmd.addString("announce");
    cmd.addString(src.getName().c_str());
    cmd.addInt32(0);
    ContactStyle style;
    bool ok = NetworkBase::writeToNameServer(cmd,
                                             reply,
                                             style);
    return ok ? 0 : 1;
}


static int enactConnection(const Contact& src,
                           const Contact& dest,
                           const ContactStyle& style,
                           int mode,
                           bool reversed)
{
    ContactStyle rpc;
    rpc.admin = true;
    rpc.quiet = style.quiet;
    rpc.timeout = style.timeout;

    yCTrace(NETWORK,
            "enactConnection: SRC %s DST %s using carrier %s, MODE=%d, rev=%d",
            src.getName().c_str(),
            dest.getName().c_str(),
            style.carrier.c_str(),
            mode,
            reversed);

    if (style.persistent) {
        bool ok = false;
        // we don't talk to the ports, we talk to the nameserver
        NameSpace& ns = getNameSpace();
        if (mode == YARP_ENACT_CONNECT) {
            ok = ns.connectPortToPortPersistently(src, dest, style);
        } else if (mode == YARP_ENACT_DISCONNECT) {
            ok = ns.disconnectPortToPortPersistently(src, dest, style);
        } else {
            yCError(NETWORK, "Failure: cannot check subscriptions yet");
            return 1;
        }
        if (!ok) {
            return 1;
        }
        if (!style.quiet) {
            yCInfo(NETWORK, "Success: port-to-port persistent connection added.");
        }
        return 0;
    }

    Bottle cmd;
    Bottle reply;
    cmd.addVocab32("list");
    cmd.addVocab32(reversed ? "in" : "out");
    cmd.addString(dest.getName().c_str());
    yCDebug(NETWORK, "asking %s: %s", src.toString().c_str(), cmd.toString().c_str());
    bool ok = NetworkBase::write(src, cmd, reply, rpc);
    if (!ok) {
        noteDud(src);
        return 1;
    }
    if (reply.check("carrier")) {
        std::string carrier = reply.find("carrier").asString();
        if (!style.quiet) {
            yCInfo(NETWORK,
                   "Connection found between %s and %s using carrier %s",
                   src.getName().c_str(),
                   dest.getName().c_str(),
                   carrier.c_str());
        }
        if (mode == YARP_ENACT_EXISTS) {
            return (carrier == style.carrier) ? 0 : 1;
        }

        // This is either a connect or a disconnect command, but the current
        // connection is connectionless, the other side will not know that we
        // are closing the connection and therefore will continue sending data.
        // Therefore we send an explicit disconnect here.
        bool currentIsConnectionLess = false;
        bool currentIsPush = true;
        if (reply.check("push")) {
            currentIsPush = reply.find("push").asBool();
        }
        if (reply.check("connectionless")) {
            currentIsConnectionLess = reply.find("connectionless").asBool();
        }
        if (currentIsConnectionLess && ((reversed && currentIsPush) || (!reversed && !currentIsPush))) {
            enactConnection(dest, src, style, YARP_ENACT_DISCONNECT, !reversed);
        }
    }
    if (mode == YARP_ENACT_EXISTS) {
        return 1;
    }

    yarp::conf::vocab32_t act = (mode == YARP_ENACT_DISCONNECT) ? yarp::os::createVocab32('d', 'e', 'l') : yarp::os::createVocab32('a', 'd', 'd');

    // Let's ask the destination to connect/disconnect to the source.
    // We assume the YARP carrier will reverse the connection if
    // appropriate when connecting.
    cmd.clear();
    reply.clear();
    cmd.addVocab32(act);
    Contact c = dest;
    if (!style.carrier.empty()) {
        c.setCarrier(style.carrier);
    }
    if (mode != YARP_ENACT_DISCONNECT) {
        cmd.addString(c.toString());
    } else {
        cmd.addString(c.getName());
    }

    Contact c2 = src;
    if (c2.getPort() <= 0) {
        c2 = NetworkBase::queryName(c2.getName());
    }

    yCDebug(NETWORK, "** asking %s: %s", src.toString().c_str(), cmd.toString().c_str());
    ok = NetworkBase::write(c2, cmd, reply, rpc);
    if (!ok) {
        noteDud(src);
        return 1;
    }
    std::string msg;
    if (reply.get(0).isInt32()) {
        ok = (reply.get(0).asInt32() == 0);
        msg = reply.get(1).asString();
    } else {
        // older protocol
        // FIXME Is this protocol still in use?
        msg = reply.get(0).asString();
        ok = msg[0] == 'A' || msg[0] == 'R';
    }
    if (mode == YARP_ENACT_DISCONNECT && !ok) {
        msg = "no such connection";
    }
    if (mode == YARP_ENACT_CONNECT && !ok) {
        noteDud(dest);
    }
    if (!style.quiet) {
        if (!ok) {
            yCError(NETWORK, "%s %s", "Failure:", msg.c_str());
        } else if (style.verboseOnSuccess) {
            yCInfo(NETWORK,  "%s %s", "Success:", msg.c_str());
        }
    }
    return ok ? 0 : 1;
}

static std::string collectParams(Contact& c)
{
    std::string carrier_name = c.getCarrier();
    auto pos = carrier_name.find('+');
    if (pos != std::string::npos) {
        return carrier_name.substr(pos);
    }
    return {};
}

static std::string extractCarrierNameOnly(const std::string& carrier_name_with_params)
{
    return carrier_name_with_params.substr(0, carrier_name_with_params.find('+'));
}

/*

   Connect two ports, bearing in mind that one of them may not be
   a regular YARP port.

   Normally, YARP sends a request to the source port asking it to
   connect to the destination port.  But the source port may not
   be capable of initiating connections, in which case we can
   request the destination port to connect to the source (this
   is appropriate for carriers that can reverse the initiative).

   The source or destination could also be topic ports, which are
   entirely virtual.  In that case, we just need to tell the name
   server, and it will take care of the details.

*/

static int metaConnect(const std::string& src,
                       const std::string& dest,
                       ContactStyle style,
                       int mode)
{
    yCTrace(NETWORK,
            "working on connection %s to %s (%s)",
            src.c_str(),
            dest.c_str(),
            (mode == YARP_ENACT_CONNECT) ? "connect" : ((mode == YARP_ENACT_DISCONNECT) ? "disconnect" : "check"));
    // check if source name and destination name contain spaces
    if (dest.find(' ') != std::string::npos || src.find(' ') != std::string::npos) {
        yCError(NETWORK,
                "Failure: no way to make connection %s->%s, one of the port names contains a space character.",
                src.c_str(),
                dest.c_str());
        return 1;
    }

    yCTrace(NETWORK,
            "METACONNECT: src=%s dest=%s style=%s",
            src.c_str(),
            dest.c_str(),
            style.carrier.c_str());

    // get the expressed contacts, without name server input
    Contact dynamicSrc = Contact::fromString(src);
    Contact dynamicDest = Contact::fromString(dest);

    yCTrace(NETWORK,
            "DYNAMIC_SRC: name=%s, carrier=%s",
            dynamicSrc.getName().c_str(),
            dynamicSrc.getCarrier().c_str());
    yCTrace(NETWORK,
            "DYNAMIC_DST: name=%s, carrier=%s",
            dynamicDest.getName().c_str(),
            dynamicDest.getCarrier().c_str());

    if (!NetworkBase::isValidPortName(dynamicSrc.getName())) {
        yCError(NETWORK,
                "Failure: no way to make connection, invalid source '%s'",
                dynamicSrc.getName().c_str());
        return 1;
    }
    if (!NetworkBase::isValidPortName(dynamicDest.getName())) {
        yCError(NETWORK,
                "Failure: no way to make connection, invalid destination '%s'",
                dynamicDest.getName().c_str());
        return 1;
    }

    bool topical = style.persistent;
    if (dynamicSrc.getCarrier() == "topic" || dynamicDest.getCarrier() == "topic") {
        topical = true;
    }

    bool topicalNeedsLookup = !getNameSpace().connectionHasNameOfEndpoints();

    // fetch completed contacts from name server, if needed
    Contact staticSrc;
    Contact staticDest;
    if (needsLookup(dynamicSrc) && (topicalNeedsLookup || !topical)) {
        staticSrc = NetworkBase::queryName(dynamicSrc.getName());
        if (!staticSrc.isValid()) {
            if (!style.persistent) {
                if (!style.quiet) {
                    yCError(NETWORK,
                            "Failure: could not find source port %s",
                            src.c_str());
                }
                return 1;
            }
            staticSrc = dynamicSrc;
        }
    } else {
        staticSrc = dynamicSrc;
    }
    if (staticSrc.getCarrier().empty()) {
        staticSrc.setCarrier("tcp");
    }
    if (staticDest.getCarrier().empty()) {
        staticDest.setCarrier("tcp");
    }

    if (needsLookup(dynamicDest) && (topicalNeedsLookup || !topical)) {
        staticDest = NetworkBase::queryName(dynamicDest.getName());
        if (!staticDest.isValid()) {
            if (!style.persistent) {
                if (!style.quiet) {
                    yCError(NETWORK,
                            "Failure: could not find destination port %s",
                            dest.c_str());
                }
                return 1;
            }
            staticDest = dynamicDest;
        }
    } else {
        staticDest = dynamicDest;
    }

    yCTrace(NETWORK,
            "STATIC_SRC: name=%s, carrier=%s",
            staticSrc.getName().c_str(),
            staticSrc.getCarrier().c_str());
    yCTrace(NETWORK,
            "STATIC_DST: name=%s, carrier=%s",
            staticDest.getName().c_str(),
            staticDest.getCarrier().c_str());

    //DynamicSrc and DynamicDst are the contacts created by connect command
    //while staticSrc and staticDest are contacts created by querying th server

    if (staticSrc.getCarrier() == "xmlrpc" && (staticDest.getCarrier() == "xmlrpc" || (staticDest.getCarrier().find("rossrv") == 0)) && mode == YARP_ENACT_CONNECT) {
        // Unconnectable in general
        // Let's assume the first part is a YARP port, and use "tcp" instead
        staticSrc.setCarrier("tcp");
        staticDest.setCarrier("tcp");
    }

    std::string carrierConstraint;

    // see if we can do business with the source port
    bool srcIsCompetent = false;
    bool srcIsTopic = false;
    if (staticSrc.getCarrier() != "topic") {
        if (!topical) {
            Carrier* srcCarrier = nullptr;
            yCTrace(NETWORK,
                    "staticSrc.getCarrier = %s",
                    staticSrc.getCarrier().c_str());
            if (!staticSrc.getCarrier().empty()) {
                srcCarrier = Carriers::chooseCarrier(staticSrc.getCarrier());
            }
            if (srcCarrier != nullptr) {
                yCTrace(NETWORK,
                        "srcCarrier is NOT null; its name is %s",
                        srcCarrier->getName().c_str());
                std::string srcBootstrap = srcCarrier->getBootstrapCarrierName();
                if (!srcBootstrap.empty()) {

                    yCTrace(NETWORK,
                            "it is competent (bootstrapname is %s), while its name is %s",
                            srcBootstrap.c_str(),
                            srcCarrier->getName().c_str());
                    srcIsCompetent = true;
                } else {
                    //if the srcCarrier is not competent, (that is it can't perform the starting yarp handshaking)
                    //set the carrier constraint equal to the carrier with which the posrt had been registered.
                    carrierConstraint = staticSrc.getCarrier();
                    yCTrace(NETWORK,
                            "it is NOT competent. its constraint is %s",
                            carrierConstraint.c_str());
                }
                delete srcCarrier;
                srcCarrier = nullptr;
            }
        }
    } else {
        srcIsTopic = true;
    }

    // see if we can do business with the destination port
    bool destIsCompetent = false;
    bool destIsTopic = false;
    if (staticDest.getCarrier() != "topic") {
        if (!topical) {
            Carrier* destCarrier = nullptr;
            yCTrace(NETWORK,
                    "staticDest.getCarrier = %s",
                    staticDest.getCarrier().c_str());
            if (!staticDest.getCarrier().empty()) {
                destCarrier = Carriers::chooseCarrier(staticDest.getCarrier());
            }
            if (destCarrier != nullptr) {
                yCTrace(NETWORK,
                        "destCarrier is NOT null; its name is %s",
                        destCarrier->getName().c_str());
                std::string destBootstrap = destCarrier->getBootstrapCarrierName();
                if (!destBootstrap.empty()) {
                    yCTrace(NETWORK,
                            "it is competent (bootstrapname is %s), while its name is %s",
                            destBootstrap.c_str(),
                            destCarrier->getName().c_str());
                    destIsCompetent = true;
                } else {
                    //if the destCarrier is not competent, (that is it can't perform the starting yarp handshaking)
                    //set the carrier constraint equal to the carrier with which the posrt had been registered.
                    carrierConstraint = staticDest.getCarrier();
                    yCTrace(NETWORK,
                            "it is NOT competent. its constraint is %s",
                            carrierConstraint.c_str());
                }
                delete destCarrier;
                destCarrier = nullptr;
            }
        }
    } else {
        destIsTopic = true;
    }

    if (srcIsTopic || destIsTopic) {
        Bottle cmd;
        Bottle reply;
        NameSpace& ns = getNameSpace();

        bool ok = false;
        if (srcIsTopic) {
            if (mode == YARP_ENACT_CONNECT) {
                ok = ns.connectTopicToPort(staticSrc, staticDest, style);
            } else if (mode == YARP_ENACT_DISCONNECT) {
                ok = ns.disconnectTopicFromPort(staticSrc, staticDest, style);
            } else {
                yCError(NETWORK, "Failure: cannot check subscriptions yet");
                return 1;
            }
        } else {
            if (mode == YARP_ENACT_CONNECT) {
                ok = ns.connectPortToTopic(staticSrc, staticDest, style);
            } else if (mode == YARP_ENACT_DISCONNECT) {
                ok = ns.disconnectPortFromTopic(staticSrc, staticDest, style);
            } else {
                yCError(NETWORK, "Failure: cannot check subscriptions yet");
                return 1;
            }
        }
        if (!ok) {
            return 1;
        }
        if (!style.quiet) {
            if (style.verboseOnSuccess) {
                yCInfo(NETWORK, "Success: connection to topic %s.", mode == YARP_ENACT_CONNECT ? "added" : "removed");
            }
        }
        return 0;
    }

    yCTrace(NETWORK,
            "dynamicSrc.getCarrier() = %s",
            dynamicSrc.getCarrier().c_str());
    yCTrace(NETWORK,
            "dynamicDest.getCarrier() = %s",
            dynamicDest.getCarrier().c_str());
    yCTrace(NETWORK,
            "staticSrc.getCarrier() = %s",
            staticSrc.getCarrier().c_str());
    yCTrace(NETWORK,
            "staticDest.getCarrier() = %s",
            staticDest.getCarrier().c_str());
    yCTrace(NETWORK,
            "carrierConstraint is %s",
            carrierConstraint.c_str());

    yCTrace(NETWORK,
            "style.carrier (1) is %s",
            style.carrier.c_str());


    if (!dynamicSrc.getCarrier().empty()) { //if in connect command the user specified the carrier of src port
        style.carrier = dynamicSrc.getCarrier();
        yCTrace(NETWORK,
                "style.carrier is %s ==> in connect command the user specified the carrier of src port",
                style.carrier.c_str());
    }

    if (!dynamicDest.getCarrier().empty()) { //if in connect command the user specified the carrier of dest port or the carrier of the connection
        style.carrier = dynamicDest.getCarrier();
        yCTrace(NETWORK,
                "style.carrier is %s ==> in connect command the user specified the carrier of dest port or the carrier of the connection",
                style.carrier.c_str());
    }

    yCTrace(NETWORK,
            "at the end style style.carrier is %s",
            style.carrier.c_str());

    //here we'll check if the style carrier and the constraint carrier are equal.
    //note that in both string may contain params of carrier, so we need to comapare only the name of carrier.
    if (!style.carrier.empty() && !carrierConstraint.empty()) {
        //get only carrier name of style.
        std::string style_carrier_name = extractCarrierNameOnly(style.carrier);

        //get only carrier name of carrierConstraint.
        std::string carrier_constraint_name = extractCarrierNameOnly(carrierConstraint);

        if (style_carrier_name != carrier_constraint_name) {
            yCError(NETWORK, "Failure: conflict between %s and %s", style_carrier_name.c_str(), carrier_constraint_name.c_str());
            return 1;
        }
        yCTrace(NETWORK,
                "style_carrier_name=%s and carrier_constraint_name=%s are equals!",
                style_carrier_name.c_str(),
                carrier_constraint_name.c_str());
    }
    //we are going to choose the carrier of this connection, and we collect parameters specified by user
    //in order to pass them to the carrier, so it can configure itself.
    if (!carrierConstraint.empty()) {
        style.carrier = carrierConstraint;
        //if I'm here means that sorce or dest is not competent.
        //so we need to get parameters of carrier given in connect command.
        yCTrace(NETWORK,
                "if I'm here means that source or dest is not competent");
        std::string c = dynamicSrc.getCarrier();
        if (extractCarrierNameOnly(c) == extractCarrierNameOnly(style.carrier)) {
            style.carrier += collectParams(dynamicSrc);
        }
        c = dynamicDest.getCarrier();
        if (extractCarrierNameOnly(c) == extractCarrierNameOnly(style.carrier)) {
            style.carrier += collectParams(dynamicDest);
        }
    }
    if (style.carrier.empty()) {
        style.carrier = staticDest.getCarrier();
        //if I'm here means that both src and dest are copentent and the user didn't specified a carrier in the connect command
        yCTrace(NETWORK,
                "if I'm here means that both src and dest are compentent and the user didn't specified a carrier in the connect command");
        std::string c = dynamicSrc.getCarrier();
        if (extractCarrierNameOnly(c) == extractCarrierNameOnly(style.carrier)) {
            style.carrier += collectParams(staticSrc);
        }
    }

    if (style.carrier.empty()) {
        style.carrier = staticSrc.getCarrier();
        yCTrace(NETWORK, "the chosen style carrier is static src");
    }

    //now stylecarrier contains the carrier chosen for this connection

    yCTrace(NETWORK,
            "style_carrier with params  =%s",
            style.carrier.c_str());

    bool connectionIsPush = false;
    bool connectionIsPull = false;
    Carrier* connectionCarrier = nullptr;
    if (style.carrier != "topic") {
        connectionCarrier = Carriers::chooseCarrier(style.carrier);
        if (connectionCarrier != nullptr) {
            connectionIsPush = connectionCarrier->isPush();
            connectionIsPull = !connectionIsPush;
        }
    }

    int result = -1;
    if ((srcIsCompetent && connectionIsPush) || topical) {
        // Classic case.
        Contact c = Contact::fromString(dest);
        delete connectionCarrier;
        return enactConnection(staticSrc, c, style, mode, false);
    }
    if (destIsCompetent && connectionIsPull) {
        Contact c = Contact::fromString(src);
        delete connectionCarrier;
        return enactConnection(staticDest, c, style, mode, true);
    }

    if (connectionCarrier != nullptr) {
        if (!connectionIsPull) {
            Contact c = Contact::fromString(dest);
            result = connectionCarrier->connect(staticSrc, c, style, mode, false);
        } else {
            Contact c = Contact::fromString(src);
            result = connectionCarrier->connect(staticDest, c, style, mode, true);
        }
    }
    if (connectionCarrier != nullptr) {
        delete connectionCarrier;
        connectionCarrier = nullptr;
    }
    if (result != -1) {
        if (!style.quiet) {
            if (result == 0) {
                if (style.verboseOnSuccess) {
                    yCInfo(NETWORK, "Success: added connection using custom carrier method");
                }
            } else {
                yCError(NETWORK, "Failure: custom carrier method did not work");
            }
        }
        return result;
    }

    if (mode != YARP_ENACT_DISCONNECT) {
        yCError(NETWORK, "Failure: no way to make connection %s->%s", src.c_str(), dest.c_str());
    }

    return 1;
}

bool NetworkBase::connect(const std::string& src, const std::string& dest, const std::string& carrier, bool quiet)
{
    ContactStyle style;
    style.quiet = quiet;
    if (!carrier.empty()) {
        style.carrier = carrier;
    }
    return connect(src, dest, style);
}

bool NetworkBase::connect(const std::string& src,
                          const std::string& dest,
                          const ContactStyle& style)
{
    int result = metaConnect(src, dest, style, YARP_ENACT_CONNECT);
    return result == 0;
}

bool NetworkBase::disconnect(const std::string& src,
                             const std::string& dest,
                             bool quiet)
{
    ContactStyle style;
    style.quiet = quiet;
    return disconnect(src, dest, style);
}

bool NetworkBase::disconnect(const std::string& src,
                             const std::string& dest,
                             const ContactStyle& style)
{
    int result = metaConnect(src, dest, style, YARP_ENACT_DISCONNECT);
    return result == 0;
}

bool NetworkBase::disconnect(const std::string& src, const std::string& dest, const std::string& carrier, bool quiet)
{
    ContactStyle style;
    style.quiet = quiet;
    if (!carrier.empty()) {
        style.carrier = carrier;
    }
    return disconnect(src, dest, style);
}

bool NetworkBase::isConnected(const std::string& src,
                              const std::string& dest,
                              bool quiet)
{
    ContactStyle style;
    style.quiet = quiet;
    return isConnected(src, dest, style);
}

bool NetworkBase::isConnected(const std::string& src, const std::string& dest, const std::string& carrier, bool quiet)
{
    ContactStyle style;
    style.quiet = quiet;
    if (!carrier.empty()) {
        style.carrier = carrier;
    }
    return isConnected(src, dest, style);
}

bool NetworkBase::exists(const std::string& port, bool quiet, bool checkVer)
{
    ContactStyle style;
    style.quiet = quiet;
    return exists(port, style, checkVer);
}

bool NetworkBase::exists(const std::string& port, const ContactStyle& style, bool checkVer)
{
    bool silent = style.quiet;
    Contact address = NetworkBase::queryName(port);
    if (!address.isValid()) {
        if (!silent) {
            yCInfo(NETWORK, "Address of port %s is not valid", port.c_str());
        }
        return false;
    }

    Contact address2(address);
    if (style.timeout >= 0) {
        address2.setTimeout((float)style.timeout);
    }
    OutputProtocol* out = Carriers::connect(address2);

    if (out == nullptr) {
        if (!silent) {
            yCInfo(NETWORK, "Cannot connect to port %s", port.c_str());
        }
        return false;
    }
    out->close();
    delete out;
    out = nullptr;

    if (!checkVer) {
        return true;
    }

    ContactStyle style2 = style;
    style2.admin = true;
    Bottle cmd("[ver]");
    Bottle resp;
    bool ok = NetworkBase::write(Contact(port), cmd, resp, style2);
    if (!ok) {
        return false;
    }
    if (resp.get(0).toString() != "ver" && resp.get(0).toString() != "dict") {
        // YARP nameserver responds with a version
        // ROS nameserver responds with a dictionary of error data
        // Treat everything else an unknown
        return false;
    }

    return true;
}


bool NetworkBase::waitConnection(const std::string& source, const std::string& destination, bool quiet)
{
    int ct = 1;
    while (true) {

        if (ct % 30 == 1) {
            if (!quiet) {
                yCInfo(NETWORK, "Waiting for %s->%s...", source.c_str(), destination.c_str());
            }
        }
        ct++;

        int result = NetworkBase::isConnected(source, destination, true) ? 0 : 1;
        if (result != 0) {
            SystemClock::delaySystem(0.1);
        } else {
            return true;
        }
    }
}


bool NetworkBase::waitPort(const std::string& target, bool quiet)
{
    int ct = 1;
    while (true) {

        if (ct % 30 == 1) {
            if (!quiet) {
                yCInfo(NETWORK, "Waiting for %s...", target.c_str());
            }
        }
        ct++;

        bool result = exists(target, true, false);
        if (!result) {
            SystemClock::delaySystem(0.1);
        } else {
            return true;
        }
    }
}


bool NetworkBase::sync(const std::string& port, bool quiet)
{
    bool result = waitPort(port, quiet);
    if (result) {
        poll(port, true);
    }
    return result;
}


void NetworkBase::autoInitMinimum()
{
    autoInitMinimum(YARP_CLOCK_DEFAULT);
}

void NetworkBase::autoInitMinimum(yarp::os::yarpClockType clockType, yarp::os::Clock* custom)
{
    YARP_UNUSED(custom);
    if (!(__yarp_auto_init_active || (__yarp_is_initialized != 0))) {
        __yarp_auto_init_active = true;
        initMinimum(clockType);
    }
}

void NetworkBase::initMinimum()
{
    initMinimum(YARP_CLOCK_DEFAULT);
}

bool NetworkBase::isNetworkInitialized()
{
    return (__yarp_is_initialized > 0 && yarp::os::Time::isClockInitialized());
}

#if defined(YARP_HAS_ACE)
namespace {
class YARP_ACE
{
private:
    YARP_ACE()
    {
        ACE::init();
    }

public:
    ~YARP_ACE()
    {
        ACE::fini();
    }

    static YARP_ACE& init()
    {
        static YARP_ACE ace;
        return ace;
    }
};
} // namespace
#endif


void NetworkBase::initMinimum(yarp::os::yarpClockType clockType, yarp::os::Clock* custom)
{
    YARP_UNUSED(custom);
    if (__yarp_is_initialized == 0) {
        // Broken pipes need to be dealt with through other means
        yarp::os::impl::signal(SIGPIPE, SIG_IGN);

#ifdef YARP_HAS_ACE
        YARP_ACE::init();
#endif

        // make sure system is actually able to do things fast
        yarp::os::impl::Time::startTurboBoost();

        // MultiNameSpace is a c++11 singleton and need to be initialized
        // before the first port that is opened and it has to exist until
        // the last port is closed.
        getNameSpace();

        // Increment counter before initializing the clock.
        __yarp_is_initialized++;

        // The network clock open a port, and inside the open of the port
        // the __yarp_is_initialized counter is checked > 0.
        if (yarp::os::Time::getClockType() == YARP_CLOCK_UNINITIALIZED) {
            NetworkBase::yarpClockInit(clockType, nullptr);
        }
    } else {
        __yarp_is_initialized++;
    }
}

void NetworkBase::finiMinimum()
{
    if (__yarp_is_initialized == 1) {
        // The log forwarder needs to be shut down in order to close the
        // internal port. The shutdown method will do nothing if the
        // LogForwarded was not used.
        yarp::os::impl::LogForwarder::shutdown();

        Time::useSystemClock();
        yarp::os::impl::Time::removeClock();

        // reset system timer resolution
        yarp::os::impl::Time::endTurboBoost();
    }
    if (__yarp_is_initialized > 0) {
        __yarp_is_initialized--;
    }
}

void yarp::os::NetworkBase::yarpClockInit(yarp::os::yarpClockType clockType, Clock* custom)
{
    std::string clock;
    if (clockType == YARP_CLOCK_DEFAULT) {
        clock = yarp::conf::environment::get_string("YARP_CLOCK");
        if (!clock.empty()) {
            clockType = YARP_CLOCK_NETWORK;
        } else {
            clockType = YARP_CLOCK_SYSTEM;
        }
    }

    switch (clockType) {
    case YARP_CLOCK_SYSTEM:
        yCDebug(NETWORK, "Using SYSTEM clock");
        yarp::os::Time::useSystemClock();
        break;

    case YARP_CLOCK_NETWORK:
        yCDebug(NETWORK, "Using NETWORK clock");
        clock = yarp::conf::environment::get_string("YARP_CLOCK");
        // check of valid parameter is done inside the call, throws YARP_FAIL in case of error
        yarp::os::Time::useNetworkClock(clock);
        break;

    case YARP_CLOCK_CUSTOM: {
        yCDebug(NETWORK, "Using CUSTOM clock");
        // check of valid parameter is done inside the call, throws YARP_FAIL in case of error
        yarp::os::Time::useCustomClock(custom);
    } break;

    default:
        yCFatal(NETWORK, "yarpClockInit called with unknown clock type. Quitting");
        break;
    }
}

Contact NetworkBase::queryName(const std::string& name)
{
    yCDebug(NETWORK, "query name %s", name.c_str());
    if (getNameServerName() == name) {
        yCDebug(NETWORK, "query recognized as name server: %s", name.c_str());
        return getNameServerContact();
    }
    Contact c = c.fromString(name);
    if (c.isValid() && c.getPort() > 0) {
        return c;
    }
    return getNameSpace().queryName(name);
}


Contact NetworkBase::registerName(const std::string& name)
{
    yCDebug(NETWORK, "register name %s", name.c_str());
    return getNameSpace().registerName(name);
}


Contact NetworkBase::registerContact(const Contact& contact)
{
    yCDebug(NETWORK, "register contact %s", contact.toString().c_str());
    return getNameSpace().registerContact(contact);
}

Contact NetworkBase::unregisterName(const std::string& name)
{
    return getNameSpace().unregisterName(name);
}


Contact NetworkBase::unregisterContact(const Contact& contact)
{
    return getNameSpace().unregisterContact(contact);
}


bool NetworkBase::setProperty(const char* name,
                              const char* key,
                              const Value& value)
{
    return getNameSpace().setProperty(name, key, value);
}


Value* NetworkBase::getProperty(const char* name, const char* key)
{
    return getNameSpace().getProperty(name, key);
}


bool NetworkBase::setLocalMode(bool flag)
{
    return getNameSpace().setLocalMode(flag);
}

bool NetworkBase::getLocalMode()
{
    NameSpace& ns = getNameSpace();
    return ns.localOnly();
}

void NetworkBase::assertion(bool shouldBeTrue)
{
    // could replace with ACE assertions, except should not
    // evaporate in release mode
    yCAssert(NETWORK, shouldBeTrue);
}

#ifndef YARP_NO_DEPRECATED // Since YARP 3.0.0
std::string NetworkBase::readString(bool* eof)
{
    return yarp::os::impl::Terminal::readString(eof);
}
#endif // YARP_NO_DEPRECATED

bool NetworkBase::setConnectionQos(const std::string& src, const std::string& dest, const QosStyle& style, bool quiet)
{
    return setConnectionQos(src, dest, style, style, quiet);
}

bool NetworkBase::setConnectionQos(const std::string& src, const std::string& dest, const QosStyle& srcStyle, const QosStyle& destStyle, bool quiet)
{

    //e.g.,  prop set /portname (sched ((priority 30) (policy 1))) (qos ((tos 0)))
    yarp::os::Bottle cmd;
    yarp::os::Bottle reply;

    // ignore if everything left as default
    if (srcStyle.getPacketPriorityAsTOS() != -1 || srcStyle.getThreadPolicy() != -1) {
        // set the source Qos
        cmd.addString("prop");
        cmd.addString("set");
        cmd.addString(dest.c_str());
        Bottle& sched = cmd.addList();
        sched.addString("sched");
        Property& sched_prop = sched.addDict();
        sched_prop.put("priority", srcStyle.getThreadPriority());
        sched_prop.put("policy", srcStyle.getThreadPolicy());
        Bottle& qos = cmd.addList();
        qos.addString("qos");
        Property& qos_prop = qos.addDict();
        qos_prop.put("tos", srcStyle.getPacketPriorityAsTOS());
        Contact srcCon = Contact::fromString(src);
        bool ret = write(srcCon, cmd, reply, true, true, 2.0);
        if (!ret) {
            if (!quiet) {
                yCError(NETWORK, "Cannot write to '%s'", src.c_str());
            }
            return false;
        }
        if (reply.get(0).asString() != "ok") {
            if (!quiet) {
                yCError(NETWORK, "Cannot set qos properties of '%s'. (%s)", src.c_str(), reply.toString().c_str());
            }
            return false;
        }
    }

    // ignore if everything left as default
    if (destStyle.getPacketPriorityAsTOS() != -1 || destStyle.getThreadPolicy() != -1) {
        // set the destination Qos
        cmd.clear();
        reply.clear();
        cmd.addString("prop");
        cmd.addString("set");
        cmd.addString(src.c_str());
        Bottle& sched2 = cmd.addList();
        sched2.addString("sched");
        Property& sched_prop2 = sched2.addDict();
        sched_prop2.put("priority", destStyle.getThreadPriority());
        sched_prop2.put("policy", destStyle.getThreadPolicy());
        Bottle& qos2 = cmd.addList();
        qos2.addString("qos");
        Property& qos_prop2 = qos2.addDict();
        qos_prop2.put("tos", destStyle.getPacketPriorityAsTOS());
        Contact destCon = Contact::fromString(dest);
        bool ret = write(destCon, cmd, reply, true, true, 2.0);
        if (!ret) {
            if (!quiet) {
                yCError(NETWORK, "Cannot write to '%s'", dest.c_str());
            }
            return false;
        }
        if (reply.get(0).asString() != "ok") {
            if (!quiet) {
                yCError(NETWORK, "Cannot set qos properties of '%s'. (%s)", dest.c_str(), reply.toString().c_str());
            }
            return false;
        }
    }
    return true;
}

static bool getPortQos(const std::string& port, const std::string& unit, QosStyle& style, bool quiet)
{
    // request: "prop get /portname"
    // reply  : "(sched ((priority 30) (policy 1))) (qos ((priority HIGH)))"
    yarp::os::Bottle cmd;
    yarp::os::Bottle reply;

    // set the source Qos
    cmd.addString("prop");
    cmd.addString("get");
    cmd.addString(unit.c_str());
    Contact portCon = Contact::fromString(port);
    bool ret = NetworkBase::write(portCon, cmd, reply, true, true, 2.0);
    if (!ret) {
        if (!quiet) {
            yCError(NETWORK, "Cannot write to '%s'", port.c_str());
        }
        return false;
    }
    if (reply.size() == 0 || reply.get(0).asString() == "fail") {
        if (!quiet) {
            yCError(NETWORK, "Cannot get qos properties of '%s'. (%s)", port.c_str(), reply.toString().c_str());
        }
        return false;
    }

    Bottle& sched = reply.findGroup("sched");
    Bottle* sched_prop = sched.find("sched").asList();
    style.setThreadPriority(sched_prop->find("priority").asInt32());
    style.setThreadPolicy(sched_prop->find("policy").asInt32());
    Bottle& qos = reply.findGroup("qos");
    Bottle* qos_prop = qos.find("qos").asList();
    style.setPacketPrioritybyTOS(qos_prop->find("tos").asInt32());

    return true;
}

bool NetworkBase::getConnectionQos(const std::string& src, const std::string& dest, QosStyle& srcStyle, QosStyle& destStyle, bool quiet)
{
    if (!getPortQos(src, dest, srcStyle, quiet)) {
        return false;
    }
    if (!getPortQos(dest, src, destStyle, quiet)) {
        return false;
    }
    return true;
}

bool NetworkBase::isValidPortName(const std::string& portName)
{
    if (portName.empty()) {
        return false;
    }

    if (portName == "...") {
        return true;
    }

    if (portName.at(0) != '/') {
        return false;
    }

    if (portName.at(portName.size() - 1) == '/') {
        return false;
    }

    if (portName.find(' ') != std::string::npos) {
        return false;
    }

    return true;
}


bool NetworkBase::write(const Contact& contact,
                        PortWriter& cmd,
                        PortReader& reply,
                        bool admin,
                        bool quiet,
                        double timeout)
{
    ContactStyle style;
    style.admin = admin;
    style.quiet = quiet;
    style.timeout = timeout;
    style.carrier = contact.getCarrier();
    return write(contact, cmd, reply, style);
}

bool NetworkBase::write(const Contact& contact,
                        PortWriter& cmd,
                        PortReader& reply,
                        const ContactStyle& style)
{
    if (!getNameSpace().serverAllocatesPortNumbers()) {
        // switch to more up-to-date method

        Port port;
        port.setAdminMode(style.admin);
        port.openFake("network_write");
        Contact ec = contact;
        if (!style.carrier.empty()) {
            ec.setCarrier(style.carrier);
        }
        if (!port.addOutput(ec)) {
            if (!style.quiet) {
                yCError(NETWORK, "Cannot make connection to '%s'", ec.toString().c_str());
            }
            return false;
        }

        bool ok = port.write(cmd, reply);
        return ok;
    }

    const char* connectionName = "admin";
    std::string name = contact.getName();
    const char* targetName = name.c_str(); // use carefully!
    Contact address = contact;
    if (!address.isValid()) {
        address = getNameSpace().queryName(targetName);
    }
    if (!address.isValid()) {
        if (!style.quiet) {
            yCError(NETWORK, "cannot find port %s", targetName);
        }
        return false;
    }

    if (style.timeout > 0) {
        address.setTimeout((float)style.timeout);
    }
    OutputProtocol* out = Carriers::connect(address);
    if (out == nullptr) {
        if (!style.quiet) {
            yCError(NETWORK, "Cannot connect to port %s", targetName);
        }
        return false;
    }
    if (style.timeout > 0) {
        out->setTimeout(style.timeout);
    }

    Route r(connectionName, targetName, (!style.carrier.empty()) ? style.carrier.c_str() : "text_ack");
    out->open(r);

    PortCommand pc(0, style.admin ? "a" : "d");
    BufferedConnectionWriter bw(out->getConnection().isTextMode(),
                                out->getConnection().isBareMode());
    bool ok = true;
    if (out->getConnection().canEscape()) {
        ok = pc.write(bw);
    }
    if (!ok) {
        if (!style.quiet) {
            yCError(NETWORK, "could not write to connection");
        }
        delete out;
        return false;
    }
    ok = cmd.write(bw);
    if (!ok) {
        if (!style.quiet) {
            yCError(NETWORK, "could not write to connection");
        }
        delete out;
        return false;
    }
    if (style.expectReply) {
        bw.setReplyHandler(reply);
    }
    out->write(bw);
    if (out != nullptr) {
        delete out;
        out = nullptr;
    }
    return true;
}

bool NetworkBase::write(const std::string& port_name,
                        PortWriter& cmd,
                        PortReader& reply)
{
    return write(Contact(port_name), cmd, reply);
}

bool NetworkBase::isConnected(const std::string& src, const std::string& dest, const ContactStyle& style)
{
    int result = metaConnect(src, dest, style, YARP_ENACT_EXISTS);
    if (result != 0) {
        if (!style.quiet) {
            yCInfo(NETWORK, "No connection from %s to %s found",
                   src.c_str(),
                   dest.c_str());
        }
    }
    return result == 0;
}


std::string NetworkBase::getNameServerName()
{
    NameConfig nc;
    std::string name = nc.getNamespace(false);
    return name;
}


Contact NetworkBase::getNameServerContact()
{
    return getNameSpace().getNameServerContact();
}


bool NetworkBase::setNameServerName(const std::string& name)
{
    NameConfig nc;
    std::string fname = nc.getConfigFileName(YARP_CONFIG_NAMESPACE_FILENAME);
    nc.writeConfig(fname, name + "\n");
    nc.getNamespace(true);
    getNameSpace().activate(true);
    return true;
}


bool NetworkBase::checkNetwork()
{
    return getNameSpace().checkNetwork();
}


bool NetworkBase::checkNetwork(double timeout)
{
    return getNameSpace().checkNetwork(timeout);
}


bool NetworkBase::initialized()
{
    return __yarp_is_initialized > 0;
}

#ifndef YARP_NO_DEPRECATED // Since YARP 3.4
void NetworkBase::setVerbosity(int verbosity)
{
    if (verbosity < 0) {
        yarp::os::Log::setMinimumPrintLevel(yarp::os::Log::WarningType);
        yarp::os::Log::setMinimumForwardLevel(yarp::os::Log::WarningType);
    } else if (verbosity == 0) {
        yarp::os::Log::setMinimumPrintLevel(yarp::os::Log::DebugType);
        yarp::os::Log::setMinimumForwardLevel(yarp::os::Log::DebugType);
    }
}
#endif

void NetworkBase::queryBypass(NameStore* store)
{
    getNameSpace().queryBypass(store);
}

NameStore* NetworkBase::getQueryBypass()
{
    return getNameSpace().getQueryBypass();
}

#ifndef YARP_NO_DEPRECATED // Since YARP 3.4.0

std::string NetworkBase::getEnvironment(const char* key,
                                        bool* found)
{
    return yarp::conf::environment::get_string(key, found);
}

void NetworkBase::setEnvironment(const std::string& key, const std::string& val)
{
    yarp::conf::environment::set_string(key, val);
}

void NetworkBase::unsetEnvironment(const std::string& key)
{
    yarp::conf::environment::unset(key);
}

#endif

#ifndef YARP_NO_DEPRECATED // Since YARP 3.3.0

std::string NetworkBase::getDirectorySeparator()
{
    return std::string{yarp::conf::filesystem::preferred_separator};
}

std::string NetworkBase::getPathSeparator()
{
    return std::string{yarp::conf::environment::path_separator};
}

#endif // YARP_NO_DEPRECATED

namespace {
std::mutex& getNetworkMutex()
{
    static std::mutex mutex;
    return mutex;
}
} // namespace

void NetworkBase::lock()
{
    getNetworkMutex().lock();
}

void NetworkBase::unlock()
{
    getNetworkMutex().unlock();
}


int NetworkBase::sendMessage(const std::string& port,
                             yarp::os::PortWriter& writable,
                             bool silent)
{
    std::string output;
    return sendMessage(port, writable, output, silent);
}

int NetworkBase::sendMessage(const std::string& port,
                             PortWriter& writable,
                             std::string& output,
                             bool quiet)
{
    output = "";
    Contact srcAddress = NetworkBase::queryName(port);
    if (!srcAddress.isValid()) {
        if (!quiet) {
            yCError(NETWORK, "Cannot find port named %s", port.c_str());
        }
        return 1;
    }
    OutputProtocol* out = Carriers::connect(srcAddress);
    if (out == nullptr) {
        if (!quiet) {
            yCError(NETWORK, "Cannot connect to port named %s at %s", port.c_str(), srcAddress.toURI().c_str());
        }
        return 1;
    }
    Route route("admin", port, "text");


    bool ok = out->open(route);
    if (!ok) {
        if (!quiet) {
            yCError(NETWORK, "Cannot make connection");
        }
        delete out;
        return 1;
    }

    BufferedConnectionWriter bw(out->getConnection().isTextMode());
    PortCommand disconnect('\0', "q");
    bool wok = writable.write(bw);
    if (!wok) {
        if (!quiet) {
            yCError(NETWORK, "Cannot write on connection");
        }
        delete out;
        return 1;
    }
    if (!disconnect.write(bw)) {
        if (!quiet) {
            yCError(NETWORK, "Cannot write on connection");
        }
        delete out;
        return 1;
    }

    out->write(bw);
    InputProtocol& ip = out->getInput();
    ConnectionReader& con = ip.beginRead();
    Bottle b;
    b.read(con);
    b.read(con);
    output = b.toString();
    if (!quiet) {
        yCInfo(NETWORK, "%s", b.toString().c_str());
    }
    ip.endRead();
    out->close();
    delete out;
    out = nullptr;

    return 0;
}

int NetworkBase::poll(const std::string& target, bool silent)
{
    PortCommand pc('\0', "*");
    return sendMessage(target, pc, silent);
}

int NetworkBase::disconnectInput(const std::string& src,
                                 const std::string& dest,
                                 bool silent)
{
    PortCommand pc('\0', std::string("~") + dest);
    return sendMessage(src, pc, silent);
}

class ForwardingCarrier : public Carrier
{
public:
    SharedLibraryClassFactory<Carrier>* factory;
    SharedLibraryClass<Carrier> car;
    Carrier* owner;

    ForwardingCarrier()
    {
        owner = nullptr;
        factory = nullptr;
    }

    ForwardingCarrier(SharedLibraryClassFactory<Carrier>* factory,
                      Carrier* owner) :
            factory(factory),
            owner(owner)
    {
        factory->addRef();
        car.open(*factory);
    }

    ~ForwardingCarrier() override
    {
        car.close();
        if (factory == nullptr) {
            return;
        }
        factory->removeRef();
        if (factory->getReferenceCount() <= 0) {
            delete factory;
        }
        factory = nullptr;
    }

    virtual Carrier& getContent()
    {
        return car.getContent();
    }

    virtual const Carrier& getContent() const
    {
        return car.getContent();
    }

    Carrier* create() const override
    {
        return owner->create();
    }


    // Forward yarp::os::Connection methods

    bool isValid() const override
    {
        return car.isValid();
    }

    bool isTextMode() const override
    {
        return getContent().isTextMode();
    }

    bool isBareMode() const override
    {
        return getContent().isBareMode();
    }

    bool canEscape() const override
    {
        return getContent().canEscape();
    }

    void handleEnvelope(const std::string& envelope) override
    {
        getContent().handleEnvelope(envelope);
    }

    bool requireAck() const override
    {
        return getContent().requireAck();
    }

    bool supportReply() const override
    {
        return getContent().supportReply();
    }

    bool isLocal() const override
    {
        return getContent().isLocal();
    }

    bool isPush() const override
    {
        return getContent().isPush();
    }

    bool isConnectionless() const override
    {
        return getContent().isConnectionless();
    }

    bool isBroadcast() const override
    {
        return getContent().isBroadcast();
    }

    bool isActive() const override
    {
        return getContent().isActive();
    }

    bool modifiesIncomingData() const override
    {
        return getContent().modifiesIncomingData();
    }

    ConnectionReader& modifyIncomingData(ConnectionReader& reader) override
    {
        return getContent().modifyIncomingData(reader);
    }

    bool acceptIncomingData(ConnectionReader& reader) override
    {
        return getContent().acceptIncomingData(reader);
    }

    bool modifiesOutgoingData() const override
    {
        return getContent().modifiesOutgoingData();
    }

    const PortWriter& modifyOutgoingData(const PortWriter& writer) override
    {
        return getContent().modifyOutgoingData(writer);
    }

    bool acceptOutgoingData(const PortWriter& writer) override
    {
        return getContent().acceptOutgoingData(writer);
    }

    bool modifiesReply() const override
    {
        return getContent().modifiesReply();
    }

    PortReader& modifyReply(PortReader& reader) override
    {
        return getContent().modifyReply(reader);
    }

    void setCarrierParams(const Property& params) override
    {
        getContent().setCarrierParams(params);
    }

    void getCarrierParams(Property& params) const override
    {
        getContent().getCarrierParams(params);
    }

    void getHeader(yarp::os::Bytes& header) const override
    {
        getContent().getHeader(header);
    }

    void prepareDisconnect() override
    {
        getContent().prepareDisconnect();
    }

    std::string getName() const override
    {
        return getContent().getName();
    }


    // Forward yarp::os::Carrier methods

    bool checkHeader(const yarp::os::Bytes& header) override
    {
        return getContent().checkHeader(header);
    }

    void setParameters(const yarp::os::Bytes& header) override
    {
        getContent().setParameters(header);
    }

    bool canAccept() const override
    {
        return getContent().canAccept();
    }

    bool canOffer() const override
    {
        return getContent().canOffer();
    }

    bool prepareSend(ConnectionState& proto) override
    {
        return getContent().prepareSend(proto);
    }

    bool sendHeader(ConnectionState& proto) override
    {
        return getContent().sendHeader(proto);
    }

    bool expectReplyToHeader(ConnectionState& proto) override
    {
        return getContent().expectReplyToHeader(proto);
    }

    bool write(ConnectionState& proto, SizedWriter& writer) override
    {
        return getContent().write(proto, writer);
    }

    bool reply(ConnectionState& proto, SizedWriter& writer) override
    {
        return getContent().reply(proto, writer);
    }

    bool expectExtraHeader(ConnectionState& proto) override
    {
        return getContent().expectExtraHeader(proto);
    }

    bool respondToHeader(ConnectionState& proto) override
    {
        return getContent().respondToHeader(proto);
    }

    bool expectIndex(ConnectionState& proto) override
    {
        return getContent().expectIndex(proto);
    }

    bool expectSenderSpecifier(ConnectionState& proto) override
    {
        return getContent().expectSenderSpecifier(proto);
    }

    bool sendAck(ConnectionState& proto) override
    {
        return getContent().sendAck(proto);
    }

    bool expectAck(ConnectionState& proto) override
    {
        return getContent().expectAck(proto);
    }

    std::string toString() const override
    {
        return getContent().toString();
    }

    void close() override
    {
        getContent().close();
    }

    std::string getBootstrapCarrierName() const override
    {
        return getContent().getBootstrapCarrierName();
    }

    int connect(const yarp::os::Contact& src,
                const yarp::os::Contact& dest,
                const yarp::os::ContactStyle& style,
                int mode,
                bool reversed) override
    {
        return getContent().connect(src, dest, style, mode, reversed);
    }

    bool configure(ConnectionState& proto) override
    {
        return getContent().configure(proto);
    }
    bool configureFromProperty(yarp::os::Property& options) override
    {
        return getContent().configureFromProperty(options);
    }

    yarp::os::Face* createFace() const override
    {
        return getContent().createFace();
    }
};


class StubCarrier : public ForwardingCarrier
{
private:
    YarpPluginSettings settings;
    YarpPlugin<Carrier> plugin;

public:
    StubCarrier(const char* dll_name, const char* fn_name)
    {
        settings.setLibraryMethodName(dll_name, fn_name);
        init();
    }

    StubCarrier(const char* name)
    {
        settings.setPluginName(name);
        init();
    }

    void init()
    {
        YarpPluginSelector selector;
        selector.scan();
        settings.setSelector(selector);
        if (plugin.open(settings)) {
            car.open(*plugin.getFactory());
            settings.setLibraryMethodName(plugin.getFactory()->getName(),
                                          settings.getMethodName());
        }
    }

    Carrier& getContent() override
    {
        return car.getContent();
    }

    const Carrier& getContent() const override
    {
        return car.getContent();
    }

    Carrier* create() const override
    {
        auto* ncar = new ForwardingCarrier(plugin.getFactory(), const_cast<StubCarrier*>(this));
        if (ncar == nullptr) {
            return nullptr;
        }
        if (!ncar->isValid()) {
            delete ncar;
            ncar = nullptr;
            return nullptr;
        }
        return ncar;
    }

    std::string getDllName() const
    {
        return settings.getLibraryName();
    }

    std::string getFnName() const
    {
        return settings.getMethodName();
    }
};


bool NetworkBase::registerCarrier(const char* name, const char* dll)
{
    StubCarrier* factory = nullptr;
    if (dll == nullptr) {
        factory = new StubCarrier(name);
        if (factory == nullptr) {
            return false;
        }
    } else {
        factory = new StubCarrier(dll, name);
    }
    if (factory == nullptr) {
        yCError(NETWORK, "Failed to register carrier");
        return false;
    }
    if (!factory->isValid()) {
        if (dll != nullptr) {
            yCError(NETWORK, "Failed to find library %s with carrier %s", dll, name);
        } else {
            yCError(NETWORK, "Failed to find library support for carrier %s", name);
        }
        delete factory;
        factory = nullptr;
        return false;
    }
    Carriers::addCarrierPrototype(factory);
    return true;
}


bool NetworkBase::localNetworkAllocation()
{
    bool globalAlloc = getNameSpace().serverAllocatesPortNumbers();
    return !globalAlloc;
}


Contact NetworkBase::detectNameServer(bool useDetectedServer,
                                      bool& scanNeeded,
                                      bool& serverUsed)
{
    return getNameSpace().detectNameServer(useDetectedServer,
                                           scanNeeded,
                                           serverUsed);
}

bool NetworkBase::setNameServerContact(Contact& nameServerContact)
{
    NameConfig nameConfig;
    if (!nameServerContact.getName().empty()) {
        setNameServerName(nameServerContact.getName());
    }
    nameConfig.fromFile();
    nameConfig.setAddress(nameServerContact);
    bool result = nameConfig.toFile();
    getNameSpace().activate(true);
    return result;
}


bool NetworkBase::writeToNameServer(PortWriter& cmd,
                                    PortReader& reply,
                                    const ContactStyle& style)
{
    NameStore* store = getNameSpace().getQueryBypass();
    if (store != nullptr) {
        Contact contact;
        return store->process(cmd, reply, contact);
    }
    return getNameSpace().writeToNameServer(cmd, reply, style);
}


std::string NetworkBase::getConfigFile(const char* fname)
{
    return NameConfig::expandFilename(fname);
}


int NetworkBase::getDefaultPortRange()
{
    std::string range = yarp::conf::environment::get_string("YARP_PORT_RANGE");
    if (!range.empty()) {
        int irange = yarp::conf::numeric::from_string<int>(range);
        if (irange != 0) {
            return irange;
        }
    }
    return 10000;
}
