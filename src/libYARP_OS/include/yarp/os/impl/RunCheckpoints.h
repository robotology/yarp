// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007-2009 RobotCub Consortium
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 * author Alessandro Scalzo alessandro.scalzo@iit.it
 */

#ifndef __YARPRUN_RUN_CHECKPOINTS__
#define __YARPRUN_RUN_CHECKPOINTS__

//#define YARPRUN_LOG

#ifdef YARPRUN_LOG

#include <string>
#include <list>

class YarprunCheckpoints // singleton
{
private:
    YarprunCheckpoints()
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
        sprintf(path,"%syarprun_log_%s.txt",temp,date);

        mLogFile=fopen(path,"w");
    }

public:
    ~YarprunCheckpoints()
    {
        if (mLogFile) fclose(mLogFile);
    }

    static YarprunCheckpoints& instance()
    {
        static YarprunCheckpoints singleton;

        return singleton;
    }

    void push(const char* label,const char* sFile,int line)
    {
        mLabels.push_back(std::string(label));

        if (mLogFile)
        {
            fprintf(mLogFile,"%s: ENTER section %s line %d\n",sFile,label,line); 
            fflush(mLogFile);
        }
    }

    void checkpoint(const char* sFile,int line)
    {
        if (mLogFile)
        {
            fprintf(mLogFile,"%s:       section %s line %d\n",sFile,mLabels.back().c_str(),line); 
            fflush(mLogFile);
        }    
    }

    void pop(const char* sFile,int line)
    {
        if (mLogFile)
        {
            fprintf(mLogFile,"%s: EXIT  section %s line %d\n",sFile,mLabels.back().c_str(),line); 
            fflush(mLogFile);
        }

        mLabels.pop_back(); 
    }

private:
    FILE* mLogFile;
    std::list<std::string> mLabels;
};

#define CHECK_ENTER(label) YarprunCheckpoints::instance().push(label,__FILE__,__LINE__);
#define CHECKPOINT() YarprunCheckpoints::instance().checkpoint(__FILE__,__LINE__);
#define CHECK_EXIT() YarprunCheckpoints::instance().pop(__FILE__,__LINE__);

#else

#define CHECK_ENTER(dummy)
#define CHECKPOINT()
#define CHECK_EXIT()

#endif

#endif

