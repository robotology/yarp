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

#define _BEGIN         YarprunCheckpoints::instance().checkpoint("BEGIN",__FUNCTION__,__FILE__,__LINE__);
#define _CHECKPOINT    YarprunCheckpoints::instance().checkpoint("     ",__FUNCTION__,__FILE__,__LINE__);
#define _RETURN(val) { YarprunCheckpoints::instance().checkpoint("END  ",__FUNCTION__,__FILE__,__LINE__); return val; }
#define _RETURN_VOID { YarprunCheckpoints::instance().checkpoint("END  ",__FUNCTION__,__FILE__,__LINE__); return; }
#define _EXIT(val)   { YarprunCheckpoints::instance().checkpoint("END  ",__FUNCTION__,__FILE__,__LINE__); exit(val); }

#else

#define _BEGIN
#define _CHECKPOINT
#define _RETURN(val) return val;
#define _RETURN_VOID return;
#define _EXIT(val)   exit(val);

#endif

#endif

