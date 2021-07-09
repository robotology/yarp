/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARPCONTEXTUTILS_H
#define YARPCONTEXTUTILS_H

#include <string>
#include <yarp/os/ResourceFinder.h>
#include <yarp/conf/filesystem.h>
#include <iostream>
#include <vector>

#define PATH_SEPARATOR std::string{yarp::conf::filesystem::preferred_separator}

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
#ifndef YARP_NO_DEPRECATED
int import(yarp::os::Bottle& importArg, folderType fType, bool verbose=false);
int importAll(folderType fType, bool verbose=false);
int remove(yarp::os::Bottle& removeArg, folderType fType, bool verbose=false);
int diff(std::string contextName, folderType fType, bool verbose=false);
int diffList(folderType fType, bool verbose=false);
int merge(yarp::os::Bottle& mergeArg, folderType fType, bool verbose=false);
#else
int import(yarp::os::Bottle& importArg, folderType fType);
int importAll(folderType fType);
int remove(yarp::os::Bottle& removeArg, folderType fType);
int diff(std::string contextName, folderType fType);
int diffList(folderType fType);
int merge(yarp::os::Bottle& mergeArg, folderType fType);
#endif

#endif // YARPCONTEXTUTILS_H
