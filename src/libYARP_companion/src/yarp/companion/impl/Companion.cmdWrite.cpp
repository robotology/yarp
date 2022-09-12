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
#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/PeriodicThread.h>
#include <yarp/os/Semaphore.h>

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
using yarp::os::NetworkBase;

class writerThread : public yarp::os::PeriodicThread
{
    private:
    Port* outport=nullptr;
    Bottle bot;
    std::mutex mut;

    public:
    writerThread (double period, Port* _outport) : yarp::os::PeriodicThread(period)
    {
        outport = _outport;
    }

    void run() override
    {
        mut.lock();
        if (outport && bot.size()!=0)
        {
            outport->write(bot);
        }
        mut.unlock();
    }

    void write(Bottle& _bot)
    {
        mut.lock();
        bot = _bot;
        mut.unlock();
    }
};

int Companion::write(const char *name, int ntargets, char *targets[], double period)
{
    Port port;
    writerThread* writer_thread = nullptr;
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

    if (period>0)
    {
        writer_thread = new writerThread(period, &port);
        writer_thread->start();
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

            if (period>0)
            {
                writer_thread->write(bot);
            }
            else
            {
                port.write(bot);
            }

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
        port.write(bot);
    }

    if (period > 0)
    {
        writer_thread->stop();
        delete writer_thread;
    }
    else
    {
        port.close();
    }

    return 0;
}


int Companion::cmdWrite(int argc, char *argv[])
{
    if (argc<1)
    {
        yCError(COMPANION, "Usage:");
        yCError(COMPANION, "Please supply the port name, and optionally some targets, e.g.");
        yCError(COMPANION, "  yarp write <port> [remote port1] [remote port2] [...] [--period <s>]");
        yCError(COMPANION, "If the period optional parameter is given, then the message is written periodically");
        return 1;
    }

    //get the name of the source port
    const char *src = argv[0];

    //the following check prevents opening as local port a port which is already registered (and active) on the yarp nameserver
    bool e = NetworkBase::exists(src, true);
    if (e)
    {
        yCError(COMPANION) << "Port" << src << "already exists! Aborting...";
        return 1;
    }

    if(argc >= 2) {
        //parsing of the period option
        double period = 0;
        //check a malformed command line in which the --period is specified, but with no value.
        if (strcmp(argv[argc - 1], "--period") == 0)
        {
            yCError(COMPANION, "Invalid period value");
            return 1;
        }
        //check if the period option is the present
        if (strcmp(argv[argc -2 ],"--period")==0)
        {
            //get the value of the --period option
            double pp=atof(argv[argc - 1]);
            if (pp>=0)
            {
                period = pp;
                yCInfo(COMPANION, "Message will be published with a period of %f s", period);
            }
            else
            {
                yCError(COMPANION, "Invalid period value");
                return 1;
            }

            //remove the --period option and its following value from argc, argv
            argc--;
            argc--;
        }

        return write(src, argc-1, argv+1, period);
    }

    return write(src, argc-1, argv+1, 0);
}
