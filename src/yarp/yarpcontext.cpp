// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 Istituto Italiano di Tecnologia
 * Authors: Elena Ceseracciu
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <yarp/os/Property.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/ResourceFinderOptions.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Os.h>
#include <yarp/os/impl/PlatformStdlib.h>
#include <stdio.h>
#include "ace/OS_NS_fcntl.h"
#include <errno.h>

#if defined(WIN32)
    #define PATH_SEPERATOR      "\\"
#else
    #define PATH_SEPERATOR      "/"
#endif

using namespace yarp::os;

int recursiveCopy(ConstString srcDirName, ConstString destDirName)
{
    bool ok=true;
    struct YARP_DIRENT **namelist;
    int n = YARP_scandir(srcDirName.c_str(),&namelist,NULL,YARP_alphasort);
    if (n<0)
    {
        printf("Could not read from  directory %s\n", srcDirName.c_str());
        return 1;
    }
    if (yarp::os::mkdir((destDirName).c_str()) < 0)
    {
        if (errno == EEXIST)
            printf("Directory %s already exist; remove it first, or use the diff/merge commands\n", destDirName.c_str());
        else
            printf("Could not create directory %s\n", destDirName.c_str());
        return 1;
    }
    for (int i=0; i<n; i++)
    {
            
        ConstString name = namelist[i]->d_name;
        
        if( name != "." && name != "..")
        {
            ConstString srcPath=srcDirName + PATH_SEPERATOR + name;
            ACE_stat statbuf;
            if( ACE_OS::stat(srcPath.c_str(), &statbuf) ==-1)
                printf("Error in checking properties for %s\n", srcPath.c_str());
            if ((statbuf.st_mode & S_IFMT)== S_IFDIR)
                recursiveCopy(srcPath, destDirName + PATH_SEPERATOR + name);
            if ((statbuf.st_mode & S_IFMT)== S_IFREG)
            {
                ACE_HANDLE fdSrc=ACE_OS::open(srcPath.c_str(),O_RDONLY);

                if ( fdSrc <0 )
                {
                   printf("BAD OPEN %s\n",srcPath.c_str());
                   // return 1;
                    ok = false;
                }
                else
                {
                    unsigned char c;
                    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
                    ACE_HANDLE fdDest=ACE_OS::creat  ((destDirName + PATH_SEPERATOR + name).c_str(), mode) ;
                    
                    if ( fdDest<0 )
                    {
                         printf("BAD OPEN %s\n",(destDirName + PATH_SEPERATOR + name).c_str());
                        
                        ACE_OS::close(fdSrc);
                        ok=false;
                        //return 1;
                    }
                    while (ACE_OS::read(fdSrc,&c,1)==1)
                    {
                        ACE_OS::write(fdDest,&c,1);
                    }
                    ACE_OS::close(fdSrc);
                    ACE_OS::close(fdDest);
                }
            }
        }
        free(namelist[i]);
    }
    free(namelist);
    
    return (ok==true ? 0 :-1);
}

int recursiveRemove(ConstString dirName)
{
    struct YARP_DIRENT **namelist;
    int n = YARP_scandir(dirName.c_str(),&namelist,NULL,YARP_alphasort);
    if (n<0)
    {
        printf("Could not read from  directory %s\n", dirName.c_str());
        return ACE_OS::rmdir(dirName.c_str()); // TODO check if this is useful...
    }
    
    for (int i=0; i<n; i++)
    {
        ConstString name = namelist[i]->d_name;
        ConstString path=dirName + PATH_SEPERATOR + name;
        if( name != "." && name != "..")
        {
            ACE_stat statbuf;
            if (ACE_OS::stat(path.c_str(), &statbuf) == -1)
                printf("Error in checking properties for %s\n", path.c_str());
            if ((statbuf.st_mode & S_IFMT)== S_IFDIR) //TODO: use stat() instead?
                recursiveRemove(path);
            if ((statbuf.st_mode & S_IFMT)== S_IFREG)
            {
               ACE_OS::unlink(path);
            }
        }
        free(namelist[i]);
    }
    free(namelist);
    
    return ACE_OS::rmdir(dirName.c_str());
    
};

void printContentDirs(ConstString curPath)
{
    struct YARP_DIRENT **namelist;
    int n = YARP_scandir(curPath.c_str(),&namelist,NULL,YARP_alphasort);
    if (n<0) {
        return;
    }
    for (int i=0; i<n; i++) {
        
        ConstString name = namelist[i]->d_name;
        if( name != "." && name != "..")
        {
            ACE_stat statbuf;
            ConstString path=curPath + PATH_SEPERATOR + name;
            if (ACE_OS::stat(path.c_str(), &statbuf) == -1)
                printf("Error in checking properties for %s\n", path.c_str());

            if ((statbuf.st_mode & S_IFMT)== S_IFDIR)
                printf("%s\n",name.c_str());
        }
        free(namelist[i]);
    }
    free(namelist);
};

void show_help() {
    printf("Usage: yarp-context [OPTION]\n\n");
    printf("Known values for OPTION are:\n\n");
    printf("  --help       display this help and exit\n");
    printf("  --list  list contexts that are available\n");
    printf("  --import <context_name>  import specified context to home directory\n");
    printf("  --import-all import all contexts to home directory\n");
    printf("  --remove  <context_name>  remove specified context from home directory\n");
    printf("  --diff  <context_name>  find differences from the context in the home directory with respect to the installation directory\n");
    printf("  --diff-list  list the contexts in the home directory that are different from the installation directory\n");
    printf("  --where  <context_name>  print full paths to the contexts that are found for <context_name> (the first one is the default one)\n");
    printf("  --merge  <context_name>  merge differences\n");
    printf("\n");

}

int main(int argc, char *argv[]) {
    yarp::os::Property options;
    options.fromCommand(argc,argv);
    if (options.check("help")) {
        show_help();
        return 0;
    }
    if (options.check("list")) {
        yarp::os::ResourceFinder rf;
        if (options.check("verbose"))
            rf.setVerbose(true);
        
        ResourceFinderOptions opts;
        opts.searchLocations=ResourceFinderOptions::User;
        yarp::os::Bottle contextPaths=rf.findPaths("contexts", opts); // separate "User" from "Installed" ?
        printf("**LOCAL USER DATA:\n");
        for (int curPathId=0; curPathId<contextPaths.size(); ++curPathId)
        {
            printf("* Directory : %s\n", contextPaths.get(curPathId).asString().c_str());
            printContentDirs(contextPaths.get(curPathId).asString());
        }
        
        opts.searchLocations=ResourceFinderOptions::Installed;
        contextPaths=rf.findPaths("contexts", opts); // separate "User" from "Installed" ?
        printf("**INSTALLED DATA:\n");
        for (int curPathId=0; curPathId<contextPaths.size(); ++curPathId)
        {
            printf("* Directory : %s\n", contextPaths.get(curPathId).asString().c_str());
            printContentDirs(contextPaths.get(curPathId).asString());
        }
        
        return 0;
    }
    
    if(options.check("import"))
    {
        ConstString contextName=options.find("import").asString().c_str();
        if (contextName=="")
        {
            printf("No context name provided\n");
            return 0;
        }
        yarp::os::ResourceFinder rf;
        if (options.check("verbose"))
            rf.setVerbose(true);
        ResourceFinderOptions opts;
        opts.searchLocations=ResourceFinderOptions::Installed;
        ConstString originalpath=rf.findPath((ConstString("contexts") + PATH_SEPERATOR +contextName).c_str(), opts);
        ConstString destDirname=rf.getDataHome() + PATH_SEPERATOR + "contexts" + PATH_SEPERATOR + contextName;

		ACE_DIR* dir= ACE_OS::opendir((rf.getDataHome()).c_str()); //TODO: wrap in yarp
        if (dir!=NULL)
            //ACE_OS::closedir(dir);
            YARP_closedir(dir);
        else
        {
            yarp::os::mkdir((rf.getDataHome()).c_str());
        }


        dir= ACE_OS::opendir((rf.getDataHome() + PATH_SEPERATOR + "contexts").c_str()); //TODO: wrap in yarp
        if (dir!=NULL)
            //ACE_OS::closedir(dir);
            YARP_closedir(dir);
        else
        {
            yarp::os::mkdir((rf.getDataHome() + PATH_SEPERATOR + "contexts").c_str());
        }
        return recursiveCopy(originalpath, destDirname);// TODO: check result!
    }
    
    if(options.check("import-all"))
    {

        yarp::os::ResourceFinder rf;
        if (options.check("verbose"))
            rf.setVerbose(true);
        ResourceFinderOptions opts;
        opts.searchLocations=ResourceFinderOptions::Installed;
        yarp::os::Bottle contextPaths=rf.findPaths("contexts", opts);
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
                    ACE_OS::stat(originalpath.c_str(), &statbuf);
                    if ((statbuf.st_mode & S_IFMT)== S_IFDIR)
                    {
                        ConstString destDirname=rf.getDataHome() + PATH_SEPERATOR + "contexts" + PATH_SEPERATOR + name;
                        recursiveCopy(originalpath, destDirname);// TODO: check result!
                    }
                }
                free(namelist[i]);
            }
            free(namelist);
        }
        return 0;
    }
    
    if(options.check("remove"))
    {
        
        ConstString contextName=options.find("remove").asString().c_str();
        if (contextName=="")
        {
            printf("No context name provided\n");
            return 0;
        }
        yarp::os::ResourceFinder rf;
        if (options.check("verbose"))
            rf.setVerbose(true);
        ResourceFinderOptions opts;
        opts.searchLocations=ResourceFinderOptions::User;
        ConstString targetPath=rf.findPath((ConstString("contexts") + PATH_SEPERATOR +contextName).c_str(), opts);
        if (targetPath=="")
        {
            printf("Could not find context %s !\n", contextName.c_str());
            return 0;
        }
        else
        {
            char choice='n';
            printf("Are you sure you want to remove this folder: %s ? (y/n): ", targetPath.c_str());
            scanf("%c",&choice);
            if (choice=='y')
                return recursiveRemove(targetPath.c_str());
            else
            {
                printf("Skipped\n");
                return 0;
            }
        }
    }
    
    if(options.check("where"))
    {
        ConstString contextName=options.find("where").asString().c_str();
        if (contextName=="")
        {
            printf("No context name provided\n");
            return 0;
        }
        yarp::os::ResourceFinder rf;
        if (options.check("verbose"))
            rf.setVerbose(true);
        yarp::os::Bottle paths=rf.findPaths((ConstString("contexts") + PATH_SEPERATOR +contextName).c_str());
        for (int curCont=0; curCont<paths.size(); ++curCont)
            printf("%s\n", paths.get(curCont).asString().c_str());
        return 0;
    }
    if(options.check("diff"))
    {
        printf("Not implemented yet\n");
        return 0;
    }
    if(options.check("diff-list"))
    {
        printf("Not implemented yet\n");
        return 0;
    }
    if(options.check("merge"))
    {
        printf("Not implemented yet\n");
        return 0;
    }
    show_help();
    return 1;
}

