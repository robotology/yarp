// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007-2009 RobotCub Consortium
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 * author Alessandro Scalzo alessandro.scalzo@iit.it
 */

#if defined(WIN32)
#include <time.h>
#else
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
    sprintf(path,"C:/Users/user/yarprun_log_%s_%u.txt",ctime(&now),(unsigned)rand());
#else
    timeval now;
    gettimeofday(&now,NULL);
    sprintf(path,"/tmp/yarprun_log_%s_%06d.txt",ctime(&(now.tv_sec)),(int)now.tv_usec);
#endif

    for (int t=0; t<256 && path[t]; ++t)
    {
        if (path[t]==' ' || path[t]==':' || path[t]=='?') path[t]='_';
    }

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

void YarprunCheckpoints::checkpoint(const char *prefix,const char* label,const char* sFile,int line)
{
    if (!mLogFile) return;

    fprintf(mLogFile,"%s: %s section %s line %d\n",prefix,sFile,label,line);
    fflush(mLogFile);
}


