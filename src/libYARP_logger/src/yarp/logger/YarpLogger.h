/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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
#include <yarp/os/PeriodicThread.h>

#include <list>
#include <mutex>
#include <vector>
#include <string>
#include <ctime>

namespace yarp::yarpLogger {
class LoggerEngine;
class LogEntry;
class LogEntryInfo;
struct MessageEntry;

enum LogLevelEnum
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
    LogLevelEnum e_level {LOGLEVEL_UNDEFINED};

public:
    LogLevel() = default;

    LogLevel(int l) :
            e_level(static_cast<LogLevelEnum>(l))
    {
    }

    void setLevel(LogLevelEnum level)
    {
        e_level = level;
    }

    void setLevel(int level)
    {
        e_level = static_cast<LogLevelEnum>(level);
    }

    int toInt()
    {
        return static_cast<int>(e_level);
    }

    std::string_view toString() const
    {
        switch(e_level) {
        case LOGLEVEL_TRACE:
            return "TRACE";
        case LOGLEVEL_DEBUG:
            return "DEBUG";
        case LOGLEVEL_INFO:
            return "INFO";
        case LOGLEVEL_WARNING:
            return "WARNING";
        case LOGLEVEL_ERROR:
            return "ERROR";
        case LOGLEVEL_FATAL:
            return "FATAL";
        case LOGLEVEL_UNDEFINED:
        default:
            return "UNDEFINED";
        }
    }

    void operator=(LogLevelEnum level)
    {
        e_level = level;
    }

    explicit operator LogLevelEnum() const
    {
        return e_level;
    }

    bool operator==(const LogLevel& other) const
    {
        return this->e_level == other.e_level;
    }
    bool operator==(const LogLevelEnum& other) const
    {
        return this->e_level == other;
    }
    bool operator>(const LogLevel& other) const
    {
        return this->e_level > other.e_level;
    }
};

enum LogSystemEnum
{
    LOGSYSTEM_YARP    = 0,
    LOGSYSTEM_YARPRUN = 1
};
} // namespace yarp::yarpLogger

struct yarp::yarpLogger::MessageEntry
{
    LogLevel      level;
    std::string   text;
    std::string   filename;
    unsigned int  line;
    std::string   function;
    std::string   hostname;
    std::string   cmd;
    std::string   args;
    int           pid;
    long          thread_id;
    std::string   component;
    double        systemtime;
    double        networktime;
    double        externaltime;
    std::string   backtrace;
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
    std::string   port_system = "null";
    std::string   port_prefix = "null";
    std::string   port_complete = "null";
    std::string   process_name = "null";
    std::string   process_pid = "null";
    std::string   ip_address = "null";
    std::time_t   last_update = 0;
    unsigned int  logsize = 0;

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
    LogEntry(bool _max_size_enabled, int _entry_list_max_size) :
        entry_list_max_size(_entry_list_max_size),
        entry_list_max_size_enabled(_max_size_enabled),
        logging_enabled(true),
        last_read_message(-1)
    {
        entry_list.reserve(entry_list_max_size);
    }

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
    class logger_thread : public yarp::os::PeriodicThread
    {
        public:
        logger_thread (std::string _portname, double _period=0.01, int _log_list_max_size=100, int _logs_max_lines=10000);
        public:
        std::mutex           mutex;
        unsigned int         log_list_max_size;
        bool                 log_list_max_size_enabled;
        std::list<LogEntry>  log_list;
        unsigned int         logs_max_lines;
        bool                 logs_max_lines_enabled;
        yarp::os::BufferedPort<yarp::os::Bottle> logger_port;
        std::string          logger_portName;
        int                  unknown_format_received;

        public:
        std::string getPortName();
        void        run() override;
        void        threadRelease() override;
        bool        listen_to_LOGLEVEL_UNDEFINED = true;
        bool        listen_to_LOGLEVEL_TRACE = true;
        bool        listen_to_LOGLEVEL_DEBUG = true;
        bool        listen_to_LOGLEVEL_INFO = true;
        bool        listen_to_LOGLEVEL_WARNING = true;
        bool        listen_to_LOGLEVEL_ERROR = true;
        bool        listen_to_LOGLEVEL_FATAL = true;
        bool        listen_to_YARP_MESSAGES = true;
        bool        listen_to_YARPRUN_MESSAGES = true;
    };

    private:
    bool           logging;
    bool           discovering;
    logger_thread* log_updater = nullptr;

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
