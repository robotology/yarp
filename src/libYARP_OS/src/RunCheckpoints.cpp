/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/impl/RunCheckpoints.h>
#include <yarp/conf/compiler.h>
#include <yarp/os/impl/PlatformUnistd.h>
#include <yarp/os/impl/PlatformTime.h>

#include <ctime>
#include <cstdio>

#if defined(_WIN32)
#include <windows.h>
#endif


YarprunCheckpoints::YarprunCheckpoints()
{
    char path[256];

#if defined(_WIN32)
    time_t now=time(nullptr);
    srand((unsigned)now);
    sprintf(path, "C:/Users/user/Documents/yarprun_log/yarprun_log_%d_%s_%u.txt", GetCurrentProcessId(), ctime(&now), (unsigned)rand());
#else
    timeval now;
    gettimeofday(&now, nullptr);
    sprintf(path, "/tmp/yarprun_log_%d_%s_%06d.txt", getpid(), ctime(&(now.tv_sec)), (int)now.tv_usec);
#endif

    for (int t=10; t<256 && path[t]; ++t)
    {
        if (path[t]=='\n' || path[t]=='\r' || path[t]==' ' || path[t]==':' || path[t]=='?') path[t]='_';
    }

    mLogFile=fopen(path, "w");

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

void YarprunCheckpoints::checkpoint(const char *prefix, const char* sFile, const char* sFunction, int line)
{
    if (!mLogFile) return;

    fprintf(mLogFile, "%s: file %s function %s line %d\n", prefix, sFile, sFunction, line);
    fflush(mLogFile);
}
