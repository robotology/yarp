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
#include <stdio.h>

#include "yarpcontextutils.h"
#include "yarprobot.h"

using namespace yarp::os;

void yarp_robot_help() {
    printf("Usage: yarp-config robot [OPTION]\n\n");
    printf("Known values for OPTION are:\n\n");
    printf("  --help       display this help and exit\n");
    printf("  --list  list robots that are available; add optional '--user', '--sysadm' or '--installed' parameters to limit the search locations\n");
//    printf("  --show <context-name>  show files that make up a context, and the location of each\n");
    printf("  --import <robot_name> import specified robot files to home directory\n");
    printf("  --import-all-robots import all robots to home directory\n");
    printf("  --remove  <robot_name> remove specified robot files from home directory\n");
    printf("  --diff  <robot_name>  find differences from the robot in the home directory with respect to the installation directory\n");
    printf("  --diff-list  list the robots in the home directory that are different from the installation directory\n");
    //printf("  --where  <context_name>  print full paths to the contexts that are found for <context_name> (the first one is the default one)\n");
    printf("  --merge  <robot_name>  file1 file2 ... merge differences in selected files-directories\n");
    printf("\n");

}

int yarp_robot_main(int argc, char *argv[]) {
    yarp::os::Property options;
    options.fromCommand(argc,argv);
    if (options.check("help")) {
        yarp_robot_help();
        return 0;
    }
    if (options.check("list")) {
        yarp::os::ResourceFinder rf;
        if (options.check("verbose"))
            rf.setVerbose(true);
        if(options.check("user") || options.check("sysadm") || options.check("installed"))
        {
            if (options.check("user"))
                printUserFolders(rf, ROBOTS);
            if (options.check("sysadm"))
                printSysadmFolders(rf, ROBOTS);
            if (options.check("installed"))
                printInstalledFolders(rf, ROBOTS);
        }
        else
        {
            printUserFolders(rf, ROBOTS);
            printSysadmFolders(rf, ROBOTS);
            printInstalledFolders(rf, ROBOTS);
        }
        return 0;
    }

    if(options.check("import"))
    {
        Bottle importArg=options.findGroup("import");
        return import(importArg, ROBOTS, options.check("verbose"));
    }

    if(options.check("import-all-robots"))
    {

        return importAll(ROBOTS, options.check("verbose"));
    }

    if(options.check("remove"))
    {

        Bottle removeArg=options.findGroup("remove");
        return remove(removeArg, ROBOTS, options.check("verbose"));
    }

    if(options.check("where"))
    {
        ConstString contextName=options.find("where").asString().c_str();
        if (contextName=="")
        {
            printf("No robot name provided\n");
            return 0;
        }
        ResourceFinderOptions opts;
        if (options.check("user") || options.check("sysadm") || options.check("installed"))
        {
            opts.searchLocations=ResourceFinderOptions::NoLocation;
            if(options.check("user"))
                opts.searchLocations= ResourceFinderOptions::SearchLocations ( opts.searchLocations | ResourceFinderOptions::User);
            if(options.check("sysadm"))
                opts.searchLocations= ResourceFinderOptions::SearchLocations ( opts.searchLocations | ResourceFinderOptions::Sysadmin);
            if (options.check("installed"))
                opts.searchLocations= ResourceFinderOptions::SearchLocations ( opts.searchLocations | ResourceFinderOptions::Installed);
        }
        yarp::os::ResourceFinder rf;
        if (options.check("verbose"))
            rf.setVerbose(true);
        yarp::os::Bottle paths=rf.findPaths((ConstString("robots") + PATH_SEPARATOR +contextName).c_str(), opts);
        for (int curCont=0; curCont<paths.size(); ++curCont)
            printf("%s\n", paths.get(curCont).asString().c_str());
        return 0;
    }
    if(options.check("current"))
    {
        const char *result = yarp::os::getenv("YARP_ROBOT_NAME");
        if (result != NULL)
            printf("Current robot is %s, identified by the environment variable YARP_ROBOT_NAME\n", result);
        else
            printf("No robot is set; please set the YARP_ROBOT_NAME environment variable.\n");
        return 0;

    }

//         if(options.check("show"))
//     {
//         ConstString contextName=options.find("show").asString().c_str();
//         if (contextName=="")
//         {
//             printf("No context name provided\n");
//             return 0;
//         }
//         yarp::os::ResourceFinder rf;
//         if (options.check("verbose"))
//             rf.setVerbose(true);
//         yarp::os::Bottle paths=rf.findPaths((ConstString("contexts") + PATH_SEPARATOR +contextName).c_str());
//         std::vector<std::string> fileList;
//         for (int curCont=0; curCont<paths.size(); ++curCont)
//         {
//             std::vector<std::string> newList;
//             fileList.resize(fileList.size()+);
//         }
//             printf("%s\n", paths.get(curCont).asString().c_str());
//         return 0;
//     }

    if(options.check("diff"))
    {
        ConstString contextName=options.find("diff").asString().c_str();
        if (contextName=="")
        {
            printf("No robot name provided\n");
            return 0;
        }
        return diff(contextName, ROBOTS, options.check("verbose"));
    }
    if(options.check("diff-list"))
    {
        return diffList(ROBOTS, options.check("verbose"));
    }
    if(options.check("merge"))
    {
        Bottle mergeArg=options.findGroup("merge");
        return merge(mergeArg, ROBOTS, options.check("verbose"));
    }
    yarp_robot_help();
    return 1;
}

