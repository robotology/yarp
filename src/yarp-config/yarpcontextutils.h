/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef YARPCONTEXTUTILS_H
#define YARPCONTEXTUTILS_H

#include <string>
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
bool fileCopy(std::string srcFileName, std::string destFileName, bool force=false, bool verbose=true);
bool fileRemove(std::string fileName);
int recursiveCopy(std::string srcDirName, std::string destDirName, bool force=false, bool verbose=true);
int recursiveRemove(std::string dirName, bool verbose=true);
std::vector<std::string> listContentDirs(const std::string &curPath);
std::vector<std::string> listContentFiles(const std::string &curPath);
void printContentDirs(const std::string &curPath);
void printUserFolders(yarp::os::ResourceFinder &rf, folderType ftype);
void printSysadmFolders(yarp::os::ResourceFinder &rf, folderType ftype);
void printInstalledFolders(yarp::os::ResourceFinder &rf, folderType ftype);
void prepareHomeFolder(yarp::os::ResourceFinder &rf, folderType ftype);
bool prepareSubFolders(const std::string& startDir, const std::string& fileName);
int recursiveDiff(std::string srcDirName, std::string destDirName, std::ostream &output=std::cout);
int fileMerge(std::string srcFileName, std::string destFileName, std::string commonParentName);
int recursiveMerge(std::string srcDirName, std::string destDirName, std::string commonParentName, std::ostream &output=std::cout);

//actual commands:
int import(yarp::os::Bottle& importArg, folderType fType, bool verbose=false);
int importAll(folderType fType, bool verbose=false);
int remove(yarp::os::Bottle& removeArg, folderType fType, bool verbose=false);
int diff(std::string contextName, folderType fType, bool verbose=false);
int diffList(folderType fType, bool verbose=false);
int merge(yarp::os::Bottle& mergeArg, folderType fType, bool verbose=false);

#endif // YARPCONTEXTUTILS_H
