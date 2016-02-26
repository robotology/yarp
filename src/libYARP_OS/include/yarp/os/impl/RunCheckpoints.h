/*
 * Copyright (C) 2007-2009 RobotCub Consortium
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 * author Alessandro Scalzo alessandro.scalzo@iit.it
 */

#ifndef YARPRUN_RUN_CHECKPOINTS
#define YARPRUN_RUN_CHECKPOINTS

#include <string>
#include <list>

class YarprunCheckpoints // singleton
{
private:
    YarprunCheckpoints();

public:
    ~YarprunCheckpoints();

    static YarprunCheckpoints& instance();

    void checkpoint(const char* prefix, const char* sFile, const char* sFunction, int line);

private:
    FILE* mLogFile;
};

#ifdef YARPRUN_LOG

#define RUNLOG(msg) YarprunCheckpoints::instance().checkpoint(msg,__FILE__,__FUNCTION__,__LINE__);

#else

#define RUNLOG(msg)

#endif

#endif
