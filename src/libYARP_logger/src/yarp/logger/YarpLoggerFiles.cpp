/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <iostream>
#include <cstring>
#include <string>
#include <sstream>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <yarp/os/RpcClient.h>
#include <yarp/os/SystemClock.h>
#include <yarp/logger/YarpLogger.h>

using namespace yarp::os;
using namespace yarp::yarpLogger;

bool LoggerEngine::export_log_to_text_file   (std::string  filename, std::string portname)
{
    if (log_updater == nullptr) {
        return false;
    }
    if (filename.size() == 0) {
        return false;
    }

    log_updater->mutex.lock();
    std::list<LogEntry>::iterator it;
    for (it = log_updater->log_list.begin(); it != log_updater->log_list.end(); it++)
    {
        if (it->logInfo.port_complete == portname)
        {
            std::ofstream file1;
            file1.open(filename.c_str());
            if (file1.is_open() == false) {log_updater->mutex.unlock(); return false;}
            std::vector<MessageEntry>::iterator it1;
            for (it1 = it->entry_list.begin(); it1 != it->entry_list.end(); it1++)
            {
                file1 << it1->yarprun_timestamp << " " << it1->local_timestamp << " " << it1->level.toString() << " " << it1->text << '\n';
            }
            file1.close();
        }
    }
    log_updater->mutex.unlock();
    return true;
}

void SaveToFile(std::ofstream& file, unsigned int data)
{
    file << data << '\n';
}

void SaveToFile(std::ofstream& file, std::string data)
{
    if (data.empty())
    {
        file << "NULL-DATA" << '\n';
    }
    else
    {
        file << data << '\n';
    }
}

void LoadFromFile(std::ifstream& file, std::string& data)
{
    file >> data;
    if (data =="NULL-DATA") data="";
}

void LoadFromFile(std::ifstream& file, unsigned int& data)
{
    file >> data;
}

std::string start_string = "<#STRING_START#>";
std::string end_string = "<#STRING_END#>";

bool LoggerEngine::save_all_logs_to_file   (std::string  filename)
{
    if (log_updater == nullptr) {
        return false;
    }
    if (filename.size() == 0) {
        return false;
    }

    const int      LOGFILE_VERSION_1 = 1;

    std::ofstream file1;
    file1.open(filename.c_str());
    if (file1.is_open() == false) {
        return false;
    }

    bool wasRunning = log_updater->isRunning();
    if (wasRunning) {
        log_updater->stop();
    }
    std::list<LogEntry>::iterator it;
    SaveToFile(file1, LOGFILE_VERSION_1);
    SaveToFile(file1, log_updater->log_list.size());
    for (it = log_updater->log_list.begin(); it != log_updater->log_list.end(); it++)
    {
        SaveToFile(file1, it->logInfo.ip_address);
        SaveToFile(file1, it->logInfo.port_complete);
        SaveToFile(file1, it->logInfo.port_prefix);
        SaveToFile(file1, it->logInfo.port_system);
        SaveToFile(file1, it->logInfo.process_name);
        SaveToFile(file1, it->logInfo.process_pid);
        SaveToFile(file1, it->logInfo.get_number_of_traces());
        SaveToFile(file1, it->logInfo.get_number_of_debugs());
        SaveToFile(file1, it->logInfo.get_number_of_infos());
        SaveToFile(file1, it->logInfo.get_number_of_warnings());
        SaveToFile(file1, it->logInfo.get_number_of_errors());
        SaveToFile(file1, it->logInfo.get_number_of_fatals());
        SaveToFile(file1, it->logInfo.logsize);
        SaveToFile(file1, it->entry_list.size());

        std::vector<MessageEntry>::iterator it1;
        for (it1 = it->entry_list.begin(); it1 != it->entry_list.end(); it1++)
        {
            SaveToFile(file1, it1->yarprun_timestamp);
            SaveToFile(file1, it1->local_timestamp);;
            SaveToFile(file1, it1->level.toInt());
            file1 << start_string;
            for (char s : it1->text)
            {
                file1.put(s);
            }
            file1 << end_string << '\n';
        }
    }
    file1.close();
    if (wasRunning) {
        log_updater->start();
    }
    return true;
}

std::streamoff get_tag(std::ifstream& file, const char* tag)
{
    std::streamoff pos=file.tellg();
    int tag_size=strlen(tag);
    char* buff = new char[tag_size+2];
    for (int i = 0; i < tag_size + 2; i++) {
        buff[i] = 0;
    }
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
        if (strcmp(buff, tag) == 0) {
            break;
        }
    }
    delete [] buff;
    return pos+off;
}

bool LoggerEngine::load_all_logs_from_file   (std::string  filename)
{
    int start_string_size=strlen(start_string.c_str());
    int end_string_size=strlen(end_string.c_str());

    if (log_updater == nullptr) {
        return false;
    }
    if (filename.size() == 0) {
        return false;
    }

    const int      LOGFILE_VERSION_1 = 1;

    std::ifstream file1;
    file1.open(filename.c_str(),std::ifstream::binary);
    if (file1.is_open() == false) {
        return false;
    }

    int log_file_version;
    bool wasRunning = log_updater->isRunning();
    if (wasRunning) {
        log_updater->stop();
    }
    file1 >> log_file_version;
    if (log_file_version == LOGFILE_VERSION_1)
    {
        unsigned int size_log_list;
        LoadFromFile(file1, size_log_list);

        log_updater->log_list.clear();
        for (int i=0; i< size_log_list; i++)
        {
            LogEntry l_tmp(true, 10000);
            unsigned int      dummy;
            LoadFromFile(file1, l_tmp.logInfo.ip_address);
            LoadFromFile(file1, l_tmp.logInfo.port_complete);
            LoadFromFile(file1, l_tmp.logInfo.port_prefix);
            LoadFromFile(file1, l_tmp.logInfo.port_system);
            LoadFromFile(file1, l_tmp.logInfo.process_name);
            LoadFromFile(file1, l_tmp.logInfo.process_pid);
            LoadFromFile(file1, dummy); //l_tmp.logInfo.number_of_traces;
            LoadFromFile(file1, dummy); //l_tmp.logInfo.number_of_debugs;
            LoadFromFile(file1, dummy); //l_tmp.logInfo.number_of_infos;
            LoadFromFile(file1, dummy); //l_tmp.logInfo.number_of_warning;
            LoadFromFile(file1, dummy); //l_tmp.logInfo.number_of_errors;
            LoadFromFile(file1, dummy); //l_tmp.logInfo.number_of_fatals;
            LoadFromFile(file1, l_tmp.logInfo.logsize);
            unsigned int size_entry_list;
            LoadFromFile(file1, size_entry_list);
            for (size_t j=0; j< size_entry_list; j++)
            {
                MessageEntry m_tmp;
                LoadFromFile(file1, m_tmp.yarprun_timestamp);
                LoadFromFile(file1, m_tmp.local_timestamp);
                unsigned int tmp_level;
                LoadFromFile(file1, tmp_level);
                m_tmp.level.setLevel(tmp_level);

                std::streamoff start_p = get_tag(file1, start_string.c_str());
                std::streamoff end_p = get_tag(file1, end_string.c_str());
                //validity check
                if (start_p < 0 || end_p < 0 || end_p - start_p + start_string_size <= 0) {
                    return false;
                }
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
    if (wasRunning) {
        log_updater->start();
    }
    return true;
}
