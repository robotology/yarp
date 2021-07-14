/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/run/impl/RunReadWrite.h>

#include <yarp/os/Time.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Property.h>
#include <yarp/os/impl/PlatformSignal.h>

#include <cstdio>

int RunWrite::loop()
{
    RUNLOG("<<<loop()")

    if (!wPort.open(wPortName))
    {
        RUNLOG("RunWrite: could not open output port")
        fprintf(stderr, "RunWrite: could not open output port\n");
        return 1;
    }
    if (yarp::os::Network::exists(wLoggerName))
    {
        if (yarp::os::Network::connect(wPortName, wLoggerName)==false)
        {
            fprintf(stderr, "RunWrite: could not mmake connection with the logger\n");
        }
    }

    char txt[2048];

    std::string tag=std::string("[")+wPortName+std::string("]");

    while (mRunning)
    {
        if (!fgets(txt, 2048, stdin) || ferror(stdin) || feof(stdin)) {
            break;
        }

        if (!mRunning) {
            break;
        }

        yarp::os::Bottle bot;
        if (mVerbose) {
            bot.addString(tag.c_str());
        }
        bot.addString(txt);
        wPort.write(bot);
    }

    RUNLOG(">>>loop()")

    return 0;
}

///////////////////////////////////////////

int RunRead::loop()
{
    RUNLOG("<<<loop()")

    if (!rPort.open(rPortName))
    {
        RUNLOG("RunRead: could not open input port")
        fprintf(stderr, "RunRead: could not open input port\n");
        return 1;
    }

    while (mRunning)
    {
        yarp::os::Bottle bot;
        if (!rPort.read(bot, true))
        {
            RUNLOG("!rPort.read(bot, true)")
            break;
        }

        if (!mRunning) {
            break;
        }

        if (bot.size()==1)
        {
            printf("%s", bot.get(0).asString().c_str());
        }
        else
        {
            printf("%s\n", bot.toString().c_str());
        }

        fflush(stdout);
    }

    rPort.close();

    RUNLOG(">>>loop()")

    return 0;
}

///////////////////////////////////////////

int RunReadWrite::loop()
{
    RUNLOG("<<<loop()")

    if (!rPort.open(rPortName))
    {
        RUNLOG("RunReadWrite: could not open input port")
        fprintf(stderr, "RunReadWrite: could not open input port\n");
        return 1;
    }

    yarp::os::ContactStyle style;
    style.persistent=true;

    yarp::os::Network::connect(UUID+"/stdout", rPortName, style);

    // forwarded section
    std::string tag;

    if (mForwarded)
    {
        tag=std::string("[")+fPortName+std::string("]");
        if (!fPort.open(fPortName))
        {
            RUNLOG("RunReadWrite: could not open forward port")
            fprintf(stderr, "RunReadWrite: could not open forward port\n");

            rPort.close();

            return 1;
        }
    }
    /////////////////////

    #if !defined(_WIN32)
    if (yarp::os::impl::getppid()!=1)
    #endif
    {
        RUNLOG("start()")
        start();

        while (mRunning)
        {
            #if !defined(_WIN32)
            if (yarp::os::impl::getppid() == 1) {
                break;
            }
#endif

            yarp::os::Bottle bot;

            if (!rPort.read(bot, true))
            {
                RUNLOG("!rPort.read(bot, true)")
                break;
            }

            if (!mRunning) {
                break;
            }

#if !defined(_WIN32)
            if (yarp::os::impl::getppid() == 1) {
                break;
            }
#endif

            if (bot.size()==1)
            {
                printf("%s", bot.get(0).asString().c_str());
                fflush(stdout);

                if (mForwarded)
                {
                    yarp::os::Bottle fwd;
                    fwd.addString(tag.c_str());
                    fwd.addString(bot.get(0).asString().c_str());
                    fPort.write(fwd);
                }
            }
            else
            {
                printf("%s\n", bot.toString().c_str());
                fflush(stdout);

                if (mForwarded)
                {
                    yarp::os::Bottle fwd;
                    fwd.addString(tag.c_str());
                    fwd.addString(bot.toString().c_str());
                    fPort.write(fwd);
                }
            }
        }

        rPort.close();

        wPort.close();

        if (mForwarded) {
            fPort.close();
        }

#if defined(_WIN32)
        ::exit(0);
#else
        int term_pipe[2];
        int warn_suppress = yarp::run::impl::pipe(term_pipe);
        YARP_UNUSED(warn_suppress);
        yarp::run::impl::dup2(term_pipe[0], STDIN_FILENO);
        FILE* file_term_pipe=fdopen(term_pipe[1], "w");
        fprintf(file_term_pipe, "SHKIATTETE!\n");
        fflush(file_term_pipe);
        fclose(file_term_pipe);
#endif
    }

    RUNLOG(">>>loop()")

    return 0;
}

void RunReadWrite::run()
{
    char txt[2048];

    RUNLOG("<<<run()")

    if (!wPort.open(wPortName))
    {
        RUNLOG("RunReadWrite: could not open output port")
        fprintf(stderr, "RunReadWrite: could not open output port\n");
        return;
    }

    yarp::os::ContactStyle style;
    style.persistent=true;

    yarp::os::Network::connect(wPortName, UUID+"/stdin", style);

    while (mRunning)
    {
        RUNLOG("mRunning")

        #if !defined(_WIN32)
        if (yarp::os::impl::getppid() == 1) {
            break;
        }
#endif

        if (!fgets(txt, 2048, stdin) || ferror(stdin) || feof(stdin)) {
            break;
        }

        RUNLOG(txt)

        #if !defined(_WIN32)
        if (yarp::os::impl::getppid() == 1) {
            break;
        }
#endif

        if (!mRunning) {
            break;
        }

        RUNLOG(txt)

        yarp::os::Bottle bot;
        bot.addString(txt);

        RUNLOG("<<<wPort.write(bot)")
        wPort.write(bot);
        RUNLOG(">>>wPort.write(bot)")
    }

    RUNLOG(">>>run()")
}
