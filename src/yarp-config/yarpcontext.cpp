/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Property.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/ResourceFinderOptions.h>
#include <yarp/os/Bottle.h>
#include <cstdio>


#include "yarpcontextutils.h"
#include "yarpcontext.h"

using namespace yarp::os;

void yarp_context_help() {
    printf("Usage: yarp-config context [OPTION]\n\n");
    printf("Known values for OPTION are:\n\n");
    printf("  --help       display this help and exit\n");
    printf("  --list  list contexts that are available; add optional '--user', '--sysadm' or '--installed' parameters to limit the search locations\n");
//    printf("  --show <context-name>  show files that make up a context, and the location of each\n");
    printf("  --import <context_name> import all the files of the specified context to home directory\n");
    printf("  --import <context_name> file1 file2 ... import specified context files to home directory\n");
    printf("  --import-all import all contexts to home directory\n");
    printf("  --remove  <context_name> file1 file2 ... remove specified context files from home directory\n");
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
#ifndef YARP_NO_DEPRECATED // Since YARP 3.4
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
        if (options.check("verbose")) {
            rf.setVerbose(true);
        }
YARP_WARNING_POP
#endif // YARP_NO_DEPRECATED
        if(options.check("user") || options.check("sysadm") || options.check("installed"))
        {
            if (options.check("user")) {
                printUserFolders(rf, CONTEXTS);
            }
            if (options.check("sysadm")) {
                printSysadmFolders(rf, CONTEXTS);
            }
            if (options.check("installed")) {
                printInstalledFolders(rf, CONTEXTS);
            }
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
#ifndef YARP_NO_DEPRECATED // Since YARP 3.4
        return import(importArg, CONTEXTS, options.check("verbose"));
#else
        return import(importArg, CONTEXTS);
#endif
    }

    if(options.check("import-all"))
    {
#ifndef YARP_NO_DEPRECATED // Since YARP 3.4
        return importAll(CONTEXTS, options.check("verbose"));
#else
        return importAll(CONTEXTS);
#endif
    }

    if(options.check("remove"))
    {
        Bottle removeArg=options.findGroup("remove");
#ifndef YARP_NO_DEPRECATED // Since YARP 3.4
        return remove(removeArg, CONTEXTS, options.check("verbose"));
#else
        return remove(removeArg, CONTEXTS);
#endif

    }

    if(options.check("where"))
    {
        std::string contextName=options.find("where").asString();
        if (contextName=="")
        {
            printf("No context name provided\n");
            return 0;
        }
        ResourceFinderOptions opts;
        if (options.check("user") || options.check("sysadm") || options.check("installed"))
        {
            opts.searchLocations=ResourceFinderOptions::NoLocation;
            if (options.check("user")) {
                opts.searchLocations = ResourceFinderOptions::SearchLocations(opts.searchLocations | ResourceFinderOptions::User);
            }
            if (options.check("sysadm")) {
                opts.searchLocations = ResourceFinderOptions::SearchLocations(opts.searchLocations | ResourceFinderOptions::Sysadmin);
            }
            if (options.check("installed")) {
                opts.searchLocations = ResourceFinderOptions::SearchLocations(opts.searchLocations | ResourceFinderOptions::Installed);
            }
        }
        yarp::os::ResourceFinder rf;
#ifndef YARP_NO_DEPRECATED // Since YARP 3.4
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING
        if (options.check("verbose")) {
            rf.setVerbose(true);
        }
YARP_WARNING_POP
#endif // YARP_NO_DEPRECATED
        yarp::os::Bottle paths=rf.findPaths(std::string("contexts") + PATH_SEPARATOR +contextName, opts);
        for (size_t curCont = 0; curCont < paths.size(); ++curCont) {
            printf("%s\n", paths.get(curCont).asString().c_str());
        }
        return 0;
    }

//         if(options.check("show"))
//     {
//         std::string contextName=options.find("show").asString().c_str();
//         if (contextName=="")
//         {
//             printf("No context name provided\n");
//             return 0;
//         }
//         yarp::os::ResourceFinder rf;
//         if (options.check("verbose"))
//             rf.setVerbose(true);
//         yarp::os::Bottle paths=rf.findPaths((std::string("contexts") + PATH_SEPARATOR +contextName).c_str());
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
        std::string contextName=options.find("diff").asString();
        if (contextName=="")
        {
            printf("No context name provided\n");
            return 0;
        }
#ifndef YARP_NO_DEPRECATED // Since YARP 3.4
        return diff(contextName, CONTEXTS, options.check("verbose"));
#else
        return diff(contextName, CONTEXTS);
#endif
    }
    if(options.check("diff-list"))
    {
#ifndef YARP_NO_DEPRECATED // Since YARP 3.4
        return diffList(CONTEXTS, options.check("verbose"));
#else
        return diffList(CONTEXTS);
#endif
    }
    if(options.check("merge"))
    {
        Bottle mergeArg=options.findGroup("merge");
#ifndef YARP_NO_DEPRECATED // Since YARP 3.4
        return merge(mergeArg, CONTEXTS, options.check("verbose"));
#else
        return merge(mergeArg, CONTEXTS);
#endif
    }
    yarp_context_help();
    return 1;
}
