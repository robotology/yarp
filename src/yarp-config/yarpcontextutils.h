/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARPCONTEXTUTILS_H
#define YARPCONTEXTUTILS_H

#include <yarp/os/ConstString.h>
#include <yarp/os/ResourceFinder.h>
#include <iostream>
#include <vector>


#if defined(_WIN32)
    #define PATH_SEPARATOR      "\\"
#else
    #define PATH_SEPARATOR      "/"
#endif

enum folderType{CONTEXTS=0, ROBOTS=1};
//helpers:
bool fileCopy(yarp::os::ConstString srcFileName, yarp::os::ConstString destFileName, bool force=false, bool verbose=true);
bool fileRemove(yarp::os::ConstString fileName);
int recursiveCopy(yarp::os::ConstString srcDirName, yarp::os::ConstString destDirName, bool force=false, bool verbose=true);
int recursiveRemove(yarp::os::ConstString dirName, bool verbose=true);
std::vector<yarp::os::ConstString> listContentDirs(const yarp::os::ConstString &curPath);
std::vector<yarp::os::ConstString> listContentFiles(const yarp::os::ConstString &curPath);
void printContentDirs(const yarp::os::ConstString &curPath);
void printUserFolders(yarp::os::ResourceFinder &rf, folderType ftype);
void printSysadmFolders(yarp::os::ResourceFinder &rf, folderType ftype);
void printInstalledFolders(yarp::os::ResourceFinder &rf, folderType ftype);
void prepareHomeFolder(yarp::os::ResourceFinder &rf, folderType ftype);
bool prepareSubFolders(const yarp::os::ConstString& startDir, const yarp::os::ConstString& fileName);
int recursiveDiff(yarp::os::ConstString srcDirName, yarp::os::ConstString destDirName, std::ostream &output=std::cout);
int fileMerge(yarp::os::ConstString srcFileName, yarp::os::ConstString destFileName, yarp::os::ConstString commonParentName);
int recursiveMerge(yarp::os::ConstString srcDirName, yarp::os::ConstString destDirName, yarp::os::ConstString commonParentName, std::ostream &output=std::cout);

//actual commands:
int import(yarp::os::Bottle& importArg, folderType fType, bool verbose=false);
int importAll(folderType fType, bool verbose=false);
int remove(yarp::os::Bottle& removeArg, folderType fType, bool verbose=false);
int diff(yarp::os::ConstString contextName, folderType fType, bool verbose=false);
int diffList(folderType fType, bool verbose=false);
int merge(yarp::os::Bottle& mergeArg, folderType fType, bool verbose=false);

#endif // YARPCONTEXTUTILS_H
