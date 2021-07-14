/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "yarpcontextutils.h"

#include <yarp/conf/dirs.h>
#include <yarp/os/Os.h>
#include <yarp/os/Log.h>
#include <yarp/os/impl/PlatformDirent.h>
#include <yarp/os/impl/PlatformSysStat.h>
#include <yarp/os/impl/PlatformUnistd.h>

#include <set>
#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cerrno>


#if defined(_WIN32)
#pragma warning (disable : 4018)
#pragma warning (disable : 4267)

#ifdef DELETE
#undef DELETE
#endif

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

#endif

#include <diff_match_patch.h>

#if defined(_WIN32)
#pragma warning(default:4018)
#pragma warning(default:4267)
#endif


using namespace yarp::os;
using namespace std;

std::string getFolderStringName(folderType ftype)
{
    switch (ftype)
    {
    case 0:
        return std::string("contexts");
    case 1:
        return std::string("robots");
    default:
        return {};
    }
}

std::string getFolderStringNameHidden(folderType ftype)
{
    switch (ftype)
    {
    case 0:
        return std::string(".contexts");
    case 1:
        return std::string(".robots");
    default:
        return {};
    }
}

bool isHidden(std::string fileName)
{
#if defined(_WIN32)
    DWORD attributes = GetFileAttributes(fileName.c_str());
    if (attributes & FILE_ATTRIBUTE_HIDDEN)
        return true;
#else
    if (fileName[0] == '.') {
        return true;
    }
#endif
    return false;
}

bool fileCopy(std::string srcFileName, std::string destFileName, bool force, bool verbose)
{
    char buf[BUFSIZ];
    size_t size;
    FILE* source = fopen(srcFileName.c_str(), "rb");
    if (source == nullptr)
    {
        if (verbose) {
            printf("Could not open source file %s\n", srcFileName.c_str());
        }
        return false;
    }
    if (!yarp::os::stat(destFileName.c_str()))
    {
        if (force)
        {
            fileRemove(destFileName);
        }
        else
        {
            if (verbose) {
                printf("File already exists : %s\n", destFileName.c_str());
            }
            fclose(source);
            return false;
        }
    }
    FILE* dest = fopen(destFileName.c_str(), "wb");
    if (dest == nullptr)
    {
        if (verbose) {
            printf("Could not open target file %s\n", destFileName.c_str());
        }
        fclose(source);
        return false;
    }
    // clean and more secure
    // feof(FILE* stream) returns non-zero if the end of file indicator for stream is set

    while ((size = fread(buf, 1, BUFSIZ, source)))
    {
        fwrite(buf, 1, size, dest);
    }

    fclose(source);
    fclose(dest);
    return true;
}

bool fileRemove(std::string fileName)
{
    return !yarp::os::impl::unlink(fileName.c_str());
}

int recursiveCopy(std::string srcDirName, std::string destDirName, bool force, bool verbose)
{
    bool ok = true;

    yarp::os::impl::YARP_stat statbuf;
    if (yarp::os::impl::stat(srcDirName.c_str(), &statbuf) == -1)
    {
        if (verbose) {
            printf("Error in checking properties for %s\n", srcDirName.c_str());
        }
        return -1;
    }
    if ((statbuf.st_mode & S_IFMT) == S_IFREG) {
        ok = fileCopy(srcDirName, destDirName, force, verbose) && ok;
    } else if ((statbuf.st_mode & S_IFMT) == S_IFDIR) {
        yarp::os::impl::dirent **namelist;
        int n = yarp::os::impl::scandir(srcDirName.c_str(), &namelist, nullptr, yarp::os::impl::alphasort);
        if (n<0)
        {
            if (verbose) {
                std::cerr << "Could not read from directory " << srcDirName << std::endl;
            }
            return -1;
        }
        if (yarp::os::mkdir((destDirName).c_str()) < 0)
        {
            if (errno == EEXIST && force)
            {
                bool ok = !recursiveRemove(destDirName, verbose);
                ok = ok && !yarp::os::mkdir((destDirName).c_str());
                if (!ok)
                {
                    if (verbose) {
                        printf("Could not create directory %s\n", destDirName.c_str());
                    }
                    return -1;
                }
            }
            else
            {
                if (verbose) {
                    if (errno == EEXIST)
                    {
                        printf("Directory %s already exist; remove it first, or use the diff/merge commands\n", destDirName.c_str());
                    }
                    else
                    {
                        printf("Could not create directory %s\n", destDirName.c_str());
                    }
                }
                return -1;
            }
        }
        for (int i = 0; i<n; i++)
        {

            std::string name = namelist[i]->d_name;

            if (name != "." && name != "..")
            {
                std::string srcPath = std::string(srcDirName).append(PATH_SEPARATOR).append(name);
                std::string destPath = std::string(destDirName).append(PATH_SEPARATOR).append(name);
                recursiveCopy(srcPath, destPath, force, verbose);
            }
            free(namelist[i]);
        }
        free(namelist);
    }
    return (ok == true ? 0 : -1);
}

int recursiveRemove(std::string dirName, bool verbose)
{
    yarp::os::impl::YARP_stat statbuf;
    if (yarp::os::impl::stat(dirName.c_str(), &statbuf) == -1)
    {
        if (verbose) {
            printf("Error in checking properties for %s\n", dirName.c_str());
        }
        return -1;
    }
    if ((statbuf.st_mode & S_IFMT) == S_IFREG) {
        return fileRemove(dirName) ? 0 : -1;
    } else if ((statbuf.st_mode & S_IFMT) == S_IFDIR) {
        yarp::os::impl::dirent **namelist;
        int n = yarp::os::impl::scandir(dirName.c_str(), &namelist, nullptr, yarp::os::impl::alphasort);
        if (n<0)
        {
            if (verbose) {
                printf("Could not read from  directory %s\n", dirName.c_str());
            }
            return yarp::os::rmdir(dirName.c_str()); // TODO check if this is useful...
        }

        for (int i = 0; i<n; i++)
        {
            std::string name = namelist[i]->d_name;
            std::string path = std::string(dirName).append(PATH_SEPARATOR).append(name);
            if (name != "." && name != "..")
            {
                recursiveRemove(path, verbose);
            }
            free(namelist[i]);
        }
        free(namelist);

        return yarp::os::rmdir(dirName.c_str());
    } else {
        return -1;
    }
}

std::vector<std::string> listContentDirs(const std::string &curPath)
{
    std::vector<std::string> dirsList;
    yarp::os::impl::dirent **namelist;
    int n = yarp::os::impl::scandir(curPath.c_str(), &namelist, nullptr, yarp::os::impl::alphasort);
    if (n<0) {
        return dirsList;
    }
    for (int i = 0; i<n; i++) {

        std::string name = namelist[i]->d_name;
        if (name != "." && name != "..")
        {
            yarp::os::impl::YARP_stat statbuf;
            std::string path = std::string(curPath).append(PATH_SEPARATOR).append(name);
            if (yarp::os::impl::stat(path.c_str(), &statbuf) == -1) {
                printf("Error in checking properties for %s\n", path.c_str());
            }

            if ((statbuf.st_mode & S_IFMT) == S_IFDIR) {
                dirsList.push_back(name);
            }
        }
        free(namelist[i]);
    }
    free(namelist);
    return dirsList;
}

std::vector<std::string> listContentFiles(const std::string &curPath)
{
    std::vector<std::string> fileList;
    yarp::os::impl::dirent **namelist;
    int n = yarp::os::impl::scandir(curPath.c_str(), &namelist, nullptr, yarp::os::impl::alphasort);
    if (n<0) {
        return fileList;
    }
    std::list<std::string> fileStack;
    for (int i = 0; i<n; i++)
    {
        fileStack.emplace_back(namelist[i]->d_name);
        free(namelist[i]);
    }
    free(namelist);

    while (!fileStack.empty())
    {
        std::string name = fileStack.front();

        if (name != "." && name != "..")
        {
            yarp::os::impl::YARP_stat statbuf;
            std::string path = std::string(curPath).append(PATH_SEPARATOR).append(name);
            if (yarp::os::impl::stat(path.c_str(), &statbuf) == -1) {
                printf("Error in checking properties for %s\n", path.c_str());
            }
            if ((statbuf.st_mode & S_IFMT) == S_IFREG)
            {
                fileList.push_back(path);
            }

            if ((statbuf.st_mode & S_IFMT) == S_IFDIR)
            {
                std::vector<std::string> nestedFiles = listContentFiles(path);
                for (auto& nestedFile : nestedFiles) {
                    fileStack.push_back(std::string {path}.append(PATH_SEPARATOR).append(nestedFile));
                }
            }
        }
        fileStack.pop_front();
    }
    return fileList;
}

void printContentDirs(const std::string &curPath)
{
    std::vector<std::string> dirsList = listContentDirs(curPath);
    for (auto& dirsIt : dirsList) {
        printf("%s\n", dirsIt.c_str());
    }
}


void printUserFolders(yarp::os::ResourceFinder &rf, folderType ftype)
{
    ResourceFinderOptions opts;
    opts.searchLocations = ResourceFinderOptions::User;
    yarp::os::Bottle contextPaths = rf.findPaths(getFolderStringName(ftype), opts);
    printf("**LOCAL USER DATA:\n");
    for (size_t curPathId = 0; curPathId<contextPaths.size(); ++curPathId)
    {
        printf("* Directory : %s\n", contextPaths.get(curPathId).asString().c_str());
        printContentDirs(contextPaths.get(curPathId).asString());
    }
}

void printSysadmFolders(yarp::os::ResourceFinder &rf, folderType ftype)
{
    ResourceFinderOptions opts;
    opts.searchLocations = ResourceFinderOptions::Sysadmin;
    yarp::os::Bottle contextPaths = rf.findPaths(getFolderStringName(ftype), opts);
    printf("**SYSADMIN DATA:\n");
    for (size_t curPathId = 0; curPathId<contextPaths.size(); ++curPathId)
    {
        printf("* Directory : %s\n", contextPaths.get(curPathId).asString().c_str());
        printContentDirs(contextPaths.get(curPathId).asString());
    }
}

void printInstalledFolders(yarp::os::ResourceFinder &rf, folderType ftype)
{
    ResourceFinderOptions opts;
    opts.searchLocations = ResourceFinderOptions::Installed;
    yarp::os::Bottle contextPaths = rf.findPaths(getFolderStringName(ftype), opts);
    printf("**INSTALLED DATA:\n");
    for (size_t curPathId = 0; curPathId<contextPaths.size(); ++curPathId)
    {
        printf("* Directory : %s\n", contextPaths.get(curPathId).asString().c_str());
        printContentDirs(contextPaths.get(curPathId).asString());
    }
}

void prepareHomeFolder(yarp::os::ResourceFinder &rf, folderType ftype)
{
    std::string yarpdatahome = yarp::conf::dirs::yarpdatahome();
    yarp::os::impl::DIR* dir = yarp::os::impl::opendir((yarpdatahome).c_str());
    if (dir != nullptr) {
        yarp::os::impl::closedir(dir);
    } else {
        yarp::os::mkdir((yarpdatahome).c_str());
    }

    dir = yarp::os::impl::opendir((yarpdatahome.append(PATH_SEPARATOR).append(getFolderStringName(ftype))).c_str());
    if (dir != nullptr) {
        yarp::os::impl::closedir(dir);
    } else {
        yarp::os::mkdir((yarpdatahome.append(PATH_SEPARATOR).append(getFolderStringName(ftype))).c_str());
    }
    dir = yarp::os::impl::opendir((yarpdatahome.append(PATH_SEPARATOR).append(getFolderStringNameHidden(ftype))).c_str());
    if (dir != nullptr) {
        yarp::os::impl::closedir(dir);
    } else {
        std::string hiddenPath = (yarpdatahome.append(PATH_SEPARATOR).append(getFolderStringNameHidden(ftype)));
        yarp::os::mkdir(hiddenPath.c_str());
#if defined(_WIN32)
        SetFileAttributes(hiddenPath.c_str(), FILE_ATTRIBUTE_HIDDEN);
#endif
    }
}

bool prepareSubFolders(const std::string& startDir, const std::string& fileName)
{
    string fname(fileName);
    if (fname.find('/') == string::npos) {
        return true;
    } else {
        size_t curPos, startPos = 0;
        while ((curPos = fname.find('/', startPos)) != string::npos)
        {
            yarp::os::mkdir((startDir + PATH_SEPARATOR + fname.substr(0, curPos)).c_str());
            startPos = curPos + string(PATH_SEPARATOR).length();
        }
    }
    return true;
}

bool recursiveFileList(const char* basePath, const char* suffix, std::set<std::string>& filenames)
{
    string strPath = string(basePath);
    if ((strPath.rfind('/') == string::npos) || (strPath.rfind('/') != strPath.size() - 1)) {
        strPath = strPath + string(PATH_SEPARATOR);
    }

    string mySuffix = string(suffix);

    if (((mySuffix.rfind('/') == string::npos) || (mySuffix.rfind('/') != mySuffix.size() - 1)) && mySuffix != "") {
        mySuffix = mySuffix + string(PATH_SEPARATOR);
    }

    strPath += mySuffix;

    yarp::os::impl::dirent **namelist;
    int n = yarp::os::impl::scandir(strPath.c_str(), &namelist, nullptr, yarp::os::impl::alphasort);
    if (n<0)
    {
        std::cerr << "Could not read from  directory " << strPath << std::endl;
        return false;
    }
    bool ok = true;
    for (int i = 0; i<n; i++)
    {

        std::string name = namelist[i]->d_name;
        yarp::os::impl::YARP_stat statbuf;
        if (name != "." && name != "..")
        {
            yarp::os::impl::stat(std::string(strPath).append(PATH_SEPARATOR).append(name).c_str(), &statbuf);
            if ((statbuf.st_mode & S_IFMT) == S_IFREG)
            {
                filenames.insert(mySuffix + name);
            }
            else
            if ((statbuf.st_mode & S_IFMT) == S_IFDIR)
            {
                ok = ok && recursiveFileList(basePath, (mySuffix + name).c_str(), filenames);
            }
        }
        free(namelist[i]);
    }
    free(namelist);
    return ok;
}


int recursiveDiff(std::string srcDirName, std::string destDirName, std::ostream &output)
{
    std::set<std::string> srcFileList;
    bool ok = recursiveFileList(srcDirName.c_str(), "", srcFileList);
    std::set<std::string> destFileList;
    ok = ok && recursiveFileList(destDirName.c_str(), "", destFileList);

    if (!ok) {
        return -1;
    }
    size_t nModifiedFiles = 0;
    for (const auto& srcIt : srcFileList)
    {
        auto destPos = destFileList.find(srcIt);
        if (destPos != destFileList.end())
        {
            diff_match_patch<std::string> dmp;
            std::string srcFileName = std::string{srcDirName}.append(PATH_SEPARATOR).append(srcIt);
            if (isHidden(srcFileName)) {
                continue;
            }

            std::ifstream in(srcFileName.c_str());
            std::string srcStr((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
            in.close();
            in.open((destDirName + PATH_SEPARATOR + (*destPos)).c_str());
            std::string destStr((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
            std::string patchString = dmp.patch_toText(dmp.patch_make(srcStr, destStr));
            if (patchString != "")
            {
                output << "- " << srcDirName << PATH_SEPARATOR << srcIt << endl;
                output << "+ " << destDirName << PATH_SEPARATOR << (*destPos) << endl;
                output << dmp.patch_toText(dmp.patch_make(srcStr, destStr)) << std::endl;
                nModifiedFiles++;
            }
            destFileList.erase(destPos);
        }
        //         else
        //         {
        //             output << "Added file  " << srcDirName+PATH_SEPARATOR +(*srcIt) <<endl;
        //             nModifiedFiles++;
        //         }
    }

    //     for(std::set<std::string>::iterator destIt=destFileList.begin(); destIt !=destFileList.end(); ++destIt)
    //     {
    //         std::string destFileName=destDirName+ PATH_SEPARATOR + (*destIt);
    //         if(isHidden(destFileName))
    //             continue;
    //
    //         output << "Removed file " << destFileName <<endl;
    //         nModifiedFiles++;
    //
    //     }

    return nModifiedFiles;//tbm
}

int fileMerge(std::string srcFileName, std::string destFileName, std::string commonParentName)
{
    diff_match_patch<std::string> dmp;
    std::ifstream in(srcFileName.c_str());
    std::string srcStr((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    in.close();
    in.open(destFileName.c_str());
    std::string destStr((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    in.close();
    in.open(commonParentName.c_str());
    std::string hiddenDestStr((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    in.close();
    diff_match_patch<string>::Patches patches = dmp.patch_make(hiddenDestStr, destStr);
    std::pair<std::string, std::vector<bool> > mergeResults = dmp.patch_apply(patches, srcStr);
    std::ofstream out(destFileName.c_str());
    out << mergeResults.first;
    out.close();
    //update hidden file from installed one
    out.open(commonParentName.c_str());
    out << srcStr;
    out.close();
    return 0;
}

int recursiveMerge(std::string srcDirName, std::string destDirName, std::string commonParentName, std::ostream &output)
{
    std::set<std::string> srcFileList;
    bool ok = recursiveFileList(srcDirName.c_str(), "", srcFileList);
    std::set<std::string> destFileList;
    ok = ok && recursiveFileList(destDirName.c_str(), "", destFileList);
    std::set<std::string> hiddenFilesList;
    ok = ok && recursiveFileList(commonParentName.c_str(), "", hiddenFilesList);

    if (!ok) {
        return -1;
    }

    for (const auto& srcIt : srcFileList)
    {
        std::string srcFileName = std::string{srcDirName}.append(PATH_SEPARATOR).append(srcIt);
        if (isHidden(srcFileName)) {
            continue;
        }

        auto destPos = destFileList.find(srcIt);
        if (destPos != destFileList.end())
        {
            std::string destFileName = destDirName + PATH_SEPARATOR + (*destPos);
            auto hiddenDestPos = hiddenFilesList.find(srcIt);
            if (hiddenDestPos != hiddenFilesList.end())
            {
                std::string hiddenFileName = commonParentName + PATH_SEPARATOR + (*hiddenDestPos);
                fileMerge(srcFileName, destFileName, hiddenFileName);
            }
            else
            {
                printf("Could not merge automatically, use mergetool\n");
            }
            destFileList.erase(destPos);
        }
        //         else
        //         {
        //             std::set<std::string>::iterator hiddenDestPos=hiddenFilesList.find(*srcIt);
        //             if (hiddenDestPos==hiddenFilesList.end())
        //             {
        //                 output << "File  " << srcDirName+PATH_SEPARATOR +(*srcIt) << " has been added to the original context" << endl;
        //             }
        //         }
    }

    //     for(std::set<std::string>::iterator destIt=destFileList.begin(); destIt !=destFileList.end(); ++destIt)
    //     {
    //         std::set<std::string>::iterator hiddenDestPos=hiddenFilesList.find(*destIt);
    //         if (hiddenDestPos==hiddenFilesList.end())
    //             output << "File " << destDirName+PATH_SEPARATOR +(*destIt) << " does not belong to the original context" << endl;
    //     }

    return (ok ? 0 : 1);//tbm
}

#ifndef YARP_NO_DEPRECATED // Since YARP 3.4
int import(yarp::os::Bottle& importArg, folderType fType, bool verbose)
#else
int import(yarp::os::Bottle& importArg, folderType fType)
#endif
{
    std::string contextName;
    if (importArg.size() > 1) {
        contextName = importArg.get(1).asString();
    }
    if (contextName == "")
    {
        printf("No %s name provided\n", fType == CONTEXTS ? "context" : "robot");
        return 0;
    }
    yarp::os::ResourceFinder rf;
#ifndef YARP_NO_DEPRECATED // Since YARP 3.4
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
    rf.setVerbose(verbose);
YARP_WARNING_POP
#endif
    ResourceFinderOptions opts;
    opts.searchLocations = ResourceFinderOptions::Installed;
    std::string yarpdatahome = yarp::conf::dirs::yarpdatahome();
    std::string originalpath = rf.findPath(getFolderStringName(fType).append(PATH_SEPARATOR).append(contextName), opts);
    std::string destDirname = yarpdatahome.append(PATH_SEPARATOR).append(getFolderStringName(fType)).append(PATH_SEPARATOR).append(contextName);
    //tmp:
    std::string hiddenDirname = yarpdatahome.append(PATH_SEPARATOR).append(getFolderStringNameHidden(fType)).append(PATH_SEPARATOR).append(contextName);
    prepareHomeFolder(rf, fType);
    if (fType== CONTEXTS && importArg.size() >= 3)
    {
        yarp::os::mkdir((destDirname).c_str());
        yarp::os::mkdir((hiddenDirname).c_str());
        bool ok = true;
        for (size_t i = 2; i<importArg.size(); ++i)
        {
            std::string fileName = importArg.get(i).asString();
            if (fileName != "")
            {
                ok = prepareSubFolders(destDirname, fileName);
                ok = (recursiveCopy(std::string(originalpath).append(PATH_SEPARATOR).append(fileName), std::string(destDirname).append(PATH_SEPARATOR).append(fileName)) >= 0) && ok;
                if (ok)
                {
                    prepareSubFolders(hiddenDirname, fileName);
                    recursiveCopy(std::string(originalpath).append(PATH_SEPARATOR).append(fileName), std::string(hiddenDirname).append(PATH_SEPARATOR).append(fileName), true, false);
                }
            }
        }
        if (ok)
        {
            printf("Copied selected files to %s\n", destDirname.c_str());
            return 0;
        }
        else
        {
            printf("ERRORS OCCURRED WHILE IMPORTING FILES FOR %s %s\n", fType == CONTEXTS ? "CONTEXT" : "ROBOT", contextName.c_str());
            return 1;
        }
    }
    else if ((fType == ROBOTS   && importArg.size() == 2) ||
             (fType == CONTEXTS && importArg.size() == 2))
    {
        int result = recursiveCopy(originalpath, destDirname);
        recursiveCopy(originalpath, hiddenDirname, true, false);

        if (result < 0) {
            printf("ERRORS OCCURRED WHILE IMPORTING %s %s\n", fType == CONTEXTS ? "CONTEXT" : "ROBOT", contextName.c_str());
        } else {
            printf("Copied %s %s from %s to %s .\n", fType == CONTEXTS ? "context" : "robot", contextName.c_str(), originalpath.c_str(), destDirname.c_str());
            printf("Current locations for this %s:\n", fType == CONTEXTS ? "context" : "robot");
            yarp::os::Bottle paths = rf.findPaths(getFolderStringName(fType).append(PATH_SEPARATOR).append(contextName));
            for (size_t curCont = 0; curCont < paths.size(); ++curCont) {
                printf("%s\n", paths.get(curCont).asString().c_str());
            }
        }
        return result;
    }
    else
    {
        yError("Invalid number of parameters");
        return 1;
    }
}

#ifndef YARP_NO_DEPRECATED // Since YARP 3.4
int importAll(folderType fType, bool verbose)
#else
int importAll(folderType fType)
#endif
{
    yarp::os::ResourceFinder rf;
#ifndef YARP_NO_DEPRECATED // Since YARP 3.4
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
    rf.setVerbose(verbose);
YARP_WARNING_POP
#endif

    prepareHomeFolder(rf, fType);
    ResourceFinderOptions opts;
    opts.searchLocations = ResourceFinderOptions::Installed;
    std::string yarpdatahome = yarp::conf::dirs::yarpdatahome();
    yarp::os::Bottle contextPaths = rf.findPaths(getFolderStringName(fType), opts);
    for (size_t curPathId = 0; curPathId<contextPaths.size(); ++curPathId)
    {

        std::string curPath = contextPaths.get(curPathId).toString();

        yarp::os::impl::dirent **namelist;
        int n = yarp::os::impl::scandir(curPath.c_str(), &namelist, nullptr, yarp::os::impl::alphasort);
        if (n<0) {
            continue;
        }
        for (int i = 0; i<n; i++) {

            std::string name = namelist[i]->d_name;

            if (name != "." && name != "..")
            {
                yarp::os::impl::YARP_stat statbuf;
                std::string originalpath = std::string(curPath).append(PATH_SEPARATOR).append(name);
                yarp::os::impl::stat(originalpath.c_str(), &statbuf);
                if ((statbuf.st_mode & S_IFMT) == S_IFDIR)
                {
                    std::string destDirname = yarpdatahome.append(PATH_SEPARATOR).append(getFolderStringName(fType)).append(PATH_SEPARATOR).append(name);
                    recursiveCopy(originalpath, destDirname);
                    std::string hiddenDirname = yarpdatahome.append(PATH_SEPARATOR).append(getFolderStringNameHidden(fType)).append(PATH_SEPARATOR).append(name);
                    recursiveCopy(originalpath, hiddenDirname, true, false);// TODO: check result!
                }
            }
            free(namelist[i]);
        }
        free(namelist);
    }

    printf("New user %s:\n", fType == CONTEXTS ? "contexts" : "robots");
    printContentDirs(yarpdatahome.append(PATH_SEPARATOR).append(getFolderStringName(fType)));
    return 0;
}

#ifndef YARP_NO_DEPRECATED // Since YARP 3.4
int remove(yarp::os::Bottle& removeArg, folderType fType, bool verbose)
#else
int remove(yarp::os::Bottle& removeArg, folderType fType)
#endif
{
    std::string contextName;
    if (removeArg.size() > 1) {
        contextName = removeArg.get(1).asString();
    }
    if (contextName == "")
    {
        printf("No %s name provided\n", fType == CONTEXTS ? "context" : "robot");
        return 0;
    }
    yarp::os::ResourceFinder rf;
#ifndef YARP_NO_DEPRECATED // Since YARP 3.4
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
    rf.setVerbose(verbose);
YARP_WARNING_POP
#endif
    ResourceFinderOptions opts;
    opts.searchLocations = ResourceFinderOptions::User;
    std::string targetPath = rf.findPath(getFolderStringName(fType).append(PATH_SEPARATOR).append(contextName), opts);
    if (targetPath == "")
    {
        printf("Could not find %s %s !\n", fType == CONTEXTS ? "context" : "robot", contextName.c_str());
        return 0;
    }
    else
    {
        if ((fType == ROBOTS && removeArg.size() == 2) ||
            (fType == CONTEXTS && removeArg.size() == 2))
        {
            char choice = 'n';
            printf("Are you sure you want to remove this folder: %s ? (y/n): ", targetPath.c_str());
            int got = scanf("%c", &choice);
            YARP_UNUSED(got);
            if (choice == 'y')
            {
                int result = recursiveRemove(targetPath);
                if (result < 0) {
                    printf("ERRORS OCCURRED WHILE REMOVING %s\n", targetPath.c_str());
                } else {
                    printf("Removed folder %s\n", targetPath.c_str());
                }
                //remove hidden folder:
                std::string hiddenPath = rf.findPath(getFolderStringNameHidden(fType).append(PATH_SEPARATOR).append(contextName), opts);
                if (hiddenPath != "") {
                    recursiveRemove(hiddenPath, false);
                }
                return result;
            }
            else
            {
                printf("Skipped\n");
                return 0;
            }
        }
        else if (fType == CONTEXTS && removeArg.size() >= 3)
        {
            char choice = 'n';
            printf("Are you sure you want to remove files from this folder: %s ? (y/n): ", targetPath.c_str());
            int got = scanf("%c", &choice);
            YARP_UNUSED(got);
            if (choice == 'y')
            {
                bool ok = true;
                bool removeHidden = true;
                std::string hiddenPath = rf.findPath(getFolderStringNameHidden(fType).append(PATH_SEPARATOR).append(contextName), opts);
                if (hiddenPath != "") {
                    removeHidden = false;
                }

                for (size_t i = 2; i<removeArg.size(); ++i)
                {
                    std::string fileName = removeArg.get(i).asString();
                    if (fileName != "")
                    {
                        ok = (recursiveRemove(std::string(targetPath).append(PATH_SEPARATOR).append(fileName)) >= 0) && ok;
                        if (removeHidden) {
                            recursiveRemove(std::string(hiddenPath).append(PATH_SEPARATOR).append(fileName), false);
                        }
                    }
                }
                if (ok)
                {
                    printf("Removed selected files from %s\n", targetPath.c_str());
                    return 0;
                }
                else
                {
                    printf("ERRORS OCCURRED WHILE IMPORTING FILES FOR %s %s\n", fType == CONTEXTS ? "CONTEXT" : "ROBOT", contextName.c_str());
                    return 1;
                }
            }
            else
            {
                printf("Skipped\n");
                return 0;
            }
        }
        else
        {
            yError ("Invalid number of parameters");
            return 1;
        }
    }
}

#ifndef YARP_NO_DEPRECATED // Since YARP 3.4
int diff(std::string contextName, folderType fType, bool verbose)
#else
int diff(std::string contextName, folderType fType)
#endif
{
    yarp::os::ResourceFinder rf;
#ifndef YARP_NO_DEPRECATED // Since YARP 3.4
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
    rf.setVerbose(verbose);
YARP_WARNING_POP
#endif

    ResourceFinderOptions opts;
    opts.searchLocations = ResourceFinderOptions::User;
    std::string userPath = rf.findPath(getFolderStringName(fType).append(PATH_SEPARATOR).append(contextName), opts);

    opts.searchLocations = ResourceFinderOptions::Installed;
    std::string installedPath = rf.findPath(getFolderStringName(fType).append(PATH_SEPARATOR).append(contextName), opts);

#ifdef DO_TEXT_DIFF
    //use this for an internal diff implementation
    recursiveDiff(installedPath, userPath);
    return 0;
#else
    //use this for an external diff program
    char command[500];
#if defined(_WIN32)
    strcpy(command, "winmerge ");
#else
    strcpy(command, "meld ");
#endif
    strcat(command, installedPath.c_str());
    strcat(command, " ");
    strcat(command, userPath.c_str());
    return std::system(command);
#endif
}

#ifndef YARP_NO_DEPRECATED // Since YARP 3.4
int diffList(folderType fType, bool verbose)
#else
int diffList(folderType fType)
#endif
{
    yarp::os::ResourceFinder rf;
    ResourceFinderOptions opts;
    opts.searchLocations = ResourceFinderOptions::Installed;
    Bottle installedPaths = rf.findPaths(getFolderStringName(fType), opts);
    for (size_t n = 0; n <installedPaths.size(); ++n)
    {
        std::string installedPath = installedPaths.get(n).asString();
        std::vector<std::string> subDirs = listContentDirs(installedPath);
        for (auto& subDir : subDirs)
        {
            ostream tmp(nullptr);
            opts.searchLocations = ResourceFinderOptions::User;
#ifndef YARP_NO_DEPRECATED // Since YARP 3.4
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
            rf.setQuiet();
YARP_WARNING_POP
#endif
            std::string userPath = rf.findPath(getFolderStringName(fType).append(PATH_SEPARATOR).append(subDir), opts);
            if (userPath == "") {
                continue;
            }
            try
            {
                if (recursiveDiff(installedPath.append(PATH_SEPARATOR).append(subDir), userPath, tmp) > 0) {
                    std::cout << subDir << std::endl;
                }
            }
            catch (...)
            {
                printf("Exception occurred during call to diffList() on path \"%s\"\n", userPath.c_str());
            }
        }
    }

    return 0;
}

#ifndef YARP_NO_DEPRECATED // Since YARP 3.4
int merge(yarp::os::Bottle& mergeArg, folderType fType, bool verbose)
#else
int merge(yarp::os::Bottle& mergeArg, folderType fType)
#endif
{
    std::string contextName;
    if (mergeArg.size() > 1) {
        contextName = mergeArg.get(1).asString();
    }
    if (contextName == "")
    {
        printf("No %s name provided\n", fType == CONTEXTS ? "context" : "robot");
        return 0;
    }
    yarp::os::ResourceFinder rf;
#ifndef YARP_NO_DEPRECATED // Since YARP 3.4
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
    rf.setVerbose(verbose);
YARP_WARNING_POP
#endif

    if (mergeArg.size() >2)
    {
        for (size_t i = 2; i<mergeArg.size(); ++i)
        {
            std::string fileName = mergeArg.get(i).asString();
            if (fileName != "")
            {
                ResourceFinderOptions opts;
                opts.searchLocations = ResourceFinderOptions::User;
                std::string userFileName = rf.findPath(getFolderStringName(fType).append(PATH_SEPARATOR).append(contextName).append(PATH_SEPARATOR).append(fileName), opts);

                std::string hiddenFileName = rf.findPath(getFolderStringNameHidden(fType).append(PATH_SEPARATOR).append(contextName).append(PATH_SEPARATOR).append(fileName), opts);

                opts.searchLocations = ResourceFinderOptions::Installed;
                std::string installedFileName = rf.findPath(getFolderStringName(fType).append(PATH_SEPARATOR).append(contextName).append(PATH_SEPARATOR).append(fileName), opts);

                if (userFileName != "" && hiddenFileName != "" && installedFileName != "") {
                    fileMerge(installedFileName, userFileName, hiddenFileName);
                } else if (userFileName != "" && installedFileName != "") {
                    printf("Need to use mergetool\n");
                } else {
                    printf("Could not merge file %s\n", fileName.c_str());
                }
            }
        }
    }
    else
    {
        ResourceFinderOptions opts;
        opts.searchLocations = ResourceFinderOptions::User;
        std::string userPath = rf.findPath(getFolderStringName(fType).append(PATH_SEPARATOR).append(contextName), opts);

        std::string hiddenUserPath = rf.findPath(getFolderStringNameHidden(fType).append(PATH_SEPARATOR).append(contextName), opts);

        opts.searchLocations = ResourceFinderOptions::Installed;
        std::string installedPath = rf.findPath(getFolderStringName(fType).append(PATH_SEPARATOR).append(contextName), opts);

        recursiveMerge(installedPath, userPath, hiddenUserPath);
    }
    return 0;
}
