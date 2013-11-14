// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007-2009 RobotCub Consortium
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 * author Alessandro Scalzo alessandro.scalzo@iit.it
 */

#if defined(WIN32)
#include <time.h>
#include <windows.h>
#else
#include <unistd.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <yarp/os/impl/RunCheckpoints.h>

YarprunCheckpoints::YarprunCheckpoints()
{
    char path[256];

#if defined(WIN32)
    time_t now=time(NULL);
    srand((unsigned)now);
    sprintf(path,"C:/Users/user/Documents/yarprun_log/yarprun_log_%d_%s_%u.txt",GetCurrentProcessId(),ctime(&now),(unsigned)rand());
#else
    timeval now;
    gettimeofday(&now,NULL);
    sprintf(path,"/tmp/yarprun_log_%d_%s_%06d.txt",getpid(),ctime(&(now.tv_sec)),(int)now.tv_usec);
#endif

    for (int t=10; t<256 && path[t]; ++t)
    {
        if (path[t]=='\n' || path[t]=='\r' || path[t]==' ' || path[t]==':' || path[t]=='?') path[t]='_';
    }

    mLogFile=fopen(path,"w");

    if (!mLogFile) perror(path);
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

void YarprunCheckpoints::checkpoint(const char *prefix,const char* sFile,const char* sFunction,int line)
{
    if (!mLogFile) return;

    fprintf(mLogFile,"%s: file %s function %s line %d\n",prefix,sFile,sFunction,line);
    fflush(mLogFile);
}


