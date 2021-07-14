/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/companion/impl/Companion.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Port.h>
#include <yarp/os/SystemClock.h>
#include <yarp/os/impl/Terminal.h>

#ifdef YARP_HAS_Libedit
#include <yarp/conf/dirs.h>
#include <yarp/conf/filesystem.h>
#include <yarp/os/Os.h>
#include <yarp/os/impl/PlatformUnistd.h>
#include <yarp/os/impl/PlatformStdio.h>
#include <algorithm>
#include <editline/readline.h>
#endif

using yarp::companion::impl::Companion;
using yarp::os::Bottle;
using yarp::os::Port;
using yarp::os::SystemClock;

int Companion::write(const char *name, int ntargets, char *targets[]) {
    Port port;
    applyArgs(port);
    port.setWriteOnly();
#ifdef YARP_HAS_Libedit
    std::string hist_file;
    bool disable_file_history=false;
    if (yarp::os::impl::isatty(yarp::os::impl::fileno(stdin))) //if interactive mode
    {
        auto yarpdatahome = yarp::conf::dirs::yarpdatahome();
        std::string hist_file = yarpdatahome + yarp::conf::filesystem::preferred_separator + "yarp_write";
        if (yarp::os::mkdir_p(hist_file.c_str(), 1) != 0)
        {
            yCError(COMPANION, "Unable to create directory into \"%s\"", yarpdatahome.c_str());
            return 1;
        }
        std::string temp;
        if (ntargets>0) {
            temp = targets[0];
        } else {
            temp = "any";
        }
        std::replace(temp.begin(), temp.end(), '/', '_');
        hist_file += yarp::conf::filesystem::preferred_separator;
        hist_file += temp;
        read_history(hist_file.c_str());
        disable_file_history=false;
    } else {
        disable_file_history = true;
    }
#endif
    if (Companion::getActivePort() == nullptr) {
        Companion::installHandler();
    }
    if (!port.open(name)) {
        return 1;
    }
    Companion::setUnregisterName(port.getName());
    if (adminMode) {
        port.setAdminMode();
    }

    bool raw = true;
    for (int i=0; i<ntargets; i++) {
        if (std::string(targets[i])=="verbatim") {
            raw = false;
        } else {
            if (connect(port.getName().c_str(), targets[i], true)!=0) {
                if (connect(port.getName().c_str(), targets[i], false)!=0) {
                    return 1;
                }
            }
        }
    }


    while (!yarp::os::impl::Terminal::EOFreached()) {
        std::string txt = yarp::os::impl::Terminal::getStdin();
        if (!yarp::os::impl::Terminal::EOFreached()) {
            if (txt.length()>0) {
                if (txt[0]<32 && txt[0]!='\n' &&
                    txt[0]!='\r' && txt[0]!='\t') {
                    break;  // for example, horrible windows ^D
                }
            }
            Bottle bot;
            if (!raw) {
                bot.addInt32(0);
                bot.addString(txt.c_str());
            } else {
                bot.fromString(txt);
            }
            //core.send(bot);
            if (waitConnect) {
                double delay = 0.1;
                while (port.getOutputCount()<1) {
                    SystemClock::delaySystem(delay);
                    delay *= 2;
                    if (delay > 4) {
                        delay = 4;
                    }
                }
            }
            port.write(bot);
#ifdef YARP_HAS_Libedit
            if (!disable_file_history) {
                write_history(hist_file.c_str());
            }
#endif
        }
    }

    if (port.isWriting()) {
        double delay = 0.1;
        while (port.isWriting()) {
            SystemClock::delaySystem(delay);
            delay *= 2;
            if (delay > 4) {
                delay = 4;
            }
        }
    }

    Companion::setActivePort(nullptr);

    if (!raw) {
        Bottle bot;
        bot.addInt32(1);
        bot.addString("<EOF>");
        //core.send(bot);
        port.write(bot);
    }

    //core.close();
    //core.join();
    port.close();

    return 0;
}


int Companion::cmdWrite(int argc, char *argv[])
{
    if (argc<1) {
        yCError(COMPANION, "Please supply the port name, and optionally some targets");
        return 1;
    }

    const char *src = argv[0];
    return write(src, argc-1, argv+1);
}
