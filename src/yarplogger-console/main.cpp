/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/RFModule.h>
#include <yarp/logger/YarpLogger.h>

#include <string>
#include <cstdio>

using namespace yarp::os;
using namespace yarp::yarpLogger;

class logger_module : public yarp::os::RFModule
{
    LoggerEngine* the_logger = nullptr;

    protected:
    yarp::os::Port rpcPort;

    bool configure(yarp::os::ResourceFinder &rf) override
    {
        the_logger = new LoggerEngine ("/logger");

        rpcPort.open("/logger/rpc:i");
        attach(rpcPort);
        //attachTerminal();

        the_logger->start_logging();
        return true;
    }

    bool interruptModule() override
    {
        rpcPort.interrupt();
        return true;
    }

    bool close() override
    {
        rpcPort.interrupt();
        rpcPort.close();

        if (the_logger)
        {
            delete the_logger;
            the_logger=nullptr;
        }
        return true;
    }

    double getPeriod() override
    {
        return 10.0;
    }

    bool updateModule() override
    {
        printf("logger running, listening to %d ports\n",the_logger->get_num_of_processes());
        return true;
    }

    bool respond(const yarp::os::Bottle& command,yarp::os::Bottle& reply) override
    {
        reply.clear();
        if (command.get(0).asString()=="quit")
        {
            reply.addString("ack");
            return false;
        }
        else if (command.get(0).asString()=="start")
        {
            if (the_logger->is_logging()==false)
            {
                this->the_logger->start_logging();
            }
            reply.addString("ack");
        }
        else if (command.get(0).asString()=="stop")
        {
            if (the_logger->is_logging()==true)
            {
                this->the_logger->stop_logging();
            }
            reply.addString("ack");
        }
        else if (command.get(0).asString()=="save")
        {
            std::string filename = command.get(1).asString();
            this->the_logger->save_all_logs_to_file(filename);
            reply.addString("ack");
        }
        else if (command.get(0).asString()=="load")
        {
            std::string filename = command.get(1).asString();
            this->the_logger->load_all_logs_from_file(filename);
            reply.addString("ack");
        }
        else if (command.get(0).asString()=="ask_by_proc")
        {
            std::string proc_name = command.get(1).asString();
            std::list<MessageEntry> m;
            the_logger->get_messages_by_process(proc_name, m);
            std::list<MessageEntry>::iterator it;
            for (it = m.begin(); it != m.end(); it++)
                printf(" %s %d %s \n",it->yarprun_timestamp.c_str(), it->level.toInt(), it->text.c_str());
            reply.addString("ack");
        }
        else if (command.get(0).asString()=="ask_all")
        {
             std::list<MessageEntry> m;
             the_logger->get_messages(m);
             std::list<MessageEntry>::iterator it;
             for (it = m.begin(); it != m.end(); it++)
                 printf(" %s %d %s \n",it->yarprun_timestamp.c_str(), it->level.toInt(), it->text.c_str());
             reply.addString("ack");
        }
        else if (command.get(0).asString()=="discover")
        {
             std::list<std::string> ports;
             the_logger->discover(ports);
             reply.addString("ack");
        }
        else if (command.get(0).asString()=="connect")
        {
             std::list<std::string> ports;
             the_logger->discover(ports);
             the_logger->connect(ports);
             reply.addString("ack");
        }
        else if (command.get(0).asString()=="get_info")
        {
             std::list<LogEntryInfo> infos;
             the_logger->get_infos(infos);
             std::list<LogEntryInfo>::iterator it;
             for (it = infos.begin(); it != infos.end(); it++)
             {
                 std::tm* tm = localtime(&it->last_update);
                 if (tm)
                 printf("%s %s hour:%d minute:%d sec:%d \n",it->port_prefix.c_str(), it->port_complete.c_str(), tm->tm_hour,tm->tm_min, tm->tm_sec);
                 else
                 printf("%s %s no data received yet \n",it->port_prefix.c_str(), it->port_complete.c_str());
             }
             reply.addString("ack");
        }
        else
        {
            reply.addString("nack");
        }
        return true;
    }
};

int main(int argc, char *argv[])
{
    yarp::os::Network yarp(yarp::os::YARP_CLOCK_SYSTEM);
    if (!yarp.checkNetwork())
    {
        fprintf(stderr,"ERROR: check YARP network.\n");
        return -1;
    }

    yarp::os::ResourceFinder rf;
    rf.setDefaultConfigFile("yarprunLogger.ini");           //overridden by --from parameter
    rf.setDefaultContext("yarprunLogger");                  //overridden by --context parameter
    rf.configure(argc,argv);

    logger_module icub_logger;
    return icub_logger.runModule(rf);
}
