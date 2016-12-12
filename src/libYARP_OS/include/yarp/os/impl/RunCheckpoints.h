/*
 * Copyright (C) 2007-2009 RobotCub Consortium
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 * Author: Alessandro Scalzo <alessandro.scalzo@iit.it>
 */

#ifndef YARP_OS_IMPL_RUNCHECKPOINTS_H
#define YARP_OS_IMPL_RUNCHECKPOINTS_H

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
# define RUNLOG(msg) YarprunCheckpoints::instance().checkpoint(msg,__FILE__,__FUNCTION__,__LINE__);
#else
# define RUNLOG(msg)
#endif

#endif // YARP_OS_IMPL_RUNCHECKPOINTS_H
