/*
 * Copyright (C) 2013 Istituto Italiano di Tecnologia
 * Authors: Elena Ceseracciu
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */
#include <set>
#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>

#include <yarp/os/Os.h>
#include <yarp/os/impl/PlatformStdlib.h>
#include "yarpcontextutils.h"

#if defined(WIN32)
#pragma warning (disable : 4018)

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

#include "diff_match_patch.h"

#if defined(WIN32)
#pragma warning(default:4018)
#endif

#include <errno.h>
//#include <stdio>

using namespace yarp::os;
using namespace std;

ConstString getFolderStringName(folderType ftype)
{
    switch(ftype)
    {
        case 0:
            return ConstString("contexts");
        case 1:
            return ConstString("robots");
        default:
            return ConstString("");
    }
}

ConstString getFolderStringNameHidden(folderType ftype)
{
    switch(ftype)
    {
        case 0:
            return ConstString(".contexts");
        case 1:
            return ConstString(".robots");
        default:
            return ConstString("");
    }
}

bool isHidden(ConstString fileName)
{
#ifdef WIN32
    DWORD attributes = GetFileAttributes(fileName.c_str());
    if (attributes & FILE_ATTRIBUTE_HIDDEN)
        return true;
#else
    if (fileName[0] == '.')
        return true;
#endif
    return false;
}

bool fileCopy(yarp::os::ConstString srcFileName, yarp::os::ConstString destFileName, bool force, bool verbose)
{
    char buf[BUFSIZ];
    size_t size;
    FILE* source = fopen(srcFileName.c_str(), "rb");
    if (source == NULL)
    {
        if (verbose)
            printf("Could not open source file %s\n", srcFileName.c_str());
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
            if (verbose)
                printf("File already exists : %s\n",destFileName.c_str());
            fclose(source);
            return false;
        }
    }
    FILE* dest = fopen(destFileName.c_str(), "wb");
    if(dest ==NULL)
    {
        if (verbose)
            printf("Could not open target file %s\n",destFileName.c_str());
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

bool fileRemove(ConstString fileName)
{
    return !YARP_unlink(fileName.c_str());
}

int recursiveCopy(ConstString srcDirName, ConstString destDirName, bool force, bool verbose)
{
    bool ok=true;
    ACE_stat statbuf;
    if( YARP_stat(srcDirName.c_str(), &statbuf) ==-1)
    {
        if (verbose)
            printf("Error in checking properties for %s\n", srcDirName.c_str());
        return -1;
    }
    if ((statbuf.st_mode & S_IFMT)== S_IFREG)
        ok = fileCopy(srcDirName, destDirName, force, verbose) && ok;
    else if ((statbuf.st_mode & S_IFMT)== S_IFDIR)
    {
        struct YARP_DIRENT **namelist;
        int n = YARP_scandir(srcDirName.c_str(),&namelist,NULL,YARP_alphasort);
        if (n<0)
        {
            if (verbose)
                std::cerr << "Could not read from directory "<< srcDirName <<std::endl;
            return -1;
        }
        if (yarp::os::mkdir((destDirName).c_str()) < 0)
        {
            if (errno == EEXIST && force)
            {
                bool ok= !recursiveRemove(destDirName, verbose);
                ok =ok && !yarp::os::mkdir((destDirName).c_str());
                if (!ok)
                {
                    if (verbose)
                        printf("Could not create directory %s\n", destDirName.c_str());
                    return -1;
                }
            }
            else
            {
                if (verbose)
                    if (errno == EEXIST)
                        printf("Directory %s already exist; remove it first, or use the diff/merge commands\n", destDirName.c_str());
                    else
                        printf("Could not create directory %s\n", destDirName.c_str());
                return -1;
            }
        }
        for (int i=0; i<n; i++)
        {

            ConstString name = namelist[i]->d_name;

            if( name != "." && name != "..")
            {
                ConstString srcPath=srcDirName + PATH_SEPARATOR + name;
                ConstString destPath=destDirName + PATH_SEPARATOR + name;
                recursiveCopy(srcPath, destPath, force, verbose);
            }
            free(namelist[i]);
        }
        free(namelist);

    }
    return (ok==true ? 0 :-1);
}

int recursiveRemove(ConstString dirName, bool verbose)
{
    ACE_stat statbuf;
    if( YARP_stat(dirName.c_str(), &statbuf) ==-1)
    {
        if (verbose)
            printf("Error in checking properties for %s\n", dirName.c_str());
        return -1;
    }
    if ((statbuf.st_mode & S_IFMT)== S_IFREG)
        return fileRemove(dirName) ? 0: -1;
    else if ((statbuf.st_mode & S_IFMT)== S_IFDIR)
    {
        struct YARP_DIRENT **namelist;
        int n = YARP_scandir(dirName.c_str(),&namelist,NULL,YARP_alphasort);
        if (n<0)
        {
            if (verbose)
                printf("Could not read from  directory %s\n", dirName.c_str());
            return yarp::os::rmdir(dirName.c_str()); // TODO check if this is useful...
        }

        for (int i=0; i<n; i++)
        {
            ConstString name = namelist[i]->d_name;
            ConstString path=dirName + PATH_SEPARATOR + name;
            if( name != "." && name != "..")
            {
                recursiveRemove(path, verbose);
            }
            free(namelist[i]);
        }
        free(namelist);

        return yarp::os::rmdir(dirName.c_str());
    }
    else
        return -1;
}

std::vector<yarp::os::ConstString> listContentDirs(const ConstString &curPath)
{
    std::vector<yarp::os::ConstString> dirsList;
    struct YARP_DIRENT **namelist;
    int n = YARP_scandir(curPath.c_str(),&namelist,NULL,YARP_alphasort);
    if (n<0) {
        return dirsList;
    }
    for (int i=0; i<n; i++) {

        ConstString name = namelist[i]->d_name;
        if( name != "." && name != "..")
        {
            ACE_stat statbuf;
            ConstString path=curPath + PATH_SEPARATOR + name;
            if (YARP_stat(path.c_str(), &statbuf) == -1)
                printf("Error in checking properties for %s\n", path.c_str());

            if ((statbuf.st_mode & S_IFMT)== S_IFDIR)
                dirsList.push_back(name);
        }
        free(namelist[i]);
    }
    free(namelist);
    return dirsList;
}

std::vector<yarp::os::ConstString> listContentFiles(const ConstString &curPath)
{
    std::vector<yarp::os::ConstString> fileList;
    struct YARP_DIRENT **namelist;
    int n = YARP_scandir(curPath.c_str(),&namelist,NULL,YARP_alphasort);
    if (n<0) {
        return fileList;
    }
    std::list<std::string> fileStack;
    for (int i=0; i<n; i++)
    {
        fileStack.push_back(namelist[i]->d_name);
        free(namelist[i]);
    }
    free(namelist);

    while(!fileStack.empty())
    {
        ConstString name= fileStack.front();

        if( name != "." && name != "..")
        {
            ACE_stat statbuf;
            ConstString path=curPath + PATH_SEPARATOR + name;
            if (YARP_stat(path.c_str(), &statbuf) == -1)
                printf("Error in checking properties for %s\n", path.c_str());
            if ((statbuf.st_mode & S_IFMT)== S_IFREG)
            {
                fileList.push_back(path.c_str());
            }

            if ((statbuf.st_mode & S_IFMT)== S_IFDIR)
            {
                std::vector<yarp::os::ConstString> nestedFiles=listContentFiles(path);
                for(std::vector<yarp::os::ConstString>::iterator nestedIt=nestedFiles.begin(); nestedIt !=nestedFiles.end(); ++nestedIt)
                    fileStack.push_back((path + PATH_SEPARATOR + (*nestedIt)).c_str());
            }
        }
        fileStack.pop_front();
    }
    return fileList;
}

void printContentDirs(const ConstString &curPath)
{
    std::vector<yarp::os::ConstString> dirsList=listContentDirs(curPath);
    for(std::vector<yarp::os::ConstString>::iterator dirsIt=dirsList.begin(); dirsIt!=dirsList.end(); ++dirsIt)
        printf("%s\n",(*dirsIt).c_str());
}


void printUserFolders(yarp::os::ResourceFinder &rf, folderType ftype)
{
    ResourceFinderOptions opts;
    opts.searchLocations=ResourceFinderOptions::User;
    yarp::os::Bottle contextPaths=rf.findPaths(getFolderStringName(ftype), opts);
    printf("**LOCAL USER DATA:\n");
    for (int curPathId=0; curPathId<contextPaths.size(); ++curPathId)
    {
        printf("* Directory : %s\n", contextPaths.get(curPathId).asString().c_str());
        printContentDirs(contextPaths.get(curPathId).asString());
    }
}

void printSysadmFolders(yarp::os::ResourceFinder &rf, folderType ftype)
{
    ResourceFinderOptions opts;
    opts.searchLocations=ResourceFinderOptions::Sysadmin;
    yarp::os::Bottle contextPaths=rf.findPaths(getFolderStringName(ftype), opts);
    printf("**SYSADMIN DATA:\n");
    for (int curPathId=0; curPathId<contextPaths.size(); ++curPathId)
    {
        printf("* Directory : %s\n", contextPaths.get(curPathId).asString().c_str());
        printContentDirs(contextPaths.get(curPathId).asString());
    }
}

void printInstalledFolders(yarp::os::ResourceFinder &rf, folderType ftype)
{
    ResourceFinderOptions opts;
    opts.searchLocations=ResourceFinderOptions::Installed;
    yarp::os::Bottle contextPaths=rf.findPaths(getFolderStringName(ftype), opts);
    printf("**INSTALLED DATA:\n");
    for (int curPathId=0; curPathId<contextPaths.size(); ++curPathId)
    {
        printf("* Directory : %s\n", contextPaths.get(curPathId).asString().c_str());
        printContentDirs(contextPaths.get(curPathId).asString());
    }
}

void prepareHomeFolder(yarp::os::ResourceFinder &rf, folderType ftype)
{
    ACE_DIR* dir= YARP_opendir((rf.getDataHome()).c_str());
    if (dir!=NULL)
        YARP_closedir(dir);
    else
    {
        yarp::os::mkdir((rf.getDataHome()).c_str());
    }

    dir= YARP_opendir((rf.getDataHome() + PATH_SEPARATOR + getFolderStringName(ftype)).c_str());
    if (dir!=NULL)
        YARP_closedir(dir);
    else
    {
        yarp::os::mkdir((rf.getDataHome() + PATH_SEPARATOR + getFolderStringName(ftype)).c_str());
    }
    dir= YARP_opendir((rf.getDataHome() + PATH_SEPARATOR + getFolderStringNameHidden(ftype)).c_str());
    if (dir!=NULL)
        YARP_closedir(dir);
    else
    {
        ConstString hiddenPath=(rf.getDataHome() + PATH_SEPARATOR + getFolderStringNameHidden(ftype));
        yarp::os::mkdir(hiddenPath.c_str());
#ifdef WIN32
        SetFileAttributes(hiddenPath.c_str(), FILE_ATTRIBUTE_HIDDEN);
#endif
    }
}

bool prepareSubFolders(const yarp::os::ConstString& startDir, const yarp::os::ConstString& fileName)
{
    string fname(fileName);
    if(fname.find(PATH_SEPARATOR)==string::npos)
         return true;
    else
    {
        size_t curPos, startPos=0;
        while((curPos=fname.find(PATH_SEPARATOR, startPos))!=string::npos)
        {
            yarp::os::mkdir((startDir+PATH_SEPARATOR+fname.substr(0, curPos)).c_str());
            startPos=curPos+string(PATH_SEPARATOR).length();
        }
    }
    return true;
}

bool recursiveFileList(const char* basePath, const char* suffix, std::set<std::string>& filenames)
{
    string strPath = string (basePath);
    if((strPath.rfind(PATH_SEPARATOR)==string::npos) ||
            (strPath.rfind(PATH_SEPARATOR)!=strPath.size()-1))
            strPath = strPath + string(PATH_SEPARATOR);

     string mySuffix=string(suffix);

    if(((mySuffix.rfind(PATH_SEPARATOR)==string::npos) ||
            (mySuffix.rfind(PATH_SEPARATOR)!=mySuffix.size()-1)) && mySuffix!="")
            mySuffix = mySuffix + string(PATH_SEPARATOR);

    strPath += mySuffix;

    struct YARP_DIRENT **namelist;
    int n = YARP_scandir(strPath.c_str(),&namelist,NULL,YARP_alphasort);
    if (n<0)
    {
        std::cerr << "Could not read from  directory " << strPath <<std::endl;
        return false;
    }
    bool ok=true;
    for (int i=0; i<n; i++)
    {

        ConstString name = namelist[i]->d_name;
        ACE_stat statbuf;
        if( name != "." && name != "..")
        {
            YARP_stat((strPath + PATH_SEPARATOR + name.c_str() ).c_str(), &statbuf);
            if ((statbuf.st_mode & S_IFMT)== S_IFREG)
            {
                filenames.insert(mySuffix+name.c_str());
            }
            else
                if ((statbuf.st_mode & S_IFMT)== S_IFDIR)
                {
                    ok=ok && recursiveFileList(basePath, (mySuffix+ name.c_str()).c_str(), filenames);
                }
        }
        free(namelist[i]);
    }
    free(namelist);
    return ok;
}


int recursiveDiff(yarp::os::ConstString srcDirName, yarp::os::ConstString destDirName, std::ostream &output)
{
    std::set<std::string> srcFileList;
    bool ok = recursiveFileList(srcDirName.c_str(), "", srcFileList);
    std::set<std::string> destFileList;
    ok=ok && recursiveFileList(destDirName.c_str(), "", destFileList);

    if (!ok)
        return -1;
    size_t nModifiedFiles=0;
    for(std::set<std::string>::iterator srcIt=srcFileList.begin(); srcIt !=srcFileList.end(); ++srcIt)
    {
        std::set<std::string>::iterator destPos=destFileList.find(*srcIt);
        if (destPos!=destFileList.end())
        {
            diff_match_patch<std::string> dmp;
            ConstString srcFileName=srcDirName+ PATH_SEPARATOR + (*srcIt);
            if (isHidden(srcFileName))
                continue;

            std::ifstream in(srcFileName.c_str());
            std::string srcStr((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
            in.close();
            in.open((destDirName+ PATH_SEPARATOR +(*destPos)).c_str());
            std::string destStr((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
            std::string patchString = dmp.patch_toText(dmp.patch_make(srcStr, destStr));
            if (patchString!= "")
            {
                output << "- " << srcDirName + PATH_SEPARATOR + (*srcIt)<<endl;
                output << "+ " << destDirName + PATH_SEPARATOR + (*destPos) <<endl;
                output << dmp.patch_toText(dmp.patch_make(srcStr, destStr))<<std::endl;
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
//         ConstString destFileName=destDirName+ PATH_SEPARATOR + (*destIt);
//         if(isHidden(destFileName))
//             continue;
//
//         output << "Removed file " << destFileName <<endl;
//         nModifiedFiles++;
//
//     }

    return nModifiedFiles;//tbm
}

int fileMerge(yarp::os::ConstString srcFileName, yarp::os::ConstString destFileName, yarp::os::ConstString commonParentName)
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

int recursiveMerge(yarp::os::ConstString srcDirName, yarp::os::ConstString destDirName, yarp::os::ConstString commonParentName, std::ostream &output)
{
    std::set<std::string> srcFileList;
    bool ok = recursiveFileList(srcDirName.c_str(), "", srcFileList);
    std::set<std::string> destFileList;
    ok=ok && recursiveFileList(destDirName.c_str(), "", destFileList);
    std::set<std::string> hiddenFilesList;
    ok=ok && recursiveFileList(commonParentName.c_str(), "", hiddenFilesList);

    if (!ok)
        return -1;

    for(std::set<std::string>::iterator srcIt=srcFileList.begin(); srcIt !=srcFileList.end(); ++srcIt)
    {
            ConstString srcFileName=srcDirName+ PATH_SEPARATOR + (*srcIt);
            if (isHidden(srcFileName))
                continue;

        std::set<std::string>::iterator destPos=destFileList.find(*srcIt);
        if (destPos!=destFileList.end())
        {
            ConstString destFileName=destDirName+ PATH_SEPARATOR + (*destPos);
            std::set<std::string>::iterator hiddenDestPos=hiddenFilesList.find(*srcIt);
            if (hiddenDestPos!=hiddenFilesList.end())
            {
                ConstString hiddenFileName=commonParentName+ PATH_SEPARATOR + (*hiddenDestPos);
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

    return (ok? 0: 1);//tbm
}


int import(yarp::os::Bottle& importArg, folderType fType, bool verbose)
{
    ConstString contextName;
    if (importArg.size() >1 )
        contextName=importArg.get(1).asString().c_str();
    if (contextName=="")
    {
        printf("No %s name provided\n", fType==CONTEXTS ? "context" : "robot" );
        return 0;
    }
    yarp::os::ResourceFinder rf;
    rf.setVerbose(verbose);
    ResourceFinderOptions opts;
    opts.searchLocations=ResourceFinderOptions::Installed;
    ConstString originalpath=rf.findPath((getFolderStringName(fType) + PATH_SEPARATOR +contextName).c_str(), opts);
    ConstString destDirname=rf.getDataHome() + PATH_SEPARATOR + getFolderStringName(fType) + PATH_SEPARATOR + contextName;
    //tmp:
    ConstString hiddenDirname=rf.getDataHome() + PATH_SEPARATOR + getFolderStringNameHidden(fType) + PATH_SEPARATOR + contextName;
    prepareHomeFolder(rf, fType);
    if (importArg.size() >2 )
    {

        yarp::os::mkdir((destDirname).c_str());
        yarp::os::mkdir((hiddenDirname).c_str());
        bool ok=true;
        for (int i=2; i<importArg.size(); ++i)
        {
            ConstString fileName=importArg.get(i).asString();
            if(fileName != "")
            {
                ok= prepareSubFolders(destDirname, fileName);
                ok = (recursiveCopy(originalpath+ PATH_SEPARATOR + fileName, destDirname + PATH_SEPARATOR + fileName) >=0 ) && ok;
                if (ok)
                {
                    prepareSubFolders(hiddenDirname, fileName);
                    recursiveCopy(originalpath+ PATH_SEPARATOR + fileName, hiddenDirname + PATH_SEPARATOR + fileName, true, false);
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
            printf("ERRORS OCCURRED WHILE IMPORTING FILES FOR %s %s\n", fType==CONTEXTS ? "CONTEXT" : "ROBOT" ,  contextName.c_str());
            return 1;
        }
    }
    else
    {

        int result= recursiveCopy(originalpath, destDirname);
        recursiveCopy(originalpath, hiddenDirname, true, false);

        if (result < 0)
            printf("ERRORS OCCURRED WHILE IMPORTING %s %s\n", fType==CONTEXTS ? "CONTEXT" : "ROBOT", contextName.c_str());
        else
        {
            printf("Copied %s %s from %s to %s .\n", fType==CONTEXTS ? "context" : "robot",  contextName.c_str(), originalpath.c_str(), destDirname.c_str());
            printf("Current locations for this %s:\n", fType==CONTEXTS ? "context" : "robot");
            yarp::os::Bottle paths=rf.findPaths((getFolderStringName(fType) + PATH_SEPARATOR +contextName).c_str());
            for (int curCont=0; curCont<paths.size(); ++curCont)
                printf("%s\n", paths.get(curCont).asString().c_str());
        }
        return result;
    }
}

int importAll(folderType fType, bool verbose)
{
    yarp::os::ResourceFinder rf;
    rf.setVerbose(verbose);

    prepareHomeFolder(rf, fType);
    ResourceFinderOptions opts;
    opts.searchLocations=ResourceFinderOptions::Installed;
    yarp::os::Bottle contextPaths=rf.findPaths(getFolderStringName(fType), opts);
    for (int curPathId=0; curPathId<contextPaths.size(); ++curPathId)
    {

        ConstString curPath= contextPaths.get(curPathId).toString();

        struct YARP_DIRENT **namelist;
        int n = YARP_scandir(curPath.c_str(),&namelist,NULL,YARP_alphasort);
        if (n<0) {
            continue;
        }
        for (int i=0; i<n; i++) {

            ConstString name = namelist[i]->d_name;

            if( name != "." && name != "..")
            {
                ACE_stat statbuf;
                ConstString originalpath=curPath + PATH_SEPARATOR + name;
                YARP_stat(originalpath.c_str(), &statbuf);
                if ((statbuf.st_mode & S_IFMT)== S_IFDIR)
                {
                    ConstString destDirname=rf.getDataHome() + PATH_SEPARATOR + getFolderStringName(fType) + PATH_SEPARATOR + name;
                    recursiveCopy(originalpath, destDirname);
                    ConstString hiddenDirname=rf.getDataHome() + PATH_SEPARATOR + getFolderStringNameHidden(fType) + PATH_SEPARATOR + name;
                    recursiveCopy(originalpath, hiddenDirname, true, false);// TODO: check result!
                }
            }
            free(namelist[i]);
        }
        free(namelist);
    }

    printf("New user %s:\n", fType==CONTEXTS ? "contexts" : "robots");
    printContentDirs(rf.getDataHome() + PATH_SEPARATOR + getFolderStringName(fType));
    return 0;
}

int remove(yarp::os::Bottle& removeArg, folderType fType, bool verbose)
{
    ConstString contextName;
    if (removeArg.size() >1 )
        contextName=removeArg.get(1).asString().c_str();
    if (contextName=="")
    {
        printf("No %s name provided\n", fType==CONTEXTS ? "context" : "robot" );
        return 0;
    }
    yarp::os::ResourceFinder rf;
    rf.setVerbose(verbose);
    ResourceFinderOptions opts;
    opts.searchLocations=ResourceFinderOptions::User;
    ConstString targetPath=rf.findPath((getFolderStringName(fType) + PATH_SEPARATOR +contextName).c_str(), opts);
    if (targetPath=="")
    {
        printf("Could not find %s %s !\n",fType==CONTEXTS ? "context" : "robot", contextName.c_str());
        return 0;
    }
    else
    {
        if (removeArg.size() <3 )
        {
            char choice='n';
            printf("Are you sure you want to remove this folder: %s ? (y/n): ", targetPath.c_str());
            int got = scanf("%c",&choice);
            if (choice=='y')
            {
                int result= recursiveRemove(targetPath.c_str());
                if (result < 0)
                    printf("ERRORS OCCURRED WHILE REMOVING %s\n", targetPath.c_str());
                else
                    printf("Removed folder %s\n", targetPath.c_str());
                //remove hidden folder:
                ConstString hiddenPath=   rf.findPath((getFolderStringNameHidden(fType) + PATH_SEPARATOR +contextName).c_str(), opts);
                if (hiddenPath != "")
                    recursiveRemove(hiddenPath.c_str(), false);
                return result;
            }
            else
            {
                printf("Skipped\n");
                return 0;
            }
        }
        else
        {
            char choice='n';
            printf("Are you sure you want to remove files from this folder: %s ? (y/n): ", targetPath.c_str());
            int got = scanf("%c",&choice);
            if (choice=='y')
            {
                bool ok=true;
                bool removeHidden=true;
                ConstString hiddenPath=   rf.findPath((getFolderStringNameHidden(fType) + PATH_SEPARATOR +contextName).c_str(), opts);
                if (hiddenPath != "")
                    removeHidden=false;

                for (int i=2; i<removeArg.size(); ++i)
                {
                    ConstString fileName=removeArg.get(i).asString();
                    if(fileName != "")
                    {
                        ok = (recursiveRemove(targetPath+ PATH_SEPARATOR + fileName) >=0 ) && ok;
                        recursiveRemove(hiddenPath + PATH_SEPARATOR + fileName, false);
                    }
                }
                if (ok)
                {
                    printf("Removed selected files from %s\n", targetPath.c_str());
                    return 0;
                }
                else
                {
                    printf("ERRORS OCCURRED WHILE IMPORTING FILES FOR %s %s\n", fType==CONTEXTS ? "CONTEXT" : "ROBOT" ,  contextName.c_str());
                    return 1;
                }
            }
            else
            {
                printf("Skipped\n");
                return 0;
            }
        }
    }
}

int diff(yarp::os::ConstString contextName, folderType fType, bool verbose)
{
    yarp::os::ResourceFinder rf;
    rf.setVerbose(verbose);

    ResourceFinderOptions opts;
    opts.searchLocations=ResourceFinderOptions::User;
    ConstString userPath=rf.findPath((getFolderStringName(fType) + PATH_SEPARATOR +contextName).c_str(), opts);

    opts.searchLocations=ResourceFinderOptions::Installed;
    ConstString installedPath=rf.findPath((getFolderStringName(fType) + PATH_SEPARATOR +contextName).c_str(), opts);

#ifdef DO_TEXT_DIFF
    //use this for an internal diff implementation
    recursiveDiff(installedPath, userPath);
#else
    //use this for an external diff program
    char command [500];
    #ifdef WIN32
        strcpy (command, "winmerge ");
    #else
        strcpy (command, "meld ");
    #endif
    strcat (command, installedPath.c_str());
    strcat (command, " ");
    strcat (command, userPath.c_str());
    std::system(command);
#endif
    
    return 0;
}

int diffList(folderType fType, bool verbose)
{
    yarp::os::ResourceFinder rf;
    ResourceFinderOptions opts;
    opts.searchLocations=ResourceFinderOptions::Installed;
    Bottle installedPaths=rf.findPaths(getFolderStringName(fType), opts);
    for(int n=0; n <installedPaths.size(); ++n)
    {
        std::string installedPath=installedPaths.get(n).asString();
        std::vector<yarp::os::ConstString> subDirs=listContentDirs(installedPath);
        for (std::vector<yarp::os::ConstString>::iterator subDirIt= subDirs.begin(); subDirIt!=subDirs.end(); ++subDirIt)
        {
            ostream tmp(0);
            opts.searchLocations=ResourceFinderOptions::User;
            rf.setQuiet();
            ConstString userPath=rf.findPath((getFolderStringName(fType) + PATH_SEPARATOR +(*subDirIt)).c_str(), opts);
            if (userPath == "")
                continue;
            if ( recursiveDiff(installedPath + PATH_SEPARATOR + (*subDirIt).c_str(), userPath, tmp)>0)
                std::cout<< (*subDirIt) <<std::endl;
        }
    }

    return 0;
}

int merge(yarp::os::Bottle& mergeArg, folderType fType, bool verbose)
{
    ConstString contextName;
    if (mergeArg.size() >1 )
        contextName=mergeArg.get(1).asString().c_str();
    if (contextName=="")
    {
        printf("No %s name provided\n", fType==CONTEXTS ? "context" : "robot");
        return 0;
    }
    yarp::os::ResourceFinder rf;
    rf.setVerbose(verbose);

    if (mergeArg.size() >2 )
    {
        for (int i=2; i<mergeArg.size(); ++i)
        {
            ConstString fileName=mergeArg.get(i).asString();
            if(fileName != "")
            {
                ResourceFinderOptions opts;
                opts.searchLocations=ResourceFinderOptions::User;
                ConstString userFileName=rf.findPath((getFolderStringName(fType) + PATH_SEPARATOR +contextName + PATH_SEPARATOR + fileName).c_str(), opts);

                ConstString hiddenFileName=rf.findPath((getFolderStringNameHidden(fType) + PATH_SEPARATOR +contextName+ PATH_SEPARATOR + fileName).c_str(), opts);

                opts.searchLocations=ResourceFinderOptions::Installed;
                ConstString installedFileName=rf.findPath((getFolderStringName(fType) + PATH_SEPARATOR +contextName+ PATH_SEPARATOR + fileName).c_str(), opts);

                if (userFileName!="" && hiddenFileName != "" && installedFileName !="")
                    fileMerge(installedFileName, userFileName, hiddenFileName);
                else if (userFileName!=""  && installedFileName !="")
                    printf("Need to use mergetool\n");
                else
                    printf("Could not merge file %s\n", fileName.c_str());
            }
        }
    }
    else
    {
        ResourceFinderOptions opts;
        opts.searchLocations=ResourceFinderOptions::User;
        ConstString userPath=rf.findPath((getFolderStringName(fType) + PATH_SEPARATOR +contextName).c_str(), opts);

        ConstString hiddenUserPath=rf.findPath((getFolderStringNameHidden(fType) + PATH_SEPARATOR +contextName).c_str(), opts);

        opts.searchLocations=ResourceFinderOptions::Installed;
        ConstString installedPath=rf.findPath((getFolderStringName(fType) + PATH_SEPARATOR +contextName).c_str(), opts);

        recursiveMerge(installedPath, userPath, hiddenUserPath);
    }
    return 0;
}
