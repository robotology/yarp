/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_RUN_IMPL_RUNCHECKPOINTS_H
#define YARP_RUN_IMPL_RUNCHECKPOINTS_H

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
# define RUNLOG(msg) YarprunCheckpoints::instance().checkpoint(msg, __FILE__, __FUNCTION__, __LINE__);
#else
# define RUNLOG(msg)
#endif

#endif // YARP_RUN_IMPL_RUNCHECKPOINTS_H
