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

#ifndef YARP_LOGGER
#define YARP_LOGGER

#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Time.h>
#include <yarp/os/Vocab.h>

#include <yarp/os/Thread.h>
#include <yarp/os/RateThread.h>
#include <yarp/os/Semaphore.h>

#include <list>
#include <vector>
#include <string>
#include <ctime>

namespace yarp
{
    namespace yarpLogger
    {
        class  LoggerEngine;
        class  LogEntry;
        class  LogEntryInfo;
        struct MessageEntry;

        enum loglLevelEnum
        {
            LOGLEVEL_UNDEFINED = 0,
            LOGLEVEL_TRACE = 1,
            LOGLEVEL_DEBUG = 2,
            LOGLEVEL_INFO = 3,
            LOGLEVEL_WARNING = 4,
            LOGLEVEL_ERROR = 5,
            LOGLEVEL_FATAL = 6
        };

        class  LogLevel
        {
            private:
            int e_level;

            public:
            LogLevel()
            {
                e_level = 0;
            }
            LogLevel(int l)
            {
                    e_level = l;
            }
            void setLevel(loglLevelEnum level)
            {
                e_level = level;
            }
            void setLevel(int level)
            {
                e_level = level;
            }
            int toInt()
            {
                return e_level;
            }
            std::string toString()
            {
                if (e_level == 0) { return "<UNDEFINED>"; }
                if (e_level == 1) { return "<TRACE>"; }
                if (e_level == 2) { return "<DEBUG>"; }
                if (e_level == 3) { return "<INFO>"; }
                if (e_level == 4) { return "<WARNING>"; }
                if (e_level == 5) { return "<ERROR>"; }
                if (e_level == 6) { return "<FATAL>"; }
                else { return "<UNDEFINED>"; }
            }
            void operator = (loglLevelEnum level)
            {
                e_level = level;
            }
            bool operator == (const LogLevel& other)
            {
                return this->e_level == other.e_level;
            }
            bool operator == (const loglLevelEnum& other)
            {
                return this->e_level == other;
            }
            bool operator > (const LogLevel& other)
            {
                return this->e_level > other.e_level;
            }
        };
        enum   LogSystemEnum
        {
            LOGSYSTEM_YARP    = 0,
            LOGSYSTEM_YARPRUN = 1
        };
    }
}

struct yarp::yarpLogger::MessageEntry
{
    LogLevel      level;
    std::string   text;
    std::string   yarprun_timestamp;
    std::string   local_timestamp;
};

class yarp::yarpLogger::LogEntryInfo
{
    private:
    LogLevel      highest_error;
    unsigned int  number_of_traces;
    unsigned int  number_of_debugs;
    unsigned int  number_of_infos;
    unsigned int  number_of_warnings;
    unsigned int  number_of_errors;
    unsigned int  number_of_fatals;

    public:
    std::string   port_system;
    std::string   port_prefix;
    std::string   port_complete;
    std::string   process_name;
    std::string   process_pid;
    std::string   ip_address;
    std::time_t   last_update;
    unsigned int  logsize;

    LogEntryInfo  ()  {clear();}
    void          clear ();

    LogLevel      getLastError           ();
    void          clearLastError         ();
    void          setNewError            (LogLevel level);
    unsigned int  get_number_of_traces   () { return number_of_traces;   }
    unsigned int  get_number_of_debugs   () { return number_of_debugs;   }
    unsigned int  get_number_of_infos    () { return number_of_infos;    }
    unsigned int  get_number_of_warnings () { return number_of_warnings; }
    unsigned int  get_number_of_errors   () { return number_of_errors;   }
    unsigned int  get_number_of_fatals   () { return number_of_fatals;   }
};

class yarp::yarpLogger::LogEntry
{
    private:
    unsigned int                  entry_list_max_size;
    bool                          entry_list_max_size_enabled;

    public:
    bool                          logging_enabled;
    std::vector<MessageEntry>     entry_list;
    int                           last_read_message;
    void                          clear_logEntries();
    bool                          append_logEntry(MessageEntry entry);

    public:
    LogEntry                       (int _entry_list_max_size=10000) {logging_enabled=true;entry_list_max_size=_entry_list_max_size; last_read_message=-1;entry_list.reserve(entry_list_max_size);};

    int  getLogEntryMaxSize        ()          {return entry_list_max_size;}
    bool getLogEntryMaxSizeEnabled ()          {return entry_list_max_size_enabled;}
    void setLogEntryMaxSize        (int  size);
    void setLogEntryMaxSizeEnabled (bool enable);

    public:
    yarp::yarpLogger::LogEntryInfo logInfo;
};

class yarp::yarpLogger::LoggerEngine
{
    //private class
    class logger_thread : public yarp::os::SystemRateThread
    {
        public:
        logger_thread (std::string _portname, int _rate=10, int _log_list_max_size=100);
        public:
        yarp::os::Semaphore  mutex;
        unsigned int         log_list_max_size;
        bool                 log_list_max_size_enabled;
        std::list<LogEntry>  log_list;
        yarp::os::BufferedPort<yarp::os::Bottle> logger_port;
        std::string          logger_portName;
        int                  unknown_format_received;

        public:
        std::string getPortName();
        void        run() YARP_OVERRIDE;
        void        threadRelease() YARP_OVERRIDE;
        bool        listen_to_LOGLEVEL_UNDEFINED;
        bool        listen_to_LOGLEVEL_TRACE;
        bool        listen_to_LOGLEVEL_DEBUG;
        bool        listen_to_LOGLEVEL_INFO;
        bool        listen_to_LOGLEVEL_WARNING;
        bool        listen_to_LOGLEVEL_ERROR;
        bool        listen_to_LOGLEVEL_FATAL;
        bool        listen_to_YARP_MESSAGES;
        bool        listen_to_YARPRUN_MESSAGES;
    };

    private:
    bool           logging;
    bool           discovering;
    logger_thread* log_updater;

    public:
    void discover             (std::list<std::string>& ports);
    void connect              (const std::list<std::string>& ports);

    public:
    LoggerEngine                 (std::string portName);
    ~LoggerEngine                ();
    bool start_logging           ();
    bool stop_logging            ();
    void start_discover          ();
    void stop_discover           ();
    bool is_logging              () {return logging;}
    bool is_discovering          () {return discovering;}
    bool clear                   ();

    bool save_all_logs_to_file           (std::string  filename);
    bool load_all_logs_from_file         (std::string  filename);
    bool export_log_to_text_file         (std::string  filename, std::string portname);
    int  get_num_of_processes            ();
    void get_infos                       (std::list<LogEntryInfo>&   infos);
    void get_messages                    (std::list<MessageEntry>& messages);
    void get_messages_by_port_prefix     (std::string  port,    std::list<MessageEntry>& messages, bool from_beginning = false);
    void get_messages_by_port_complete   (std::string  port,    std::list<MessageEntry>& messages, bool from_beginning = false);
    void get_messages_by_process         (std::string  process, std::list<MessageEntry>& messages, bool from_beginning = false);
    void get_messages_by_pid             (std::string  pid,     std::list<MessageEntry>& messages, bool from_beginning = false);
    void clear_messages_by_port_complete (std::string  port);
    void set_log_enable_by_port_complete (std::string  port, bool enable);
    bool get_log_enable_by_port_complete (std::string  port);

    void set_listen_option               (LogLevel      logLevel,  bool enable);
    void set_listen_option               (std::string   option,    bool enable);
    void set_listen_option               (LogSystemEnum logSystem, bool enable);
    bool get_listen_option               (LogLevel      logLevel);
    bool get_listen_option               (std::string   option);
    bool get_listen_option               (LogSystemEnum logSystem);

    void set_log_lines_max_size          (bool  enabled,  int new_size);
    void set_log_list_max_size           (bool  enabled,  int new_size);
    void get_log_lines_max_size          (bool& enabled, int& current_size);
    void get_log_list_max_size           (bool& enabled, int& current_size);

    std::list<MessageEntry> filter_by_level (int level, const std::list<MessageEntry>& messages);
};

#endif
