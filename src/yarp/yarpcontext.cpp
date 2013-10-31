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
#include <stdio.h>


#include "yarpcontextutils.h"
#include "yarpcontext.h"

using namespace yarp::os;

void yarp_context_help() {
    printf("Usage: yarp-config context [OPTION]\n\n");
    printf("Known values for OPTION are:\n\n");
    printf("  --help       display this help and exit\n");
    printf("  --list  list contexts that are available; add optional '--user', '--sysadm' or '--installed' parameters to limit the search locations\n");
//    printf("  --show <context-name>  show files that make up a context, and the location of each\n");
    printf("  --import <context_name> file1 file2 import specified context files to home directory\n");
    printf("  --import-all import all contexts to home directory\n");
    printf("  --remove  <context_name>  remove specified context from home directory\n");
    printf("  --diff  <context_name>  find differences from the context in the home directory with respect to the installation directory\n");
    printf("  --diff-list  list the contexts in the home directory that are different from the installation directory\n");
    printf("  --where  <context_name>  print full paths to the contexts that are found for <context_name> (the first one is the default one)\n");
    printf("  --merge  <context_name>  file1 file2 ... merge differences in selected files-directories\n");
    printf("\n");

}

int yarp_context_main(int argc, char *argv[]) {
    yarp::os::Property options;
    options.fromCommand(argc,argv);
    if (options.check("help")) {
        yarp_context_help();
        return 0;
    }
    if (options.check("list")) {
        yarp::os::ResourceFinder rf;
        if (options.check("verbose"))
            rf.setVerbose(true);
        if(options.check("user") || options.check("sysadm") || options.check("installed"))
        {
            if (options.check("user"))
                printUserFolders(rf, CONTEXTS);
            if (options.check("sysadm"))
                printSysadmFolders(rf, CONTEXTS);
            if (options.check("installed"))
                printInstalledFolders(rf, CONTEXTS);
        }
        else
        {
            printUserFolders(rf, CONTEXTS);
            printSysadmFolders(rf, CONTEXTS);
            printInstalledFolders(rf, CONTEXTS);
        }
        return 0;
    }

    if(options.check("import"))
    {
        Bottle importArg=options.findGroup("import");
        return import(importArg, CONTEXTS, options.check("verbose"));
    }

    if(options.check("import-all"))
    {
        return importAll(CONTEXTS, options.check("verbose"));
    }

    if(options.check("remove"))
    {

        ConstString contextName=options.find("remove").asString().c_str();
        if (contextName=="")
        {
            printf("No context name provided\n");
            return 0;
        }
        return remove(contextName, CONTEXTS, options.check("verbose"));
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
        yarp::os::Bottle paths=rf.findPaths((ConstString("contexts") + PATH_SEPARATOR +contextName).c_str());
        for (int curCont=0; curCont<paths.size(); ++curCont)
            printf("%s\n", paths.get(curCont).asString().c_str());
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
            printf("No context name provided\n");
            return 0;
        }
        return diff(contextName, CONTEXTS, options.check("verbose"));
    }
    if(options.check("diff-list"))
    {
        return diffList(CONTEXTS, options.check("verbose"));
    }
    if(options.check("merge"))
    {
        Bottle mergeArg=options.findGroup("merge");
        return merge(mergeArg, CONTEXTS, options.check("verbose"));
    }
    yarp_context_help();
    return 1;
}

