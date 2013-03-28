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

    void push(const char* label,const char* sFile,int line);

    void checkpoint(const char* sFile,int line);

    void pop(const char* sFile,int line);

private:
    FILE* mLogFile;
    std::list<std::string> mLabels;
};

#ifdef YARPRUN_LOG

#define CHECK_ENTER(label) YarprunCheckpoints::instance().push(label,__FILE__,__LINE__);
#define CHECKPOINT() YarprunCheckpoints::instance().checkpoint(__FILE__,__LINE__);
#define CHECK_EXIT() YarprunCheckpoints::instance().pop(__FILE__,__LINE__);

#else

#define CHECK_ENTER(dummy)
#define CHECKPOINT()
#define CHECK_EXIT()

#endif

#endif

