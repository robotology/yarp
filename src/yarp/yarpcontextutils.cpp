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

#include <yarp/os/Os.h>
#include <yarp/os/impl/PlatformStdlib.h>
#include "yarpcontextutils.h"

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

bool fileCopy(yarp::os::ConstString srcFileName, yarp::os::ConstString destFileName)
{
    char buf[BUFSIZ];
    size_t size;
    FILE* source = fopen(srcFileName.c_str(), "rb");
    if (source == NULL)
    {
        printf("Could not open source file %s\n", srcFileName.c_str());
        return false;
    }
    if (!yarp::os::stat(destFileName.c_str()))
    {
        printf("File already exists : %s\n",destFileName.c_str());
        fclose(source);
        return false;
    }
    FILE* dest = fopen(destFileName.c_str(), "wb");
    if(dest ==NULL)
    {
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

int recursiveCopy(ConstString srcDirName, ConstString destDirName)
{
    bool ok=true;
    ACE_stat statbuf;
    if( YARP_stat(srcDirName.c_str(), &statbuf) ==-1)
    {
        printf("Error in checking properties for %s\n", srcDirName.c_str());
        return -1;
    }
    if ((statbuf.st_mode & S_IFMT)== S_IFREG)
        ok = fileCopy(srcDirName, destDirName) && ok;
    else if ((statbuf.st_mode & S_IFMT)== S_IFDIR)
    {
        struct YARP_DIRENT **namelist;
        int n = YARP_scandir(srcDirName.c_str(),&namelist,NULL,YARP_alphasort);
        if (n<0)
        {
            std::cerr << "Could not read from  directory "<< srcDirName <<std::endl;
            return -1;
        }
        if (yarp::os::mkdir((destDirName).c_str()) < 0)
        {
            if (errno == EEXIST)
                printf("Directory %s already exist; remove it first, or use the diff/merge commands\n", destDirName.c_str());
            else
                printf("Could not create directory %s\n", destDirName.c_str());
            return -1;
        }
        for (int i=0; i<n; i++)
        {

            ConstString name = namelist[i]->d_name;

            if( name != "." && name != "..")
            {
                ConstString srcPath=srcDirName + PATH_SEPERATOR + name;
                ConstString destPath=destDirName + PATH_SEPERATOR + name;
                recursiveCopy(srcPath, destPath);
            }
            free(namelist[i]);
        }
        free(namelist);

    }
    return (ok==true ? 0 :-1);
}

int recursiveRemove(ConstString dirName)
{
    struct YARP_DIRENT **namelist;
    int n = YARP_scandir(dirName.c_str(),&namelist,NULL,YARP_alphasort);
    if (n<0)
    {
        printf("Could not read from  directory %s\n", dirName.c_str());
        return yarp::os::rmdir(dirName.c_str()); // TODO check if this is useful...
    }

    for (int i=0; i<n; i++)
    {
        ConstString name = namelist[i]->d_name;
        ConstString path=dirName + PATH_SEPERATOR + name;
        if( name != "." && name != "..")
        {
            ACE_stat statbuf;
            if (YARP_stat(path.c_str(), &statbuf) == -1)
                printf("Error in checking properties for %s\n", path.c_str());
            if ((statbuf.st_mode & S_IFMT)== S_IFDIR)
                recursiveRemove(path);
            if ((statbuf.st_mode & S_IFMT)== S_IFREG)
            {
                YARP_unlink(path.c_str());
            }
        }
        free(namelist[i]);
    }
    free(namelist);

    return yarp::os::rmdir(dirName.c_str());
};

std::vector<std::string> listContentDirs(const ConstString &curPath)
{
    std::vector<std::string> dirsList;
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
            ConstString path=curPath + PATH_SEPERATOR + name;
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

std::vector<std::string> listContentFiles(const ConstString &curPath)
{
    std::vector<std::string> fileList;
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
            ConstString path=curPath + PATH_SEPERATOR + name;
            if (YARP_stat(path.c_str(), &statbuf) == -1)
                printf("Error in checking properties for %s\n", path.c_str());
            if ((statbuf.st_mode & S_IFMT)== S_IFREG)
            {
                fileList.push_back(path.c_str());
            }

            if ((statbuf.st_mode & S_IFMT)== S_IFDIR)
            {
                std::vector<std::string> nestedFiles=listContentFiles(path);
                for(std::vector<std::string>::iterator nestedIt=nestedFiles.begin(); nestedIt !=nestedFiles.end(); ++nestedIt)
                    fileStack.push_back((path + PATH_SEPERATOR + (*nestedIt)).c_str());
            }
        }
        fileStack.pop_front();
    }
    return fileList;
}

void printContentDirs(const ConstString &curPath)
{
    std::vector<std::string> dirsList=listContentDirs(curPath);
    for(std::vector<std::string>::iterator dirsIt=dirsList.begin(); dirsIt!=dirsList.end(); ++dirsIt)
        printf("%s\n",(*dirsIt).c_str());
};


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

    dir= YARP_opendir((rf.getDataHome() + PATH_SEPERATOR + getFolderStringName(ftype)).c_str());
    if (dir!=NULL)
        YARP_closedir(dir);
    else
    {
        yarp::os::mkdir((rf.getDataHome() + PATH_SEPERATOR + getFolderStringName(ftype)).c_str());
    }
    dir= YARP_opendir((rf.getDataHome() + PATH_SEPERATOR + getFolderStringNameHidden(ftype)).c_str());
    if (dir!=NULL)
        YARP_closedir(dir);
    else
    {
        ConstString hiddenPath=(rf.getDataHome() + PATH_SEPERATOR + getFolderStringNameHidden(ftype));
        yarp::os::mkdir(hiddenPath.c_str());
#ifdef WIN32
        SetFileAttributes(hiddenPath.c_str(), FILE_ATTRIBUTE_HIDDEN);
#endif
    }
}

bool recursiveFileList(const char* basePath, const char* suffix, std::set<std::string>& filenames)
{
    string strPath = string (basePath);
    if((strPath.rfind(PATH_SEPERATOR)==string::npos) ||
            (strPath.rfind(PATH_SEPERATOR)!=strPath.size()-1))
            strPath = strPath + string(PATH_SEPERATOR);

     string mySuffix=string(suffix);

    if(((mySuffix.rfind(PATH_SEPERATOR)==string::npos) ||
            (mySuffix.rfind(PATH_SEPERATOR)!=mySuffix.size()-1)) && mySuffix!="")
            mySuffix = mySuffix + string(PATH_SEPERATOR);

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
        if( name != "." && name != "..")
        {
            if (namelist[i]->d_type == DT_REG)
            {
                filenames.insert(mySuffix+name);
            }
            else
                if(namelist[i]->d_type == DT_DIR)
                {
                    ok=ok && recursiveFileList(basePath, (mySuffix+ name).c_str(), filenames);
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
            ConstString srcFileName=srcDirName+ PATH_SEPERATOR + (*srcIt);
            if (isHidden(srcFileName))
                continue;

            std::ifstream in(srcFileName.c_str());
            std::string srcStr((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
            in.close();
            in.open((destDirName+ PATH_SEPERATOR +(*destPos)).c_str());
            std::string destStr((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
            std::string patchString = dmp.patch_toText(dmp.patch_make(srcStr, destStr));
            if (patchString!= "")
            {
                output << "- " << srcDirName + PATH_SEPERATOR + (*srcIt)<<endl;
                output << "+ " << destDirName + PATH_SEPERATOR + (*destPos) <<endl;
                output << dmp.patch_toText(dmp.patch_make(srcStr, destStr))<<std::endl;
                nModifiedFiles++;
            }
            destFileList.erase(destPos);
        }
//         else
//         {
//             output << "Added file  " << srcDirName+PATH_SEPERATOR +(*srcIt) <<endl;
//             nModifiedFiles++;
//         }
    }

//     for(std::set<std::string>::iterator destIt=destFileList.begin(); destIt !=destFileList.end(); ++destIt)
//     {
//         ConstString destFileName=destDirName+ PATH_SEPERATOR + (*destIt);
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
            ConstString srcFileName=srcDirName+ PATH_SEPERATOR + (*srcIt);
            if (isHidden(srcFileName))
                continue;

        std::set<std::string>::iterator destPos=destFileList.find(*srcIt);
        if (destPos!=destFileList.end())
        {
            ConstString destFileName=destDirName+ PATH_SEPERATOR + (*destPos);
            std::set<std::string>::iterator hiddenDestPos=hiddenFilesList.find(*srcIt);
            if (hiddenDestPos!=hiddenFilesList.end())
            {
                ConstString hiddenFileName=commonParentName+ PATH_SEPERATOR + (*hiddenDestPos);
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
//                 output << "File  " << srcDirName+PATH_SEPERATOR +(*srcIt) << " has been added to the original context" << endl;
//             }
//         }
    }

//     for(std::set<std::string>::iterator destIt=destFileList.begin(); destIt !=destFileList.end(); ++destIt)
//     {
//         std::set<std::string>::iterator hiddenDestPos=hiddenFilesList.find(*destIt);
//         if (hiddenDestPos==hiddenFilesList.end())
//             output << "File " << destDirName+PATH_SEPERATOR +(*destIt) << " does not belong to the original context" << endl;
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
    ConstString originalpath=rf.findPath((getFolderStringName(fType) + PATH_SEPERATOR +contextName).c_str(), opts);
    ConstString destDirname=rf.getDataHome() + PATH_SEPERATOR + getFolderStringName(fType) + PATH_SEPERATOR + contextName;
    //tmp:
    ConstString hiddenDirname=rf.getDataHome() + PATH_SEPERATOR + getFolderStringNameHidden(fType) + PATH_SEPERATOR + contextName;
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
                ok = (recursiveCopy(originalpath+ PATH_SEPERATOR + fileName, destDirname + PATH_SEPERATOR + fileName) >=0 ) && ok;
                ok = ok && (recursiveCopy(originalpath+ PATH_SEPERATOR + fileName, hiddenDirname + PATH_SEPERATOR + fileName) >=0 );
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
        recursiveCopy(originalpath, hiddenDirname);

        if (result < 0)
            printf("ERRORS OCCURRED WHILE IMPORTING %s %s\n", fType==CONTEXTS ? "CONTEXT" : "ROBOT", contextName.c_str());
        else
        {
            printf("Copied %s %s from %s to %s .\n", fType==CONTEXTS ? "context" : "robot",  contextName.c_str(), originalpath.c_str(), destDirname.c_str());
            printf("Current locations for this %s:\n", fType==CONTEXTS ? "context" : "robot");
            yarp::os::Bottle paths=rf.findPaths((getFolderStringName(fType) + PATH_SEPERATOR +contextName).c_str());
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
                ConstString originalpath=curPath + PATH_SEPERATOR + name;
                YARP_stat(originalpath.c_str(), &statbuf);
                if ((statbuf.st_mode & S_IFMT)== S_IFDIR)
                {
                    ConstString destDirname=rf.getDataHome() + PATH_SEPERATOR + getFolderStringName(fType) + PATH_SEPERATOR + name;
                    recursiveCopy(originalpath, destDirname);
                    ConstString hiddenDirname=rf.getDataHome() + PATH_SEPERATOR + getFolderStringNameHidden(fType) + PATH_SEPERATOR + name;
                    recursiveCopy(originalpath, hiddenDirname);// TODO: check result!
                }
            }
            free(namelist[i]);
        }
        free(namelist);
    }

    printf("New user %s:\n", fType==CONTEXTS ? "contexts" : "robots");
    printContentDirs(rf.getDataHome() + PATH_SEPERATOR + getFolderStringName(fType));
    return 0;
}

int remove(yarp::os::ConstString contextName, folderType fType, bool verbose)
{
    yarp::os::ResourceFinder rf;
    rf.setVerbose(verbose);
    ResourceFinderOptions opts;
    opts.searchLocations=ResourceFinderOptions::User;
    ConstString targetPath=rf.findPath((getFolderStringName(fType) + PATH_SEPERATOR +contextName).c_str(), opts);
    if (targetPath=="")
    {
        printf("Could not find %s %s !\n",fType==CONTEXTS ? "context" : "robot", contextName.c_str());
        return 0;
    }
    else
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
            ConstString hiddenPath=   rf.findPath((getFolderStringNameHidden(fType) + PATH_SEPERATOR +contextName).c_str(), opts);
            if (hiddenPath != "")
                recursiveRemove(hiddenPath.c_str());
            return result;
        }
        else
        {
            printf("Skipped\n");
            return 0;
        }
    }
}

int diff(yarp::os::ConstString contextName, folderType fType, bool verbose)
{
    yarp::os::ResourceFinder rf;
    rf.setVerbose(verbose);

    ResourceFinderOptions opts;
    opts.searchLocations=ResourceFinderOptions::User;
    ConstString userPath=rf.findPath((getFolderStringName(fType) + PATH_SEPERATOR +contextName).c_str(), opts);

    opts.searchLocations=ResourceFinderOptions::Installed;
    ConstString installedPath=rf.findPath((getFolderStringName(fType) + PATH_SEPERATOR +contextName).c_str(), opts);

    recursiveDiff(installedPath, userPath);
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
        std::vector<std::string> subDirs=listContentDirs(installedPath);
        for (std::vector<std::string>::iterator subDirIt= subDirs.begin(); subDirIt!=subDirs.end(); ++subDirIt)
        {
            ostream tmp(0);
            opts.searchLocations=ResourceFinderOptions::User;
            rf.setQuiet();
            ConstString userPath=rf.findPath((getFolderStringName(fType) + PATH_SEPERATOR +(*subDirIt)).c_str(), opts);
            if (userPath == "")
                continue;
            if ( recursiveDiff(installedPath + PATH_SEPERATOR + (*subDirIt), userPath, tmp)>0)
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
                ConstString userFileName=rf.findPath((getFolderStringName(fType) + PATH_SEPERATOR +contextName + PATH_SEPERATOR + fileName).c_str(), opts);

                ConstString hiddenFileName=rf.findPath((getFolderStringNameHidden(fType) + PATH_SEPERATOR +contextName+ PATH_SEPERATOR + fileName).c_str(), opts);

                opts.searchLocations=ResourceFinderOptions::Installed;
                ConstString installedFileName=rf.findPath((getFolderStringName(fType) + PATH_SEPERATOR +contextName+ PATH_SEPERATOR + fileName).c_str(), opts);

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
        ConstString userPath=rf.findPath((getFolderStringName(fType) + PATH_SEPERATOR +contextName).c_str(), opts);

        ConstString hiddenUserPath=rf.findPath((getFolderStringNameHidden(fType) + PATH_SEPERATOR +contextName).c_str(), opts);

        opts.searchLocations=ResourceFinderOptions::Installed;
        ConstString installedPath=rf.findPath((getFolderStringName(fType) + PATH_SEPERATOR +contextName).c_str(), opts);

        recursiveMerge(installedPath, userPath, hiddenUserPath);
    }
    return 0;
}
