// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007-2009 RobotCub Consortium
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 * author Alessandro Scalzo alessandro.scalzo@iit.it
 */

#include <yarp/os/impl/RunCheckpoints.h>

#include <ace/ACE.h>

YarprunCheckpoints::YarprunCheckpoints()
{
    char temp[512];
    ACE::get_temp_dir(temp,512);

    char date[256];        
    ACE::timestamp(date,256);
    for (int t=0; t<256 && date[t]; ++t)
    {
        if (date[t]==' ' || date[t]==':') date[t]='_';
    }

    char path[512];
    sprintf(path,"%s/yarprun_log_%s.txt","C:/Users/user/Documents/temp",date);

    mLogFile=fopen(path,"w");
}

YarprunCheckpoints::~YarprunCheckpoints()
{
    if (mLogFile) fclose(mLogFile);
}

YarprunCheckpoints& YarprunCheckpoints::instance()
{
    static YarprunCheckpoints singleton;

    return singleton;
}

void YarprunCheckpoints::push(const char* label,const char* sFile,int line)
{
    mLabels.push_back(std::string(label));

    if (mLogFile)
    {
        fprintf(mLogFile,"%s: ENTER section %s line %d\n",sFile,label,line); 
        fflush(mLogFile);
    }
}

void YarprunCheckpoints::checkpoint(const char* sFile,int line)
{
    if (mLogFile)
    {
        fprintf(mLogFile,"%s:       section %s line %d\n",sFile,mLabels.back().c_str(),line); 
        fflush(mLogFile);
    }    
}

void YarprunCheckpoints::pop(const char* sFile,int line)
{
    if (mLogFile)
    {
        fprintf(mLogFile,"%s: EXIT  section %s line %d\n",sFile,mLabels.back().c_str(),line); 
        fflush(mLogFile);
    }

    mLabels.pop_back(); 
}

