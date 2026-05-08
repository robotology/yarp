/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/run/Run.h>
#include <yarp/run/impl/RunCheckpoints.h>
#include <yarp/run/impl/RunProcManager.h>
#include <yarp/run/impl/RunReadWrite.h>
#include <yarp/run/impl/PlatformStdlib.h>
#include <yarp/run/impl/PlatformUnistd.h>
#include <yarp/run/impl/PlatformSysPrctl.h>

#include <yarp/conf/environment.h>
#include <yarp/conf/filesystem.h>

#include <yarp/os/Network.h>
#include <yarp/os/Os.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/RpcClient.h>
#include <yarp/os/RpcServer.h>
#include <yarp/os/Semaphore.h>
#include <yarp/os/SystemInfo.h>
#include <yarp/os/SystemInfoSerializer.h>
#include <yarp/os/Time.h>

#include <yarp/os/impl/NameClient.h>
#include <yarp/os/impl/PlatformSignal.h>
#include <yarp/os/impl/PlatformStdio.h>

#include <cstdio>
#include <string>
#include <cstring>
#include <random>

// CLIENT
int yarp::run::Run::client(yarp::os::Property& config)
{
    // WITH STDIO
    //
    if (config.check("cmd") && config.check("stdio"))
    {
        ///////////////
        // syntax check
        if (config.find("stdio").asString()=="")
        {
            Help("SYNTAX ERROR: missing remote stdio server\n");
            return YARPRUN_ERROR;
        }
        if (config.find("cmd").asString()=="")
        {
            Help("SYNTAX ERROR: missing command\n");
            return YARPRUN_ERROR;
        }
        if (!config.check("as") || config.find("as").asString()=="")
        {
            Help("SYNTAX ERROR: missing tag\n");
            return YARPRUN_ERROR;
        }
        if (!config.check("on") || config.find("on").asString()=="")
        {
            Help("SYNTAX ERROR: missing remote server\n");
            return YARPRUN_ERROR;
        }
        //
        ///////////////

        printf("*********** %s ************\n", config.toString().c_str());

        yarp::os::Bottle msg;
        msg.addList()=config.findGroup("stdio");
        msg.addList()=config.findGroup("cmd");
        msg.addList()=config.findGroup("as");
        msg.addList()=config.findGroup("on");

        if (config.check("workdir")) {
            msg.addList() = config.findGroup("workdir");
        }
        if (config.check("geometry")) {
            msg.addList() = config.findGroup("geometry");
        }
        if (config.check("hold")) {
            msg.addList() = config.findGroup("hold");
        }
        if (config.check("env")) {
            msg.addList() = config.findGroup("env");
        }
        if (config.check("log")) {
            msg.addList() = config.findGroup("log");
        }
        /*
        {
            yarp::os::Bottle log;
            log.addString("log");
            log.addString("log");
            msg.addList()=log;
        }
        */

        std::string on=config.find("on").asString();

        yarp::os::Bottle response=sendMsg(msg, on);

        if (!response.size()) {
            return YARPRUN_ERROR;
        }

        if (response.get(0).asInt32() <= 0) {
            return 2;
        }

        return 0;
    }

    // NO STDIO
    //
    if (config.check("cmd"))
    {
        ///////////////
        // syntax check
        if (config.find("cmd").asString()=="")
        {
            Help("SYNTAX ERROR: missing command\n");
            return YARPRUN_ERROR;
        }
        if (!config.check("as") || config.find("as").asString()=="")
        {
            Help("SYNTAX ERROR: missing tag\n");
            return YARPRUN_ERROR;
        }
        if (!config.check("on") || config.find("on").asString()=="")
        {
            Help("SYNTAX ERROR: missing remote server\n");
            return YARPRUN_ERROR;
        }
        //
        ///////////////

        yarp::os::Bottle msg;
        msg.addList()=config.findGroup("cmd");
        msg.addList()=config.findGroup("as");

        if (config.check("workdir")) {
            msg.addList() = config.findGroup("workdir");
        }
        if (config.check("log")) {
            msg.addList() = config.findGroup("log");
        }
        /*
        {
            yarp::os::Bottle log;
            log.addString("log");
            log.addString("log");
            msg.addList()=log;
        }
        */
        if (config.check("env")) {
            msg.addList() = config.findGroup("env");
        }

        yarp::os::Bottle response=sendMsg(msg, config.find("on").asString());

        if (!response.size()) {
            return YARPRUN_ERROR;
        }

        if (response.get(0).asInt32() <= 0) {
            return 2;
        }

        return 0;
    }





    //----------------------------
    if (config.check("kill"))
    {
        if (!config.check("on") || config.find("on").asString()=="")
        {
            Help("SYNTAX ERROR: missing remote server\n");
            return YARPRUN_ERROR;
        }
        if (config.findGroup("kill").get(1).asString()=="")
        {
            Help("SYNTAX ERROR: missing tag\n");
            return YARPRUN_ERROR;
        }
        if (config.findGroup("kill").get(2).asInt32()==0)
        {
            Help("SYNTAX ERROR: missing signum\n");
            return YARPRUN_ERROR;
        }

        int ret = kill(config.find("on").asString(),
                       config.findGroup("kill").get(1).asString(),
                       config.findGroup("kill").get(2).asInt32());

        return ret?0:2;
    }

    //----------------------------
    if (config.check("sigterm"))
    {
        if (config.find("sigterm").asString()=="")
        {
            Help("SYNTAX ERROR: missing tag");
            return YARPRUN_ERROR;
        }
        if (!config.check("on") || config.find("on").asString()=="")
        {
            Help("SYNTAX ERROR: missing remote server\n");
            return YARPRUN_ERROR;
        }

        sigterm(config.find("on").asString(),
                config.find("sigterm").asString());

        return 0;
    }

    //----------------------------
    if (config.check("sigtermall"))
    {
        if (!config.check("on") || config.find("on").asString()=="")
        {
            Help("SYNTAX ERROR: missing remote server\n");
            return YARPRUN_ERROR;
        }

        sigtermall(config.find("on").asString());

        return 0;
    }

    //----------------------------
    if (config.check("ps"))
    {
        if (!config.check("on") || config.find("on").asString()=="")
        {
            Help("SYNTAX ERROR: missing remote server\n");
            return YARPRUN_ERROR;
        }

        std::vector<processInfo> processes;
        ps(config.find("on").asString(),processes);

        return 0;
    }

    //----------------------------
    if (config.check("isrunning"))
    {
        if (!config.check("on") || config.find("on").asString()=="")
        {
            Help("SYNTAX ERROR: missing remote server\n");
            return YARPRUN_ERROR;
        }

        if (config.find("isrunning").asString()=="")
        {
            Help("SYNTAX ERROR: missing tag\n");
            return YARPRUN_ERROR;
        }

        bool running = isRunning(config.find("on").asString(),
                                 config.find("isrunning").asString());

        return running?0:2;
    }

    //----------------------------
    if (config.check("sysinfo"))
    {
        if (!config.check("on") || config.find("on").asString()=="")
        {
            Help("SYNTAX ERROR: missing remote server\n");
            return YARPRUN_ERROR;
        }

        yarp::os::SystemInfoSerializer info;
        bool ret = sysinfo(config.find("on").asString(),info);
        if (!ret)
        {
            fprintf(stdout, "No response. (timeout)\n");
            return YARPRUN_ERROR;
        }

        fprintf(stdout, "Platform name    : %s\n", info.platform.name.c_str());
        fprintf(stdout, "Platform dist    : %s\n", info.platform.distribution.c_str());
        fprintf(stdout, "Platform release : %s\n", info.platform.release.c_str());
        fprintf(stdout, "Platform code    : %s\n", info.platform.codename.c_str());
        fprintf(stdout, "Platform kernel  : %s\n\n", info.platform.kernel.c_str());

        fprintf(stdout, "User Id        : %d\n", info.user.userID);
        fprintf(stdout, "User name      : %s\n", info.user.userName.c_str());
        fprintf(stdout, "User real name : %s\n", info.user.realName.c_str());
        fprintf(stdout, "User home dir  : %s\n\n", info.user.homeDir.c_str());

        fprintf(stdout, "Cpu load Ins.: %d\n", info.load.cpuLoadInstant);
        fprintf(stdout, "Cpu load 1   : %.2lf\n", info.load.cpuLoad1);
        fprintf(stdout, "Cpu load 5   : %.2lf\n", info.load.cpuLoad5);
        fprintf(stdout, "Cpu load 15  : %.2lf\n\n", info.load.cpuLoad15);

        fprintf(stdout, "Memory total : %dM\n", info.memory.totalSpace);
        fprintf(stdout, "Memory free  : %dM\n\n", info.memory.freeSpace);

        fprintf(stdout, "Storage total : %dM\n", info.storage.totalSpace);
        fprintf(stdout, "Storage free  : %dM\n\n", info.storage.freeSpace);

        fprintf(stdout, "Processor model     : %s\n", info.processor.model.c_str());
        fprintf(stdout, "Processor model num : %d\n", info.processor.modelNumber);
        fprintf(stdout, "Processor family    : %d\n", info.processor.family);
        fprintf(stdout, "Processor vendor    : %s\n", info.processor.vendor.c_str());
        fprintf(stdout, "Processor arch      : %s\n", info.processor.architecture.c_str());
        fprintf(stdout, "Processor cores     : %d\n", info.processor.cores);
        fprintf(stdout, "Processor siblings  : %d\n", info.processor.siblings);
        fprintf(stdout, "Processor Mhz       : %.2lf\n\n", info.processor.frequency);

        fprintf(stdout, "Environment variables  :\n%s\n", info.platform.environmentVars.toString().c_str());
        //fprintf(stdout, "Network IP4 : %s\n", info.network.ip4.c_str());
        //fprintf(stdout, "Network IP6 : %s\n", info.network.ip6.c_str());
        //fprintf(stdout, "Network mac : %s\n\n", info.network.mac.c_str());

        return 0;
    }

    //----------------------------
    if (config.check("which"))
    {
        if (!config.check("on") || config.find("on").asString()=="")
        {
            Help("SYNTAX ERROR: missing remote server\n");

            return YARPRUN_ERROR;
        }

        bool ret = which(config.find("on").asString(), config.find("which").asString());

        return ret?0:2;
    }

    //----------------------------
    if (config.check("exit"))
    {
        if (!config.check("on") || config.find("on").asString()=="")
        {
            Help("SYNTAX ERROR: missing remote server\n");

            return YARPRUN_ERROR;
        }

        bool ret = exit(config.find("on").asString());

        return ret?0:2;
    }

    return 0;
}

void yarp::run::Run::Help(const char *msg)
{
    fprintf(stderr, "%s", msg);
    fprintf(stderr, "\nUSAGE:\n\n");
    fprintf(stderr, "yarp run --server SERVERPORT\nrun a server on the local machine\n\n");
    fprintf(stderr, "yarp run --on SERVERPORT --as TAG --cmd COMMAND [ARGLIST] [--workdir WORKDIR] [--env ENVIRONMENT]\nrun a command on SERVERPORT server\n\n");
    fprintf(stderr, "yarp run --on SERVERPORT --as TAG --stdio STDIOSERVERPORT [--hold] [--geometry WxH+X+Y] --cmd COMMAND [ARGLIST] [--workdir WORKDIR] [--env ENVIRONMENT]\n");
    fprintf(stderr, "run a command on SERVERPORT server sending I/O to STDIOSERVERPORT server\n\n");
    fprintf(stderr, "yarp run --on SERVERPORT --kill TAG SIGNUM\nsend SIGNUM signal to TAG command\n\n");
    fprintf(stderr, "yarp run --on SERVERPORT --sigterm TAG\nterminate TAG command\n\n");
    fprintf(stderr, "yarp run --on SERVERPORT --sigtermall\nterminate all commands\n\n");
    fprintf(stderr, "yarp run --on SERVERPORT --ps\nreport commands running on SERVERPORT\n\n");
    fprintf(stderr, "yarp run --on SERVERPORT --isrunning TAG\nTAG command is running?\n\n");
    fprintf(stderr, "yarp run --on SERVERPORT --sysinfo\nreport system information of SERVERPORT\n\n");
    fprintf(stderr, "yarp run --on SERVERPORT --exit\nstop SERVERPORT server\n\n");
    fprintf(stderr, "yarp run --on SERVERPORT --which\nsearch for path of provided filename\n\n");
}

/////////////////////////////////////////////////////////////////
// API
/////////////////////////////////////////////////////////////////

bool yarp::run::Run::isRunning(const std::string &node, const std::string &keyv)
{
    yarp::os::Bottle msg, grp, response;

    grp.clear();
    grp.addString("on");
    grp.addString(node.c_str());
    msg.addList()=grp;

    grp.clear();
    grp.addString("isrunning");
    grp.addString(keyv.c_str());
    msg.addList()=grp;

    printf(":: %s\n", msg.toString().c_str());

    response=sendMsg(msg, node);

    if (!response.size()) {
        return false;
    }

    return response.get(0).asString()=="running";
}


bool yarp::run::Run::sysinfo(const std::string &node, yarp::os::SystemInfoSerializer& info)
{
    yarp::os::Bottle msg, grp, response;

    grp.clear();
    grp.addString("on");
    grp.addString(node.c_str());
    msg.addList()=grp;

    grp.clear();
    grp.addString("sysinfo");
    msg.addList()=grp;

    printf(":: %s\n", msg.toString().c_str());
    yarp::os::RpcClient port;
    //port.setTimeout(5.0);
    if (!port.open("..."))
    {
        fprintf(stderr, "RESPONSE:\n=========\n");
        fprintf(stderr, "Cannot open port, aborting...\n");
        return false;
    }
    bool connected = yarp::os::Network::connect(port.getName(), node);
    if (!connected)
    {
        fprintf(stderr, "RESPONSE:\n=========\n");
        fprintf(stderr, "Cannot connect to remote server, aborting...\n");
        port.close();
        //yarp::os::Network::unregisterName(port.getName());
        return false;
    }
    RUNLOG("<<<port.write(msg, info)")
    int ret = port.write(msg, info);
    RUNLOG(">>>port.write(msg, info)")
    yarp::os::Network::disconnect(port.getName(), node);
    port.close();
    //yarp::os::Network::unregisterName(port.getName());
    fprintf(stdout, "RESPONSE:\n=========\n\n");
    if (!ret)
    {
        fprintf(stdout, "No response. (timeout)\n");
        return false;
    }

    return true;
}

bool yarp::run::Run::start(const std::string &node, yarp::os::Property &command, std::string &keyv)
{
    yarp::os::Bottle msg, grp, response;

    grp.clear();
    grp.addString("on");
    grp.addString(node.c_str());
    msg.addList()=grp;

    std::string dest_srv=node;

    if (command.check("stdio"))
    {
        dest_srv=std::string(command.find("stdio").asString());

        grp.clear();
        grp.addString("stdio");
        grp.addString(dest_srv.c_str());
        msg.addList()=grp;

        if (command.check("geometry"))
        {
            grp.clear();
            grp.addString("geometry");
            grp.addString(command.find("geometry").asString().c_str());
            msg.addList()=grp;
        }

        if (command.check("hold"))
        {
            grp.clear();
            grp.addString("hold");
            msg.addList()=grp;
        }
    }

    grp.clear();
    grp.addString("as");
    grp.addString(keyv.c_str());
    msg.addList()=grp;

    grp.clear();
    grp.addString("cmd");
    grp.addString(command.find("name").asString().c_str());
    grp.addString(command.find("parameters").asString().c_str());
    msg.addList()=grp;

    printf(":: %s\n", msg.toString().c_str());

    response=sendMsg(msg, dest_srv);

    char buff[16];
    sprintf(buff, "%d", response.get(0).asInt32());
    keyv=std::string(buff);

    return response.get(0).asInt32()>0?true:false;
}

bool yarp::run::Run::sigterm(const std::string &node, const std::string &keyv)
{
    yarp::os::Bottle msg, grp, response;

    grp.clear();
    grp.addString("on");
    grp.addString(node.c_str());
    msg.addList()=grp;

    grp.clear();
    grp.addString("sigterm");
    grp.addString(keyv.c_str());
    msg.addList()=grp;

    printf(":: %s\n", msg.toString().c_str());

    response=sendMsg(msg, node);

    return response.get(0).asString()=="sigterm OK"?true:false;
}

bool yarp::run::Run::sigtermall(const std::string &node)
{
    yarp::os::Bottle msg, grp, response;

    grp.clear();
    grp.addString("on");
    grp.addString(node.c_str());
    msg.addList()=grp;

    grp.clear();
    grp.addString("sigtermall");
    msg.addList()=grp;

    printf(":: %s\n", msg.toString().c_str());

    response=sendMsg(msg, node);

    return response.get(0).asString()=="sigtermall OK"?true:false;
}

bool yarp::run::Run::kill(const std::string &node, const std::string &keyv, int s)
{
    yarp::os::Bottle msg, grp, response;

    grp.clear();
    grp.addString("on");
    grp.addString(node.c_str());
    msg.addList()=grp;

    grp.clear();
    grp.addString("kill");
    grp.addString(keyv.c_str());
    grp.addInt32(s);
    msg.addList()=grp;

    printf(":: %s\n", msg.toString().c_str());

    response=sendMsg(msg, node);

    return response.get(0).asString()=="kill OK"?true:false;
}

bool yarp::run::Run::ps(const std::string &node, std::vector<processInfo>& processes)
{
    yarp::os::Bottle msg, grp, response;
    grp.clear();
    grp.addString("on");
    grp.addString(node.c_str());
    msg.addList()=grp;

    grp.clear();
    grp.addString("ps");
    msg.addList()=grp;

    printf(":: %s\n", msg.toString().c_str());

    response=sendMsg(msg, node);
    for (size_t i=0; i<response.size(); i++)
    {
        response.get(i).toString();
        processInfo temp;
        yarp::os::Bottle* b = response.get(i).asList();

        temp.pid = b->get(0).asList()->get(1).asInt32();
        temp.tag = b->get(1).asList()->get(1).asString();
        temp.status = b->get(2).asList()->get(1).asString();
        temp.command = b->get(3).asList()->get(1).asString();
        temp.env = b->get(4).asList()->get(1).asString();
        processes.push_back(temp);
    }
    return true;
}

bool yarp::run::Run::which(const std::string &node, const std::string &keyv)
{
    yarp::os::Bottle msg, grp, response;

    grp.clear();
    grp.addString("on");
    grp.addString(node.c_str());
    msg.addList()=grp;

    grp.clear();
    grp.addString("which");
    grp.addString(keyv.c_str());
    msg.addList()=grp;

    printf(":: %s\n", msg.toString().c_str());

    response=sendMsg(msg, node);

    if (!response.size()) {
        return false;
    }

    return true;
}

bool yarp::run::Run::exit(const std::string &node)
{
    yarp::os::Bottle msg, grp, response;

    grp.clear();
    grp.addString("on");
    grp.addString(node.c_str());
    msg.addList()=grp;

    grp.clear();
    grp.addString("exit");
    msg.addList()=grp;

    printf(":: %s\n", msg.toString().c_str());

    response=sendMsg(msg, node);

    if (!response.size()) {
        return false;
    }

    return true;
}
