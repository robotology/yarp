/*
 * Copyright (C) 2014 Istituto Italiano di Tecnologia (IIT)
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

#include <yarp/os/RpcClient.h>
#include <yarp/os/SystemClock.h>
#include <yarp/logger/YarpLogger.h>

#include <iostream>
#include <cstring>
#include <string>
#include <sstream>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <algorithm>

using namespace yarp::os;
using namespace yarp::yarpLogger;
using namespace std;
/*
const std::string RED    ="\033[01;31m";
const std::string GREEN  ="\033[01;32m";
const std::string YELLOW ="\033[01;33m";
const std::string BLUE   ="\033[01;34m";
const std::string CLEAR  ="\033[00m";

const std::string RED_ERROR      = RED+"ERROR"+CLEAR;
const std::string YELLOW_WARNING = YELLOW+"WARNING"+CLEAR;
*/

LogEntry::LogEntry(int _entry_list_max_size)
: entry_list_max_size(_entry_list_max_size)
, logging_enabled(true)
, last_read_message(-1)
, entry_list_max_size_enabled(true)
{
}

void LogEntry::clear_logEntries()
{
    entry_list.clear();
    logInfo.clear();
    last_read_message=-1;
}

void LogEntry::setLogEntryMaxSize(int size)
{
    entry_list_max_size = size;
    clear_logEntries();
}

void LogEntry::setLogEntryMaxSizeEnabled (bool enable)
{
    entry_list_max_size_enabled=enable;
}

bool LogEntry::append_logEntry(const MessageEntry& entry)
{
    if (entry_list.size() >= entry_list_max_size && entry_list_max_size_enabled)
    {
        //printf("WARNING: exceeded entry_list_max_size=%d\n",entry_list_max_size);
        std::pair<size_t, size_t> removedRows = std::make_pair(0, 1);
        for (auto& observer : observers) {
            observer->logEntryWillRemoveRows(*this, removedRows);
        }
        entry_list.pop_front();
        for (auto& observer : observers) {
            observer->logEntryDidRemoveRows(*this, removedRows);
        }
    }
    std::pair<size_t, size_t> addedRows = std::make_pair(entry_list.size(), 1);
    for (auto& observer : observers) {
        observer->logEntryWillAddRows(*this, addedRows);
    }
    entry_list.push_back(entry);
    logInfo.logsize = entry_list.size();
    for (auto& observer : observers) {
        observer->logEntryDidAddRows(*this, addedRows);
    }
    return true;
}

void LogEntryInfo::clear()
{
    logsize = 0;
    number_of_traces=0;
    number_of_debugs=0;
    number_of_infos=0;
    number_of_warnings=0;
    number_of_errors=0;
    number_of_fatals=0;
    highest_error=LOGLEVEL_UNDEFINED;
    last_update=-1;
}

LogLevel LogEntryInfo::getLastError()
{
    return highest_error;
}

void LogEntryInfo::clearLastError()
{
    highest_error=LOGLEVEL_UNDEFINED;
}

void LogEntryInfo::setNewError(LogLevel level)
{
    if      (level==LOGLEVEL_TRACE)   number_of_traces++;
    else if (level==LOGLEVEL_DEBUG)   number_of_debugs++;
    else if (level==LOGLEVEL_INFO)    number_of_infos++;
    else if (level==LOGLEVEL_WARNING) number_of_warnings++;
    else if (level==LOGLEVEL_ERROR)   number_of_errors++;
    else if (level==LOGLEVEL_FATAL)   number_of_fatals++;
    if (level>highest_error) highest_error=level;
}

void LoggerEngine::discover  (std::list<std::string>& ports)
{
    RpcClient p;
    string logger_portname = log_updater->getPortName();
    p.open(logger_portname+"/discover");
    std::string yarpservername = yarp::os::Network::getNameServerName();
    yarp::os::Network::connect(logger_portname+"/discover",yarpservername.c_str());
    Bottle cmd,response;
    cmd.addString("bot");
    cmd.addString("list");
    p.write(cmd,response);
    printf ("%s\n\n", response.toString().c_str());
    int size = response.size();
    for (int i=1; i<size; i++) //beware: skip i=0 is intentional!
    {
        Bottle* n1 = response.get(i).asList();
        if (n1 && n1->get(0).toString()=="port")
        {
            Bottle* n2 = n1->get(1).asList();
            if (n2 && n2->get(0).toString()=="name")
            {
                char* log_off = nullptr;
                char* yarprun_log_off = nullptr;
                log_off = std::strstr((char*)(n2->get(1).toString().c_str()), "/log/");
                if (log_off)
                {
                    std::string logport = n2->get(1).toString();
                    printf ("%s\n", logport.c_str());
                    ports.push_back(logport);
                }
                yarprun_log_off = std::strstr((char*)(n2->get(1).toString().c_str()), "/yarprunlog/");
                if (yarprun_log_off)
                {
                    std::string logport = n2->get(1).toString();
                    printf ("%s\n", logport.c_str());
                    ports.push_back(logport);
                }
            }
        }
    }

    std::list<std::string>::iterator ports_it;
    for (ports_it=ports.begin(); ports_it!=ports.end(); ports_it++)
    {
        LogEntry entry;
        entry.logInfo.port_complete = (*ports_it);
        yarp::os::Contact contact = yarp::os::Network::queryName(entry.logInfo.port_complete);
        if (contact.isValid())
        {
            entry.logInfo.ip_address = contact.getHost();
        }
        else
        {
            printf("ERROR: invalid contact: %s\n", entry.logInfo.port_complete.c_str());
        }
        std::istringstream iss(*ports_it);
        std::string token;
        getline(iss, token, '/');
        getline(iss, token, '/');
        getline(iss, token, '/'); entry.logInfo.port_prefix  = "/"+ token;
        getline(iss, token, '/'); entry.logInfo.process_name = token;
        getline(iss, token, '/'); entry.logInfo.process_pid  = token;

        std::list<LogEntry>::iterator it;
        this->log_updater->mutex.wait();
        bool found = false;
        for (it = this->log_updater->log_list.begin(); it != log_updater->log_list.end(); it++)
        {
            if (it->logInfo.port_complete==entry.logInfo.port_complete)
            {
                found=true; break;
            }
        }
        if (found==false)
        {
            log_updater->log_list.push_back(entry);
        }
        this->log_updater->mutex.post();
    }
}

void LoggerEngine::connect (const std::list<std::string>& ports)
{
    yarp::os::ContactStyle style;
    style.timeout=1.0;
    style.quiet=true;

    std::list<std::string>::const_iterator it;
    for (it = ports.begin(); it != ports.end(); it++)
    {
        if (yarp::os::Network::exists(it->c_str(),style) == true)
        {
            yarp::os::Network::connect(it->c_str(),this->log_updater->getPortName().c_str());
        }
        else
        {
            //fprintf(stderr,"unable to connect to port %s\n",it->c_str());
        }
    }
}

std::string LoggerEngine::logger_thread::getPortName()
{
    return logger_portName;
}

LoggerEngine::logger_thread::logger_thread (std::string _portname, int _rate,  int _log_list_max_size) : SystemRateThread(_rate)
{
        logger_portName              = _portname;
        log_list_max_size            = _log_list_max_size;
        log_list_max_size_enabled    = true;
        listen_to_LOGLEVEL_UNDEFINED = true;
        listen_to_LOGLEVEL_TRACE     = true;
        listen_to_LOGLEVEL_DEBUG     = true;
        listen_to_LOGLEVEL_INFO      = true;
        listen_to_LOGLEVEL_WARNING   = true;
        listen_to_LOGLEVEL_ERROR     = true;
        listen_to_LOGLEVEL_FATAL     = true;
        listen_to_YARP_MESSAGES      = true;
        listen_to_YARPRUN_MESSAGES   = true;
        unknown_format_received      = 0;
}

void LoggerEngine::logger_thread::run()
{
    //if (is_discovering()==true)
    {
        //yarp::os::Network::
    }

    //yarp::os::SystemClock::delaySystem(0.001);
    //if (logger_port.getInputCount()>0)
    {
        int bufferport_size = logger_port.getPendingReads();

        while (bufferport_size>0)
        {
            std::time_t machine_current_time = std::time(nullptr);
            char machine_current_time_c [50];
            //strftime(machine_current_time_s, 20, "%Y-%m-%d %H:%M:%S", localtime(&machine_current_time));
            static double d_time_i = yarp::os::SystemClock::nowSystem();
            double d_time = yarp::os::SystemClock::nowSystem() - d_time_i;
            sprintf(machine_current_time_c,"%f",d_time);
            string machine_current_time_s = string(machine_current_time_c);

            Bottle *b = logger_port.read(); //this is blocking
            bufferport_size = logger_port.getPendingReads();

            if (b==nullptr)
            {
                fprintf (stderr, "ERROR: something strange happened here, bufferport_size = %d!\n",bufferport_size);
                return;
            }

            if (b->size()!=2)
            {
                fprintf (stderr, "ERROR: unknown log format!\n");
                unknown_format_received++;
                continue;
            }

            std::string bottlestring = b->toString();
            std::string header;

            if (b->get(0).isString())
            {
                header = b->get(0).asString();
            }
            else
            {
                fprintf(stderr, "ERROR: unknown log format!\n");
                unknown_format_received++;
                continue;
            }

            MessageEntry body;
            std::string s;

            if (b->get(1).isString())
            {
                s = b->get(1).asString();
            }
            else
            {
                fprintf(stderr, "ERROR: unknown log format!\n");
                unknown_format_received++;
                continue;
            }

            body.text = s;
            char ttstr [20];
            static int count=0;
            sprintf(ttstr,"%d",count++);
            body.yarprun_timestamp = string(ttstr);
            body.local_timestamp   = machine_current_time_s;
            body.level = LOGLEVEL_UNDEFINED;

            size_t str = s.find('[',0);
            size_t end = s.find(']',0);
            if (str==std::string::npos || end==std::string::npos )
            {
                body.level = LOGLEVEL_UNDEFINED;
            }
            else if (str==0)
            {
                std::string level = s.substr(str,end+1);
                body.level = LOGLEVEL_UNDEFINED;
                if      (level.find("TRACE")!=std::string::npos)   body.level = LOGLEVEL_TRACE;
                else if (level.find("DEBUG")!=std::string::npos)   body.level = LOGLEVEL_DEBUG;
                else if (level.find("INFO")!=std::string::npos)    body.level = LOGLEVEL_INFO;
                else if (level.find("WARNING")!=std::string::npos) body.level = LOGLEVEL_WARNING;
                else if (level.find("ERROR")!=std::string::npos)   body.level = LOGLEVEL_ERROR;
                else if (level.find("FATAL")!=std::string::npos)   body.level = LOGLEVEL_FATAL;
                body.text = s.substr(end+1);
            }
            else
            {
                body.level = LOGLEVEL_UNDEFINED;
            }

            if (body.level == LOGLEVEL_UNDEFINED && listen_to_LOGLEVEL_UNDEFINED == false) {continue;}
            if (body.level == LOGLEVEL_TRACE     && listen_to_LOGLEVEL_TRACE     == false) {continue;}
            if (body.level == LOGLEVEL_DEBUG     && listen_to_LOGLEVEL_DEBUG     == false) {continue;}
            if (body.level == LOGLEVEL_INFO      && listen_to_LOGLEVEL_INFO      == false) {continue;}
            if (body.level == LOGLEVEL_WARNING   && listen_to_LOGLEVEL_WARNING   == false) {continue;}
            if (body.level == LOGLEVEL_ERROR     && listen_to_LOGLEVEL_ERROR     == false) {continue;}
            if (body.level == LOGLEVEL_FATAL     && listen_to_LOGLEVEL_FATAL     == false) {continue;}

            this->mutex.wait();
            LogEntry entry;
            entry.logInfo.port_complete = header;
            entry.logInfo.port_complete.erase(0,1);
            entry.logInfo.port_complete.erase(entry.logInfo.port_complete.size()-1);
            std::istringstream iss(header);
            std::string token;
            getline(iss, token, '/');
            getline(iss, token, '/'); entry.logInfo.port_system  = token;
            getline(iss, token, '/'); entry.logInfo.port_prefix  = "/"+ token;
            getline(iss, token, '/'); entry.logInfo.process_name = token;
            getline(iss, token, '/'); entry.logInfo.process_pid  = token.erase(token.size()-1);
            if (entry.logInfo.port_system == "log" && listen_to_YARP_MESSAGES==false)    continue;
            if (entry.logInfo.port_system == "yarprunlog" && listen_to_YARPRUN_MESSAGES==false) continue;

            std::list<LogEntry>::iterator it;
            for (it = log_list.begin(); it != log_list.end(); it++)
            {
                if (it->logInfo.port_complete==entry.logInfo.port_complete)
                {
                    if (it->logging_enabled)
                    {
                        it->logInfo.setNewError(body.level);
                        it->logInfo.last_update=machine_current_time;
                        it->append_logEntry(body);
                    }
                    else
                    {
                        //just skipping this message
                    }
                    break;
                }
            }
            if (it == log_list.end())
            {
                if (log_list.size() < log_list_max_size || log_list_max_size_enabled==false )
                {
                    yarp::os::Contact contact = yarp::os::Network::queryName(entry.logInfo.port_complete);
                    if (contact.isValid())
                    {
                        entry.logInfo.setNewError(body.level);
                        entry.logInfo.ip_address = contact.getHost();
                    }
                    else
                    {
                        printf("ERROR: invalid contact: %s\n", entry.logInfo.port_complete.c_str());
                    };
                    entry.append_logEntry(body);
                    entry.logInfo.last_update=machine_current_time;
                    log_list.push_back(entry);
                }
                //else
                //{
                //    printf("WARNING: exceeded log_list_max_size=%d\n",log_list_max_size);
                //}
            }

            this->mutex.post();
        }
    }

}

//public methods
bool LoggerEngine::start_logging()
{
    if (logging == true)
    {
        fprintf(stderr,"logger is already running, listening on port %s\n", log_updater->getPortName().c_str());
        return true;
    }

    if (log_updater->logger_port.open(log_updater->getPortName().c_str()))
    {
        fprintf(stdout,"Logger successfully started, listening on port %s\n", log_updater->getPortName().c_str());
    }
    else
    {
        fprintf(stderr,"Unable to start logger: port %s is unavailable\n", log_updater->getPortName().c_str());
        return false;
    }
    log_updater->logger_port.resume();
    log_updater->logger_port.setStrict();
    logging=true;
    log_updater->start();
    return true;
}

void LoggerEngine::logger_thread::threadRelease()
{
    logger_port.interrupt();
    logger_port.close();
}

bool LoggerEngine::stop_logging()
{
    if (log_updater == nullptr) return false;

    logging=false;
    if (log_updater->isRunning()==true) log_updater->stop();
    return true;
}

void LoggerEngine::start_discover()
{
    if (log_updater == nullptr) return;

    log_updater->start();
    discovering=true;
}

void LoggerEngine::stop_discover()
{
    discovering=false;
}

LoggerEngine::LoggerEngine(std::string portName)
{
    int thread_rate = 10;
    log_updater=new logger_thread (portName,thread_rate);
    logging = false;
    discovering = false;
}

LoggerEngine::~LoggerEngine()
{
    this->stop_logging();
    if (log_updater!=nullptr)
    {
        delete log_updater;
        log_updater = nullptr;
    }
}

int  LoggerEngine::get_num_of_processes()
{
    if (log_updater == nullptr) return 0;

    return log_updater->logger_port.getInputCount();
}

void LoggerEngine::get_infos (std::list<LogEntryInfo>& infos)
{
    if (log_updater == nullptr) return;

    log_updater->mutex.wait();
    std::list<LogEntry>::iterator it;
    for (it = log_updater->log_list.begin(); it != log_updater->log_list.end(); it++)
    {
        infos.push_back(it->logInfo);
    }
    log_updater->mutex.post();
}

void LoggerEngine::get_messages (std::list<MessageEntry>& messages)
{
    if (log_updater == nullptr) return;

    log_updater->mutex.wait();
    std::list<LogEntry>::iterator it;
    for (it = log_updater->log_list.begin(); it != log_updater->log_list.end(); it++)
    {
        messages.insert(messages.end(), it->entry_list.begin(), it->entry_list.end());
    }
    log_updater->mutex.post();
}

void LoggerEngine::get_messages_by_port_prefix    (std::string  port,  std::list<MessageEntry>& messages,  bool from_beginning)
{
    if (log_updater == nullptr) return;

    log_updater->mutex.wait();
    std::list<LogEntry>::iterator it;
    for (it = log_updater->log_list.begin(); it != log_updater->log_list.end(); it++)
    {
        if (it->logInfo.port_prefix == port)
        {
            //messages = (it->entry_list);
            if (it->last_read_message==-1)
            {
                from_beginning=true;
            }
            if (from_beginning==true)
            {
                it->last_read_message = 0;
            }
            int i=it->last_read_message;
            for (; i<(int)it->entry_list.size(); i++)
            {
                messages.push_back(it->entry_list[i]);
            }
            it->last_read_message=i;
            break;
        }
    }
    log_updater->mutex.post();
}

void LoggerEngine::clear_messages_by_port_complete    (std::string  port)
{
    if (log_updater == nullptr) return;

    log_updater->mutex.wait();
    std::list<LogEntry>::iterator it;
    for (it = log_updater->log_list.begin(); it != log_updater->log_list.end(); it++)
    {
        if (it->logInfo.port_complete == port)
           {
               it->clear_logEntries();
               break;
           }
    }
    log_updater->mutex.post();
}

void LoggerEngine::get_messages_by_port_complete    (std::string  port,  std::list<MessageEntry>& messages,  bool from_beginning) const
{
    if (log_updater == nullptr) return;

    log_updater->mutex.wait();
    std::list<LogEntry>::iterator it;
    for (it = log_updater->log_list.begin(); it != log_updater->log_list.end(); it++)
    {
        if (it->logInfo.port_complete == port)
        {
            //messages = (it->entry_list);
            if (it->last_read_message==-1)
            {
                from_beginning=true;
            }
            if (from_beginning==true)
            {
                it->last_read_message = 0;
            }
            int i=it->last_read_message;
            int size = (int)it->entry_list.size();
            for (; i<size; i++)
            {
                messages.push_back(it->entry_list[i]);
            }
            it->last_read_message=i;
            break;
        }
    }
    log_updater->mutex.post();
}

bool LoggerEngine::getLogEntryByPortComplete(const std::string& port, LogEntry*& entry)
{
    if (log_updater == nullptr) return false;

    log_updater->mutex.wait();
    for (std::list<LogEntry>::iterator it(log_updater->log_list.begin());
         it != log_updater->log_list.end(); ++it)
    {
        if (it->logInfo.port_complete == port)
        {
            entry = &(*it);
            log_updater->mutex.post();
            return true;
        }
    }
    log_updater->mutex.post();
    return false;
}

void LoggerEngine::get_messages_by_process (std::string  process,  std::list<MessageEntry>& messages,  bool from_beginning)
{
    if (log_updater == nullptr) return;

    log_updater->mutex.wait();
    std::list<LogEntry>::iterator it;
    for (it = log_updater->log_list.begin(); it != log_updater->log_list.end(); it++)
    {
        if (it->logInfo.process_name == process)
        {
            //messages = (it->entry_list);
            if (it->last_read_message==-1)
            {
                from_beginning=true;
            }
            if (from_beginning==true)
            {
                it->last_read_message = 0;
            }
            int i=it->last_read_message;
            for (; i<(int)it->entry_list.size(); i++)
            {
                messages.push_back(it->entry_list[i]);
            }
            it->last_read_message=i;
            break;
        }
    }
    log_updater->mutex.post();
}

void LoggerEngine::get_messages_by_pid     (std::string pid, std::list<MessageEntry>& messages,  bool from_beginning)
{
    if (log_updater == nullptr) return;

    log_updater->mutex.wait();
    std::list<LogEntry>::iterator it;
    for (it = log_updater->log_list.begin(); it != log_updater->log_list.end(); it++)
    {
        if (it->logInfo.process_pid == pid)
        {
            //messages = (it->entry_list);
            if (it->last_read_message==-1)
            {
                from_beginning=true;
            }
            if (from_beginning==true)
            {
                it->last_read_message = 0;
            }
            int i=it->last_read_message;
            for (; i<(int)it->entry_list.size(); i++)
            {
                messages.push_back(it->entry_list[i]);
            }
            it->last_read_message=i;
            break;
        }
    }
    log_updater->mutex.post();
}

const std::list<MessageEntry> filter_by_level (int level, const std::list<MessageEntry>& messages)
{
    std::list<MessageEntry> ret;
    std::list<MessageEntry>::const_iterator it;
    for (it = messages.begin(); it != messages.end(); it++)
    {
        LogLevel llevel = it->level;
        if (llevel.toInt() == level)
            ret.push_back(*it);
    }
    return ret;
}

void LoggerEngine::set_listen_option               (LogLevel logLevel, bool enable)
{
    if (log_updater == nullptr) return;
    log_updater->mutex.wait();
    if      (logLevel == LOGLEVEL_UNDEFINED)  {log_updater->listen_to_LOGLEVEL_UNDEFINED=enable;}
    else if (logLevel == LOGLEVEL_TRACE)      {log_updater->listen_to_LOGLEVEL_TRACE=enable;}
    else if (logLevel == LOGLEVEL_INFO)       {log_updater->listen_to_LOGLEVEL_INFO=enable;}
    else if (logLevel == LOGLEVEL_DEBUG)      {log_updater->listen_to_LOGLEVEL_DEBUG=enable;}
    else if (logLevel == LOGLEVEL_WARNING)    {log_updater->listen_to_LOGLEVEL_WARNING=enable;}
    else if (logLevel == LOGLEVEL_ERROR)      {log_updater->listen_to_LOGLEVEL_ERROR=enable;}
    else if (logLevel == LOGLEVEL_FATAL)      {log_updater->listen_to_LOGLEVEL_FATAL=enable;}
    log_updater->mutex.post();
}

bool LoggerEngine::get_listen_option               (LogLevel logLevel)
{
    if (log_updater == nullptr) return false;
    if (logLevel == LOGLEVEL_UNDEFINED) {return log_updater->listen_to_LOGLEVEL_UNDEFINED;}
    if (logLevel == LOGLEVEL_TRACE)     {return log_updater->listen_to_LOGLEVEL_TRACE;}
    if (logLevel == LOGLEVEL_DEBUG)     {return log_updater->listen_to_LOGLEVEL_DEBUG;}
    if (logLevel == LOGLEVEL_INFO)      {return log_updater->listen_to_LOGLEVEL_INFO;}
    if (logLevel == LOGLEVEL_WARNING)   {return log_updater->listen_to_LOGLEVEL_WARNING;}
    if (logLevel == LOGLEVEL_ERROR)     {return log_updater->listen_to_LOGLEVEL_ERROR;}
    if (logLevel == LOGLEVEL_FATAL)     {return log_updater->listen_to_LOGLEVEL_FATAL;}
    return false;
}

void LoggerEngine::set_listen_option               (std::string   option, bool enable)
{
    if (log_updater == nullptr) return;
    log_updater->mutex.wait();
    log_updater->mutex.post();
}

bool LoggerEngine::get_listen_option               (std::string   option)
{
    return false;
}

void LoggerEngine::set_listen_option               (LogSystemEnum   logSystem, bool enable)
{
    if (log_updater == nullptr) return;
    log_updater->mutex.wait();
    if      (logSystem == LOGSYSTEM_YARP)         {log_updater->listen_to_YARP_MESSAGES=enable;}
    else if (logSystem == LOGSYSTEM_YARPRUN)      {log_updater->listen_to_YARPRUN_MESSAGES=enable;}
    log_updater->mutex.post();
}

bool LoggerEngine::get_listen_option               (LogSystemEnum   logSystem)
{
    if (log_updater == nullptr) return false;
    if (logSystem == LOGSYSTEM_YARP)        {return log_updater->listen_to_YARP_MESSAGES;}
    if (logSystem == LOGSYSTEM_YARPRUN)     {return log_updater->listen_to_YARPRUN_MESSAGES;}
    return false;
}

bool LoggerEngine::export_log_to_text_file   (std::string  filename, std::string portname)
{
    if (log_updater == nullptr) return false;
    if (filename.size() == 0) return false;

    log_updater->mutex.wait();
    std::list<LogEntry>::iterator it;
    for (it = log_updater->log_list.begin(); it != log_updater->log_list.end(); it++)
    {
        if (it->logInfo.port_complete == portname)
        {
            ofstream file1;
            file1.open(filename.c_str());
            if (file1.is_open() == false) {log_updater->mutex.post(); return false;}
            for (std::deque<MessageEntry>::const_iterator it1(it->entry_list.begin());
                 it1 != it->entry_list.end(); ++it1)
            {
                file1 << it1->yarprun_timestamp << " " << it1->local_timestamp << " " << it1->level.toString() << " " << it1->text << " " << std::endl;
            }
            file1.close();
        }
    }
    log_updater->mutex.post();
    return true;
}

bool LoggerEngine::save_all_logs_to_file   (std::string  filename)
{
    string start_string ="<#STRING_START#>";
    string end_string ="<#STRING_END#>";

    if (log_updater == nullptr) return false;
    if (filename.size() == 0) return false;

    const int      LOGFILE_VERSION = 1;

    ofstream file1;
    file1.open(filename.c_str());
    if (file1.is_open() == false) return false;

    bool wasRunning = log_updater->isRunning();
    if (wasRunning) log_updater->stop();
    std::list<LogEntry>::iterator it;
    file1 << LOGFILE_VERSION << std::endl;
    file1 << log_updater->log_list.size() << std::endl;
    for (it = log_updater->log_list.begin(); it != log_updater->log_list.end(); it++)
    {
        file1 << it->logInfo.ip_address << std::endl;
        file1 << it->logInfo.port_complete << std::endl;
        file1 << it->logInfo.port_prefix << std::endl;
        file1 << it->logInfo.port_system << std::endl;
        file1 << it->logInfo.process_name << std::endl;
        file1 << it->logInfo.process_pid << std::endl;
        file1 << it->logInfo.get_number_of_traces() << std::endl;
        file1 << it->logInfo.get_number_of_debugs() << std::endl;
        file1 << it->logInfo.get_number_of_infos() << std::endl;
        file1 << it->logInfo.get_number_of_warnings() << std::endl;
        file1 << it->logInfo.get_number_of_errors() << std::endl;
        file1 << it->logInfo.get_number_of_fatals() << std::endl;
        file1 << it->entry_list.size() << std::endl;
        for (std::deque<MessageEntry>::const_iterator it1(it->entry_list.begin());
             it1 != it->entry_list.end(); ++it1)
        {
            file1 << it1->yarprun_timestamp << std::endl;
            file1 << it1->local_timestamp << std::endl;
            file1 << it1->level.toInt() << std::endl;
            file1 << start_string;
            for (unsigned int s=0; s< it1->text.size(); s++) file1.put(it1->text[s]);
            file1 << end_string <<endl;
        }
    }
    file1.close();
    if (wasRunning) log_updater->start();
    return true;
}

std::streamoff get_tag(ifstream& file, const char* tag)
{
    std::streamoff pos=file.tellg();
    int tag_size=strlen(tag);
    char* buff = new char[tag_size+2];
    for(int i=0; i<tag_size+2;i++) buff[i]=0;
    std::streamoff off=0;
    for (; ;off++)
    {
        file.seekg(pos+off);
        file.read(buff,tag_size);
        if (file.good()==false)
        {
            delete [] buff;
            return -1;
        }
        if (strcmp(buff,tag)==0) break;
    }
    delete [] buff;
    return pos+off;
}

bool LoggerEngine::load_all_logs_from_file   (std::string  filename)
{
    string start_string ="<#STRING_START#>";
    int start_string_size=strlen(start_string.c_str());
    string end_string ="<#STRING_END#>";
    int end_string_size=strlen(end_string.c_str());

    if (log_updater == nullptr) return false;
    if (filename.size() == 0) return false;

    const int      LOGFILE_VERSION = 1;

    ifstream file1;
    file1.open(filename.c_str(),std::ifstream::binary);
    if (file1.is_open() == false) return false;

    int log_file_version;
    bool wasRunning = log_updater->isRunning();
    if (wasRunning) log_updater->stop();
    file1 >> log_file_version;
    if (log_file_version == LOGFILE_VERSION)
    {
        int size_log_list;
        file1 >> size_log_list;
        log_updater->log_list.clear();
        for (int i=0; i< size_log_list; i++)
        {
            LogEntry l_tmp;
            int      dummy;
            file1 >> l_tmp.logInfo.ip_address;
            file1 >> l_tmp.logInfo.port_complete;
            file1 >> l_tmp.logInfo.port_prefix;
            file1 >> l_tmp.logInfo.port_system;
            file1 >> l_tmp.logInfo.process_name;
            file1 >> l_tmp.logInfo.process_pid;
            file1 >> dummy; //l_tmp.logInfo.number_of_traces;
            file1 >> dummy; //l_tmp.logInfo.number_of_debugs;
            file1 >> dummy; //l_tmp.logInfo.number_of_infos;
            file1 >> dummy; //l_tmp.logInfo.number_of_warning;
            file1 >> dummy; //l_tmp.logInfo.number_of_errors;
            file1 >> dummy; //l_tmp.logInfo.number_of_fatals;
            int size_entry_list;
            file1 >> size_entry_list;
            for (int j=0; j< size_entry_list; j++)
            {
                MessageEntry m_tmp;
                file1 >> m_tmp.yarprun_timestamp;
                file1 >> m_tmp.local_timestamp;
                int tmp_level;
                file1 >> tmp_level;
                m_tmp.level.setLevel(tmp_level);
                std::streamoff start_p = get_tag(file1, start_string.c_str());
                std::streamoff end_p = get_tag(file1, end_string.c_str());
                //validity check
                if (start_p<0 || end_p<0 || end_p-start_p+start_string_size<=0) return false;
                char *buff = new char[(unsigned int)(end_p-start_p+start_string_size)];
                //memset(buff,0,end_p-start_p+start_string_size);
                file1.seekg(start_p+start_string_size);
                if (end_p-start_p-start_string_size!=1)
                {
                    file1.get(buff,end_p-start_p-start_string_size);
                }
                else
                {
                    //skip empty strings
                }
                file1.seekg(end_p+end_string_size);
                m_tmp.text=buff;
                delete [] buff;
                l_tmp.entry_list.push_back(m_tmp);
            }
            log_updater->log_list.push_back(l_tmp);
        }
    }
    file1.close();
    if (wasRunning) log_updater->start();
    return true;
}

void LoggerEngine::set_log_lines_max_size (bool  enabled,  int new_size)
{
    if (log_updater == nullptr) return;
    log_updater->mutex.wait();

    std::list<LogEntry>::iterator it;
    for (it = log_updater->log_list.begin(); it != log_updater->log_list.end(); it++)
    {
        it->setLogEntryMaxSize(new_size);
        it->setLogEntryMaxSizeEnabled(enabled);
    }

    log_updater->mutex.post();
}


void LoggerEngine::set_log_list_max_size           (bool  enabled,  int new_size)
{
    if (log_updater == nullptr) return;
    log_updater->mutex.wait();
    log_updater->log_list_max_size_enabled = enabled;
    log_updater->log_list_max_size = new_size;
    log_updater->mutex.post();
}

void LoggerEngine::get_log_lines_max_size          (bool& enabled, int& current_size)
{
    if (log_updater == nullptr) return;
    if (log_updater->log_list.empty() == true) return;
    log_updater->mutex.wait();
    std::list<LogEntry>::iterator it = log_updater->log_list.begin();
    current_size = it->getLogEntryMaxSize();
    enabled = it->getLogEntryMaxSizeEnabled();
    log_updater->mutex.post();
}

void LoggerEngine::get_log_list_max_size           (bool& enabled, int& current_size)
{
    if (log_updater == nullptr) return;
    log_updater->mutex.wait();
    enabled=log_updater->log_list_max_size_enabled;
    current_size=log_updater->log_list_max_size;
    log_updater->mutex.post();
}

bool LoggerEngine::clear()
{
    if (log_updater == nullptr) return false;
    log_updater->mutex.wait();
    log_updater->log_list.clear();
    log_updater->mutex.post();
    return true;
}

void LoggerEngine::set_log_enable_by_port_complete (std::string  port, bool enable)
{
    if (log_updater == nullptr) return;

    log_updater->mutex.wait();
    std::list<LogEntry>::iterator it;
    for (it = log_updater->log_list.begin(); it != log_updater->log_list.end(); it++)
    {
        if (it->logInfo.port_complete == port)
           {
               it->logging_enabled=enable;
               break;
           }
    }
    log_updater->mutex.post();
}

bool  LoggerEngine::get_log_enable_by_port_complete (std::string  port)
{
    if (log_updater == nullptr) return false;

    bool enabled=false;
    log_updater->mutex.wait();
    std::list<LogEntry>::iterator it;
    for (it = log_updater->log_list.begin(); it != log_updater->log_list.end(); it++)
    {
        if (it->logInfo.port_complete == port)
           {
               enabled=it->logging_enabled;
               break;
           }
    }
    log_updater->mutex.post();
    return enabled;
}

void yarp::yarpLogger::LogEntry::addObserver(yarp::yarpLogger::LogEntryObserver& observer)
{
    observers.push_back(&observer);
}

void yarp::yarpLogger::LogEntry::removeObserver(yarp::yarpLogger::LogEntryObserver& observer)
{
    auto found = std::find(observers.begin(), observers.end(), &observer);

    if (found != observers.end()) {
        observers.erase(found);
    }
}

LogEntryObserver::~LogEntryObserver() {}
void LogEntryObserver::logEntryWillAddRows(yarp::yarpLogger::LogEntry& entry, const std::pair<size_t, size_t> &addedRows) {}
void LogEntryObserver::logEntryDidAddRows(yarp::yarpLogger::LogEntry& entry, const std::pair<size_t, size_t> &addedRows) {}
void LogEntryObserver::logEntryWillRemoveRows(yarp::yarpLogger::LogEntry& entry, const std::pair<size_t, size_t> &removedRows) {}
void LogEntryObserver::logEntryDidRemoveRows(yarp::yarpLogger::LogEntry& entry, const std::pair<size_t, size_t> &removedRows) {}

