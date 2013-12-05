// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007-2009 RobotCub Consortium
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 * author Alessandro Scalzo alessandro.scalzo@iit.it
 */

#ifndef __YARPRUN_RUN_CHECKPOINTS__
#define __YARPRUN_RUN_CHECKPOINTS__

#include <string>
#include <list>

class YarprunCheckpoints // singleton
{
private:
    YarprunCheckpoints();

public:
    ~YarprunCheckpoints();

    static YarprunCheckpoints& instance();

    void checkpoint(const char* prefix,const char* label,const char* sFile,int line);

private:
    FILE* mLogFile;
};

#ifdef YARPRUN_LOG

#define RUNLOG(msg) YarprunCheckpoints::instance().checkpoint(msg,__FILE__,__FUNCTION__,__LINE__);

#else

#define RUNLOG(msg)

#endif

#endif
