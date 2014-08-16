/* 
 * Copyright (C)2014  iCub Facility - Istituto Italiano di Tecnologia
 * Author: Marco Randazzo
 * email:  marco.randazzo@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/

#ifndef _YARPRUN_LOGGER_
#define _YARPRUN_LOGGER_

#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>
#include <yarp/os/Vocab.h>

#include <yarp/os/Thread.h>
#include <yarp/os/RateThread.h>
#include <yarp/os/Semaphore.h>

#include <list>
#include <string>
#include <ctime>

namespace yarp
{
  namespace os
  {
    namespace YarprunLogger
    {
        class  LoggerEngine;
        class  LogEntry;
        class  LogEntryInfo;
        struct MessageEntry;
        //enum   LevelEnum;
    }
  }
}

/*enum yarp::os::YarprunLogger::LevelEnum
{
    info = 0,
    debug = 1,
    warn = 2,
    error =3,
    critical =4
};*/

struct yarp::os::YarprunLogger::MessageEntry
{
    int         level;
    std::string text;
    std::string timestamp;
};

class yarp::os::YarprunLogger::LogEntryInfo
{
    public:
    std::string  port_prefix;
    std::string  port_complete;
    std::string  process_name;
    std::string  process_pid;
    std::string  ip_address;
    std::time_t  last_update;
    int          logsize;
    int          number_of_errors;
    int          number_of_warnings;
    int          number_of_debugs;
    int          number_of_infos;
    LogEntryInfo()  {logsize=0;number_of_errors=0;number_of_warnings=0;number_of_debugs=0;number_of_infos=0;}
};

class yarp::os::YarprunLogger::LogEntry
{
    private:
    int                     entry_list_max_size;

    public:
    std::list<MessageEntry> entry_list;
    std::list<MessageEntry>::iterator last_read_message;
    void clear_logEntries();
    bool append_logEntry(MessageEntry entry);

    public:
    LogEntry() {entry_list_max_size=1000;};
    void setLogEntryMaxSize (int size);
    int  getLogEntryMaxSize () {return entry_list_max_size;}

    public:
    yarp::os::YarprunLogger::LogEntryInfo logInfo;
};

class yarp::os::YarprunLogger::LoggerEngine
{
    //private class
    class logger_thread : public RateThread
    {
        public:
        logger_thread (int _rate, std::string _portname, int _log_list_max_size=100) : RateThread(_rate) {portName=_portname; log_list_max_size=_log_list_max_size;};

        public:
        yarp::os::Semaphore mutex;
        int                 log_list_max_size;
        std::list<LogEntry> log_list;
        Port logger_port;
        std::string portName;
    
        public:
        std::string getPortName();
        void        run();
        void        threadRelease();
    };

    private:
    bool           logging;
    bool           discovering;
    logger_thread* log_updater;
    
    public:
    void discover             (std::list<std::string>& ports);
    void connect              (const std::list<std::string>& ports);
    
    public:
    LoggerEngine(std::string portName);
    ~LoggerEngine();
    bool start_logging           ();
    void stop_logging            ();
    void start_discover          ();
    void stop_discover           ();
    bool is_logging              () {return logging;}
    bool is_discovering          () {return discovering;}

    void save_all_logs_to_file           (std::string  filename);
    void load_all_logs_from_file         (std::string  filename);
    void export_log_to_text_file         (std::string  filename, std::string portname);
    int  get_num_of_processes            ();
    void get_infos                       (std::list<LogEntryInfo>&   infos);
    void get_messages                    (std::list<MessageEntry>& messages);
    void get_messages_by_port_prefix     (std::string  port,    std::list<MessageEntry>& messages, bool from_beginning = false);
    void get_messages_by_port_complete   (std::string  port,    std::list<MessageEntry>& messages, bool from_beginning = false);
    void get_messages_by_process         (std::string  process, std::list<MessageEntry>& messages, bool from_beginning = false);
    void get_messages_by_pid             (std::string  pid,     std::list<MessageEntry>& messages, bool from_beginning = false);
    void clear_messages_by_port_complete (std::string  port);
    
    std::list<MessageEntry> filter_by_level (int level, const std::list<MessageEntry>& messages);
};

#endif