/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/companion/impl/Companion.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/Contact.h>
#include <yarp/os/Name.h>
#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/Value.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/impl/Terminal.h>

#ifdef YARP_HAS_Libedit
#include <editline/readline.h>
#include <cstring>
#include <vector>
#endif

using yarp::companion::impl::Companion;
using yarp::os::Bottle;
using yarp::os::Contact;
using yarp::os::Name;
using yarp::os::NetworkBase;
using yarp::os::Port;
using yarp::os::Value;



namespace {

#ifdef YARP_HAS_Libedit
yarp::os::Port* rpcHelpPort = nullptr;
bool commandListInitialized = false;
std::vector<std::string> commands;

char* dupstr(char* s)
{
    char *r;
    r = (char*) malloc ((strlen (s) + 1));
    strcpy (r, s);
    return (r);
}

/* Generator function for command completion.  STATE lets us know whether
   to start from scratch; without any state (i.e. STATE == 0), then we
   start at the top of the list. */
char* command_generator (const char* text, int state)
{
    static size_t list_index, len;
    char *name;

    /* if this is a new word to complete, initialize now.  this includes
        saving the length of text for efficiency, and initializing the index
        variable to 0. */
    if (!state)
    {
        list_index = 0;
        len = strlen (text);
    }

    if (!commandListInitialized)
    {
        commands.clear();
        yarp::os::Bottle helpCommand, helpBottle;
        helpCommand.addString("help");
        bool helpOk=false;
        if (rpcHelpPort) {
            helpOk = rpcHelpPort->write(helpCommand, helpBottle);
        }
        if (helpOk)
        {
            yarp::os::Bottle* cmdList = nullptr;
            if (helpBottle.get(0).isVocab32() && helpBottle.get(0).asVocab32()==yarp::os::createVocab32('m', 'a', 'n', 'y') )
            {
                cmdList=helpBottle.get(1).asList();
            } else {
                cmdList = helpBottle.get(0).asList();
            }
            if (cmdList && cmdList->get(0).asString() == "*** Available commands:")
            {
                for (size_t i = 1; i < cmdList->size(); ++i) {
                    commands.push_back(cmdList->get(i).asString());
                }
            }
        }
        commands.emplace_back(" ");
        commandListInitialized=true;
    }
    while ((list_index<commands.size()) && (name = (char*)commands[list_index].c_str()))
        {
        list_index++;
        if (strncmp(name, text, len) == 0) {
            return (dupstr(name));
        }
        }

    /* if no names matched, then return null. */
    return ((char *)nullptr);
}

/* Attempt to complete on the contents of TEXT.  START and END show the
   region of TEXT that contains the word to complete.  We can use the
   entire line in case we want to do some simple parsing.  Return the
   array of matches, or nullptr if there aren't any. */
char ** my_completion (const char* text, int start, int end)
{
    YARP_UNUSED(end);
    char **matches;
    matches = (char **)nullptr;

    /* If this word is at the start of the line, then it is a command
       to complete. If we are completing after "help ", it is a command again.
       Otherwise, stop completing. */
    if (start == 0) {
        matches = rl_completion_matches(text, &command_generator);
    } else if (start == 5 && strncmp(text, "help ", 5) != 0) {
        matches = rl_completion_matches(text, &command_generator);
    } else {
        rl_attempted_completion_over = 1;
    }

    return (matches);
}
#endif


} // namespace


int Companion::rpc(const char *connectionName, const char *targetName)
{
    Bottle resendContent;
    bool resendFlag = false;
    int resendCount = 0;

    bool firstTimeRound = true;
#ifdef YARP_HAS_Libedit
    rl_attempted_completion_function = my_completion;
#endif

    while (!yarp::os::impl::Terminal::EOFreached()) {
        Port port;
        port.setRpcClient();
        port.openFake(connectionName);
        if (!port.addOutput(targetName)) {
            yCError(COMPANION, "Cannot make connection");
            yCError(COMPANION, "Alternative method: precede port name with --client");
            return 1;
        }
        if (adminMode) {
            port.setAdminMode();
        }

        if (!firstTimeRound) {
            yCInfo(COMPANION, "Target disappeared, reconnecting...");
        }
        firstTimeRound = false;

        if (resendFlag) {
            if (resendCount==3) {
                resendFlag = false;
                resendCount = 0;
            }
        }

#ifdef YARP_HAS_Libedit
    rpcHelpPort = &port;
#endif
        while (port.getOutputCount()==1&&!yarp::os::impl::Terminal::EOFreached()) {
            std::string txt;
            if (!resendFlag) {
                txt = yarp::os::impl::Terminal::getStdin();
            }

            if (!yarp::os::impl::Terminal::EOFreached()) {
                if (txt.length()>0) {
                    if (txt[0]<32 && txt[0]!='\n' &&
                        txt[0]!='\r') {
                        break;  // for example, horrible windows ^D
                    }
                }
                Bottle bot;
                if (!resendFlag) {
                    bot.fromString(txt);
                } else {
                    bot = resendContent;
                    resendFlag = false;
                }

                Bottle reply;
                bool ok = port.write(bot, reply);
                if (!ok) {
                    resendContent = bot;
                    resendFlag = true;
                    resendCount++;
                    break;
                }
                if (reply.get(0).isVocab32() && reply.get(0).asVocab32()==yarp::os::createVocab32('m', 'a', 'n', 'y')) {
                    yCInfo(COMPANION, "Responses:");
                    Bottle *lst = &reply;
                    int start = 1;
                    if (reply.size()==2 && reply.get(1).isList()) {
                        lst = reply.get(1).asList();
                        start = 0;
                    }
                    for (size_t i=start; i<lst->size(); i++) {
                        Value& v = lst->get(i);
                        if (v.isString()) {
                            yCInfo(COMPANION, "  %s", v.asString().c_str());
                        } else {
                            yCInfo(COMPANION, "  %s", v.toString().c_str());
                        }
                    }
                } else {
                    yCInfo(COMPANION, "Response: %s", reply.toString().c_str());
                }
                resendCount = 0;
            }
        }
    }

    return 0;
}

int Companion::rpcClient(int argc, char *argv[])
{
    if (argc<1) {
        yCError(COMPANION, "Please supply remote port name, and local port name");
        return 1;
    }

    const char *dest = argv[0];
    const char *src = "...";
    if (argc>1) {
        src = argv[1];
    }

    Port p;
    p.setRpcClient();
    applyArgs(p);
    bool ok;
    if (argc>1) {
        ok = p.open(src);
    } else {
        ok = p.open("...");
    }
    if (ok) {
        if (std::string(dest)!="--client") {
            //NetworkBase::connect(p.getName().c_str(), dest);
            ok = p.addOutput(dest);
        }
    }
    while(ok) {
        std::string txt = yarp::os::impl::Terminal::getStdin();
        if (yarp::os::impl::Terminal::EOFreached()) {
            break;
        }
        Bottle cmd(txt);
        Bottle reply;
        ok = p.write(cmd, reply);
        if (ok) {
            yCInfo(COMPANION, "%s", reply.toString().c_str());
        }
    }
    return 0;
}

int Companion::cmdRpc(int argc, char *argv[])
{
    if (argc<1) {
        yCInfo(COMPANION, "Usage:");
        yCInfo(COMPANION, "  yarp [admin] rpc </destport>");
        yCInfo(COMPANION, "    Open an RPC client and connect to 'destport'");
        yCInfo(COMPANION, "    If 'admin' is passed, connect to the administrative interface of the port");
        yCInfo(COMPANION, "  yarp rpc --client </port>");
        yCInfo(COMPANION, "    Open an RPC client on 'port' amd wait for a connection");
        return 1;
    }

    const char *dest = argv[0];
    const char *src;
    if (std::string(dest) == "--client") {
        return rpcClient(argc, argv);
    }
    Contact address = Name(dest).toAddress();
    if (address.getCarrier().empty()) {
        address = NetworkBase::queryName(dest);
    }
    // no need for a port
    src = "anon_rpc";
    if (argc>1) { src = argv[1]; }
    return rpc(src, dest);
}
