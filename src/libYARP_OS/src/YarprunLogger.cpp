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

#include <iostream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <fstream>
#include <yarp/os/RpcClient.h>
#include <yarp/os/YarprunLogger.h>

using namespace yarp::os;
using namespace yarp::os::YarprunLogger;

void LogEntry::clear()
{
    entry_list.clear();
}

void LogEntry::append(MessageEntry entry)
{
    entry_list.push_back(entry);
}

void LoggerEngine::discover  (std::list<std::string>& ports)
{
    RpcClient p;
    p.open("/logger/discover");
    std::string yarpservername = yarp::os::Network::getNameServerName();
    yarp::os::Network::connect("/logger/discover",yarpservername.c_str());
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
                char* off = 0;
                off = strstr((char*)(n2->get(1).toString().c_str()), "/log/");
                if (off > 0)
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
        entry.logInfo.port_complete.erase(0,1);
        entry.logInfo.port_complete.erase(entry.logInfo.port_complete.size()-1);
        std::istringstream iss(*ports_it);
        std::string token;
        getline(iss, token, '/');
        getline(iss, token, '/');
        getline(iss, token, '/'); entry.logInfo.port_prefix  = token;
        getline(iss, token, '/'); entry.logInfo.process_name = token;
        getline(iss, token, '/'); entry.logInfo.process_pid = token.erase(token.size()-1);
       
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
    }
}

std::string LoggerEngine::logger_thread::getPortName()
{
    return portName;
}

void LoggerEngine::logger_thread::run()
{
    //if (is_discovering()==true)
    {
        //yarp::os::Network::
    }

    yarp::os::Time::delay(0.1);
    std::time_t machine_current_time = std::time(NULL);
    if (logger_port.getInputCount()>0)
    {
        Bottle b;
        logger_port.read(b);
        this->mutex.wait();

        if (b.size()!=2) 
        {
            fprintf (stderr, "unknown log format!\n");
            this->mutex.post();
            return;
        }

        std::string header = b.get(0).asString();
        MessageEntry body;
        std::string s = b.get(1).asString();

        body.text = s;
        body.timestamp = "";
        body.level = 0;

        int str = s.find('[',0);
        int end = s.find(']',0);
        if (str==std::string::npos || end==std::string::npos )
        {
            body.level = 0;
        }
        else if (str==0)
        {
            std::string level = s.substr(str,end);
            body.level = 0;
            if      (level == "[INFO]")      body.level = 0;
            else if (level == "[DEBUG]")     body.level = 0;
            else if (level == "[WARNING]")   body.level = 0;
            else if (level == "[ERROR]")     body.level = 0;
            else if (level == "[CRITICAL]")  body.level = 0;
            body.text = s.substr(end);
        }
        else 
        {
            body.level = 0;
        }

        LogEntry entry;
        entry.logInfo.port_complete = header;
        entry.logInfo.port_complete.erase(0,1);
        entry.logInfo.port_complete.erase(entry.logInfo.port_complete.size()-1);
        std::istringstream iss(header);
        std::string token;
        getline(iss, token, '/');
        getline(iss, token, '/');
        getline(iss, token, '/'); entry.logInfo.port_prefix  = token;
        getline(iss, token, '/'); entry.logInfo.process_name = token;
        getline(iss, token, '/'); entry.logInfo.process_pid = token.erase(token.size()-1);
        
        std::list<LogEntry>::iterator it;
        for (it = log_list.begin(); it != log_list.end(); it++)
        {
            if (it->logInfo.process_pid==entry.logInfo.process_pid)
            {
                it->logInfo.last_update=machine_current_time;
                it->append(body);
                this->mutex.post();
                return;
            }
        }
        if (it == log_list.end())
        {
            entry.append(body);
            entry.logInfo.last_update=machine_current_time;
            log_list.push_back(entry);
        }
        
        this->mutex.post();
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

    if (log_updater->logger_port.open(log_updater->getPortName().c_str())==true)
    {
        fprintf(stdout,"Logger successfully started, listening on port %s\n", log_updater->getPortName().c_str());
    }
    else
    {
        fprintf(stderr,"Unable to start logger: port %s is unavailable\n", log_updater->getPortName().c_str());
        return false;
    }

    logging=true;
    log_updater->start();
    return true;
}

void LoggerEngine::logger_thread::threadRelease()
{
    logger_port.interrupt();
    logger_port.close();
}

void LoggerEngine::stop_logging()
{
    logging=false;
    if (log_updater->isRunning()==true) log_updater->stop();
}

void LoggerEngine::start_discover()
{
    log_updater->start();
    discovering=true;
}

void LoggerEngine::stop_discover()
{
    discovering=false;
}

LoggerEngine::LoggerEngine(std::string portName)
{
    log_updater=new logger_thread (10, portName);
    logging = false;
    discovering = false;
}

LoggerEngine::~LoggerEngine()
{
    this->stop_logging();
    if (log_updater!=0)
    {
        delete log_updater;
        log_updater = 0;
    }
}

int  LoggerEngine::get_num_of_processes()
{
    return log_updater->logger_port.getInputCount();
}

void LoggerEngine::get_infos (std::list<LogEntryInfo>& infos)
{
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
    log_updater->mutex.wait();
    std::list<LogEntry>::iterator it;
    for (it = log_updater->log_list.begin(); it != log_updater->log_list.end(); it++)
    {
        messages.insert(messages.end(), it->entry_list.begin(), it->entry_list.end());
    }
    log_updater->mutex.post();
}

void LoggerEngine::get_messages_by_port_prefix    (std::string  port,  std::list<MessageEntry>& messages)
{
    log_updater->mutex.wait();
    std::list<LogEntry>::iterator it;
    for (it = log_updater->log_list.begin(); it != log_updater->log_list.end(); it++)
    {
        if (it->logInfo.port_prefix == port)
        {
            messages = (it->entry_list);
            break;
        }
    }
    log_updater->mutex.post();
}

void LoggerEngine::get_messages_by_port_complete    (std::string  port,  std::list<MessageEntry>& messages)
{
    log_updater->mutex.wait();
    std::list<LogEntry>::iterator it;
    for (it = log_updater->log_list.begin(); it != log_updater->log_list.end(); it++)
    {
        if (it->logInfo.port_complete == port)
        {
            messages = (it->entry_list);
            break;
        }
    }
    log_updater->mutex.post();
}

void LoggerEngine::get_messages_by_process (std::string  process,  std::list<MessageEntry>& messages)
{
    log_updater->mutex.wait();
    std::list<LogEntry>::iterator it;
    for (it = log_updater->log_list.begin(); it != log_updater->log_list.end(); it++)
    {
        if (it->logInfo.process_name == process)
        {
            messages = (it->entry_list);
            break;
        }
    }
    log_updater->mutex.post();
}

void LoggerEngine::get_messages_by_pid     (std::string pid, std::list<MessageEntry>& messages)
{
    log_updater->mutex.wait();
    std::list<LogEntry>::iterator it;
    for (it = log_updater->log_list.begin(); it != log_updater->log_list.end(); it++)
    {
        if (it->logInfo.process_pid == pid)
        {
            messages = (it->entry_list);
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
        if (it->level==level)
            ret.push_back(*it);
    }
    return ret;
}

void LoggerEngine::save_to_file            (std::string  filename)
{
    const int      LOGFILE_VERSION = 1;

    std::fstream file1;
    file1.open(filename, std::fstream::out);
    bool wasRunning = log_updater->isRunning();
    if (wasRunning) log_updater->stop();
    std::list<LogEntry>::iterator it;
    file1 << LOGFILE_VERSION << std::endl;
    file1 << log_updater->log_list.size() << std::endl;
    for (it = log_updater->log_list.begin(); it != log_updater->log_list.end(); it++)
    {
        file1 << it->logInfo.port_complete << std::endl;
        file1 << it->logInfo.port_prefix << std::endl;
        file1 << it->logInfo.process_name << std::endl;
        file1 << it->logInfo.process_pid << std::endl;
        file1 << it->entry_list.size() << std::endl;
        std::list<MessageEntry>::iterator it1;
        for (it1 = it->entry_list.begin(); it1 != it->entry_list.end(); it1++)
        {
            file1 << it1->timestamp << std::endl;
            file1 << it1->level << std::endl;
            file1 << it1->text << std::endl;
        }
    }
    file1.close();
    if (wasRunning) log_updater->start();
}

void LoggerEngine::load_from_file          (std::string  filename)
{
    const int      LOGFILE_VERSION = 1;

    std::fstream file1;
    file1.open(filename, std::fstream::out);
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
            file1 >> l_tmp.logInfo.port_complete;
            file1 >> l_tmp.logInfo.port_prefix;
            file1 >> l_tmp.logInfo.process_name;
            file1 >> l_tmp.logInfo.process_pid;
            int size_entry_list;
            file1 >> size_entry_list;
            for (int j=0; j< size_entry_list; j++)
            {
                MessageEntry m_tmp;
                file1 >> m_tmp.timestamp;
                file1 >> m_tmp.level;
                file1 >> m_tmp.text;
                l_tmp.entry_list.push_back(m_tmp);
            }
            log_updater->log_list.push_back(l_tmp);
        }
    }
    file1.close();
    if (wasRunning) log_updater->start();
}