/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Terminator.h>

#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/TerminatorMsgs.h>
#include <yarp/os/impl/LogComponent.h>


using namespace yarp::os::impl;
using namespace yarp::os;

namespace {
YARP_OS_LOG_COMPONENT(TERMINATOR, "yarp.os.Terminator")

/*
 * Server side of the TerminatorMsgs thrift service.
 */
class TerminatorServer :
        public yarp::os::TerminatorMsgs
{
public:
    explicit TerminatorServer(volatile bool& quitFlag) :
            m_quit(quitFlag)
    {
    }

    bool quit() override
    {
        m_quit = true;
        return true;
    }

private:
    volatile bool& m_quit;
};

} // namespace


bool Terminator::terminateByName(const char* name)
{
    if (name == nullptr) {
        return false;
    }

    std::string s(name);

    if (name[0] != '/') {
        s.clear();
        s += "/";
        s += name;
    }

    Contact c = NetworkBase::queryName(s);
    if (!c.isValid()) {
        yCError(TERMINATOR, "Terminator port not found");
        return false;
    }
    c.setCarrier("tcp");

    // Client side of the TerminatorMsgs thrift service: the commands are
    // sent through a (not registered) port connected to the Terminee port.
    Port port;
    if (!port.openFake("terminator")) {
        yCError(TERMINATOR, "Cannot open the terminator client port");
        return false;
    }
    if (!port.addOutput(c)) {
        yCError(TERMINATOR, "Cannot connect to %s", s.c_str());
        return false;
    }
    TerminatorMsgs client;
    client.yarp().attachAsClient(port);
    if (!client.quit()) {
        yCError(TERMINATOR, "The quit request has not been accepted by %s", s.c_str());
        return false;
    }

    return true;
}

#define TermineeHelper Port
#define HELPER(x) (*((TermineeHelper*)(x)))

Terminee::Terminee(const char* name)
{
    ok = false;
    implementation = nullptr;
    if (name == nullptr) {
        quit = true;
        yCError(TERMINATOR, "Terminator: Please supply a proper port name");
        return;
    }

    std::string s(name);
    if (name[0] != '/') {
        s.clear();
        s += "/";
        s += name;
    }

    implementation = new TermineeHelper();
    yCAssert(TERMINATOR, implementation != nullptr);
    TermineeHelper& helper = HELPER(implementation);
    ok = helper.open(s);
    if (!ok) {
        quit = true;
        yCError(TERMINATOR, "Kill port conflict: make sure you supply a distinct --name /PORTNAME");
    } else {
        quit = false;
        start();
    }
}

void Terminee::onStop()
{
    TermineeHelper& helper = HELPER(implementation);
    quit = true;
    helper.interrupt();
}

Terminee::~Terminee()
{
    TermineeHelper& helper = HELPER(implementation);
    if (!quit) {
        Terminator::terminateByName(helper.getName().c_str());
    }

    stop();

    if (implementation != nullptr) {
        delete &HELPER(implementation);
    }
}


void Terminee::run()
{
    TermineeHelper& helper = HELPER(implementation);
    TerminatorServer server(quit);
    while (!isStopping() && !quit) {
        // The thrift service reads the command and writes the reply
        // (including the errors, e.g. for unknown commands).
        helper.read(server);
    }
}

bool Terminee::waitQuit() const
{
    // not yet implemented
    return false;
}

bool Terminee::mustQuit() const
{
    return quit;
}


bool Terminee::isOk() const
{
    return ok;
}
