/*
 * Copyright (C) 2008 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Network.h>
#include <yarp/os/impl/UnitTest.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/Os.h>
#include <yarp/os/impl/PlatformStdlib.h>
#include <yarp/os/YarpPlugin.h>

using namespace yarp::os;
using namespace yarp::os::impl;

#include <stdio.h>

class ResourceFinderTest : public UnitTest {
public:
    virtual ConstString getName() { return "ResourceFinderTest"; }

    void testBasics() {
        report(0,"testing the basics of RF...");
        ResourceFinder rf;

        const char *fname0 = "_yarp_regression_test.ini";
        const char *fname1 = "_yarp_regression_test_rf1.txt";
        const char *fname2 = "_yarp_regression_test_rf2.txt";

        // create some test files

        {
            FILE *fout = fopen(fname0,"w");
            yAssert(fout!=NULL);
            fprintf(fout,"style capability\n");
            fprintf(fout,"capability_directory \".\"\n");
            fprintf(fout,"default_capability \".\"\n");
            fclose(fout);
            fout = NULL;

            fout = fopen(fname1,"w");
            yAssert(fout!=NULL);
            fprintf(fout,"alt %s\n", fname2);
            fclose(fout);
            fout = NULL;

            fout = fopen(fname2,"w");
            fprintf(fout,"x 14\n");
            fclose(fout);
            fout = NULL;

            const char *argv[] = { "ignore",
                                   "--_yarp_regression_test", ".",
                                   "--from", fname1,
                                   "--verbose", "0",
                                   NULL };
            int argc = 7;

            rf.configure(argc,(char **)argv);
            ConstString alt = rf.findFile("alt");
            checkTrue(alt!="","found ini file");

            rf.setDefault("alt2",fname2);
            alt = rf.findFile("alt2");
            checkTrue(alt!="","default setting worked");

            rf.setDefault("alt3","_yarp_nonexistent.txt");
            alt = rf.findFile("alt3");
            checkTrue(alt=="","cannot find nonexistent files");

            rf.setDefault("alt","_yarp_nonexistent.txt");
            alt = rf.findFile("alt");
            checkTrue(alt!="","default setting is safe");

            checkTrue(rf.findPath()!="","existing path found");

            alt=rf.findFileByName(fname1);
            checkTrue(alt!="","found file by name");
        }
    }

    void testCommandLineArgs() {
        report(0,"make sure command line args take priority...");

        const char *fname0 = "_yarp_regression_test.ini";
        const char *fname1 = "_yarp_regression_test_rf1.txt";

        // create some test files

        {
            FILE *fout = fopen(fname0,"w");
            yAssert(fout!=NULL);
            fprintf(fout,"style capability\n");
            fprintf(fout,"capability_directory \".\"\n");
            fprintf(fout,"default_capability \".\"\n");
            fclose(fout);
            fout = NULL;

            fout = fopen(fname1,"w");
            fprintf(fout,"x 14\n");
            fclose(fout);
            fout = NULL;

            const char *argv[] = { "ignore",
                                   "--_yarp_regression_test", ".",
                                   "--from", fname1,
                                   "--verbose", "0",
                                   NULL };
            int argc = 7;

            ResourceFinder rf1;
            rf1.configure(argc,(char **)argv);
            checkEqual(rf1.find("x").asInt(),14,"found x");

            const char *argv2[] = { "ignore",
                                    "--_yarp_regression_test", ".",
                                    "--from", fname1,
                                    "--verbose", "0",
                                    "--x", "20",
                                    "--y", "30",
                                    NULL };
            int argc2 = 11;

            ResourceFinder rf2;
            rf2.configure(argc2,(char **)argv2);
            checkEqual(rf2.find("y").asInt(),30,"found y");
            checkEqual(rf2.find("x").asInt(),20,"override x");
        }

    }


    void testContext() {
        report(0,"test context setting");
        ResourceFinder rf;
        const char *argv[] = { "ignore",
                               "--_yarp_regression_test", ".",
                               "--context", "zig",
                               "--verbose", "0",
                               NULL };
        int argc = 7;
        rf.configure(argc,(char **)argv);
        checkEqual(rf.getContext().c_str(),"zig","recovered context");
    }

    void testSubGroup() {
        report(0,"test subgroup");
        const char *fname0 = "_yarp_regression_subgroup_test.ini";
        const char *fname1 = "_yarp_regression_subgroup_test_rf1.txt";
        const char *fname2 = "_yarp_regression_subgroup_test_rf2.txt";
        {
            FILE *fout = fopen(fname0,"w");
            yAssert(fout!=NULL);
            fprintf(fout,"[section1]\n");
            fprintf(fout,"fname \"_yarp_regression_subgroup_test_rf1.txt\"\n");
            fprintf(fout,"[section2]\n");
            fprintf(fout,"fname \"_yarp_regression_subgroup_test_rf2.txt\"\n");
            fclose(fout);
            fout = NULL;
        }
        {
            FILE *fout = fopen(fname1,"w");
            yAssert(fout!=NULL);
            fprintf(fout,"x 1\n");
            fclose(fout);
            fout = NULL;
        }
        {
            FILE *fout = fopen(fname2,"w");
            yAssert(fout!=NULL);
            fprintf(fout,"x 2\n");
            fclose(fout);
            fout = NULL;
        }
        ResourceFinder rf;
        const char *argv[] = { "ignore",
                               "--_yarp_regression_test", ".",
                               "--from", fname0,
                               "--verbose", "0",
                               NULL };
        int argc = 7;
        rf.configure(argc,(char **)argv);
        ResourceFinder rf1 = rf.findNestedResourceFinder("section1");
        //checkEqual(rf1.findFile("fname").c_str(),fname1,"section1 ok");
        checkFalse(rf1.isNull(),"section1 not null ok");
        ResourceFinder rf2 = rf.findNestedResourceFinder("section2");
        //checkEqual(rf2.findFile("fname").c_str(),fname2,"section2 ok");
        checkFalse(rf2.isNull(),"section2 not null ok");
        ResourceFinder rf3 = rf.findNestedResourceFinder("section3");
        checkTrue(rf3.isNull(),"section3 null ok");
    }

    void testDefaults() {
        report(0,"test default values of generic type");
        int defInt=42;
        double defDouble=42.42;
        ConstString defString="fortytwo";
        Bottle defList("(answers (42 24))");
        const char *argv[] = { NULL };
        int argc = 1;
        ResourceFinder rf;
        rf.setDefault("int", defInt);
        rf.setDefault("double", defDouble);
        rf.setDefault("string", defString);
        rf.setDefault("constchar", defString.c_str());
        rf.setDefault("list", defList.toString());
        rf.configure(argc,(char **)argv);
        checkEqual(rf.find("int").asInt(), defInt, "default integer set correctly");
        checkEqualish(rf.find("double").asDouble(), defDouble, "default double set correctly");
        checkEqual(rf.find("string").asString(), defString, "default string set correctly");
        checkEqual(rf.find("constchar").asString(), defString, "default string (passed as const char*) set correctly");
        Bottle *foundList=rf.find("list").asList();
        if(foundList!=NULL)
            checkEqual(rf.find("list").asList()->get(0).asString(), "answers", "default list set correctly");
        else
            report(1, "RF could not find default list");
    }

    void testGetDataHome() {
        report(0,"test getDataHome");
        saveEnvironment("YARP_DATA_HOME");
        saveEnvironment("XDG_DATA_HOME");
        saveEnvironment("HOME");
        Network::setEnvironment("YARP_DATA_HOME","/foo");
        checkEqual(ResourceFinder::getDataHome().c_str(),"/foo","YARP_DATA_HOME noticed");
        Network::unsetEnvironment("YARP_DATA_HOME");
        Network::setEnvironment("XDG_DATA_HOME","/foo");
        ConstString slash = Network::getDirectorySeparator();
        checkEqual(ResourceFinder::getDataHome().c_str(),
                   (ConstString("/foo") + slash + "yarp").c_str(),
                   "XDG_DATA_HOME noticed");
        Network::unsetEnvironment("XDG_DATA_HOME");
#ifdef __linux__
        Network::setEnvironment("HOME","/foo");
        checkEqual(ResourceFinder::getDataHome().c_str(),
                   "/foo/.local/share/yarp",
                   "HOME noticed");
#endif
        restoreEnvironment();
    }

    void testGetConfigHome() {
        report(0,"test getConfigHome");
        saveEnvironment("YARP_CONFIG_HOME");
        saveEnvironment("XDG_CONFIG_HOME");
        saveEnvironment("HOME");
        Network::setEnvironment("YARP_CONFIG_HOME","/foo");
        checkEqual(ResourceFinder::getConfigHome().c_str(),"/foo","YARP_CONFIG_HOME noticed");
        Network::unsetEnvironment("YARP_CONFIG_HOME");
        Network::setEnvironment("XDG_CONFIG_HOME","/foo");
        ConstString slash = Network::getDirectorySeparator();
        checkEqual(ResourceFinder::getConfigHome().c_str(),
                   (ConstString("/foo") + slash + "yarp").c_str(),
                   "XDG_CONFIG_HOME noticed");
        Network::unsetEnvironment("XDG_CONFIG_HOME");
#ifdef __linux__
        Network::setEnvironment("HOME","/foo");
        checkEqual(ResourceFinder::getConfigHome().c_str(),
                   "/foo/.config/yarp",
                   "HOME noticed");
#endif
        restoreEnvironment();
    }

    void testGetDataDirs() {
        report(0,"test getDataDirs");
        saveEnvironment("YARP_DATA_DIRS");
        saveEnvironment("XDG_DATA_DIRS");
        ConstString slash = Network::getDirectorySeparator();
        ConstString colon = Network::getPathSeparator();
        ConstString foobar = ConstString("/foo") + colon + "/bar";
        ConstString yfoo = ConstString("/foo") + slash + "yarp";
        ConstString ybar = ConstString("/bar") + slash + "yarp";
        Network::setEnvironment("YARP_DATA_DIRS",foobar);
        Bottle dirs;
        dirs = ResourceFinder::getDataDirs();
        checkEqual(dirs.size(),2,"YARP_DATA_DIRS parsed as two directories");
        checkEqual(dirs.get(0).asString().c_str(),"/foo","YARP_DATA_DIRS first dir ok");
        checkEqual(dirs.get(1).asString().c_str(),"/bar","YARP_DATA_DIRS second dir ok");

        Network::setEnvironment("YARP_DATA_DIRS","/foo");
        dirs = ResourceFinder::getDataDirs();
        checkEqual(dirs.size(),1,"YARP_DATA_DIRS parsed as one directory");

        Network::unsetEnvironment("YARP_DATA_DIRS");
        Network::setEnvironment("XDG_DATA_DIRS",foobar);
        dirs = ResourceFinder::getDataDirs();
        checkEqual(dirs.size(),2,"XDG_DATA_DIRS gives two directories");
        checkEqual(dirs.get(0).asString().c_str(),yfoo.c_str(),"XDG_DATA_DIRS first dir ok");
        checkEqual(dirs.get(1).asString().c_str(),ybar.c_str(),"XDG_DATA_DIRS second dir ok");

        Network::unsetEnvironment("XDG_DATA_DIRS");
#ifdef __linux__
        dirs = ResourceFinder::getDataDirs();
        checkEqual(dirs.size(),2,"DATA_DIRS default length 2");
        checkEqual(dirs.get(0).asString().c_str(),"/usr/local/share/yarp","DATA_DIRS default element 0 is ok");
        checkEqual(dirs.get(1).asString().c_str(),"/usr/share/yarp","DATA_DIRS default element 1 is ok");
#endif

        restoreEnvironment();
    }

    void testGetConfigDirs() {
        report(0,"test getConfigDirs");
        saveEnvironment("YARP_CONFIG_DIRS");
        saveEnvironment("XDG_CONFIG_DIRS");
        ConstString slash = Network::getDirectorySeparator();
        ConstString colon = Network::getPathSeparator();
        ConstString foobar = ConstString("/foo") + colon + "/bar";
        ConstString yfoo = ConstString("/foo") + slash + "yarp";
        ConstString ybar = ConstString("/bar") + slash + "yarp";
        Network::setEnvironment("YARP_CONFIG_DIRS",foobar);
        Bottle dirs;
        dirs = ResourceFinder::getConfigDirs();
        checkEqual(dirs.size(),2,"YARP_CONFIG_DIRS parsed as two directories");
        checkEqual(dirs.get(0).asString().c_str(),"/foo","YARP_CONFIG_DIRS first dir ok");
        checkEqual(dirs.get(1).asString().c_str(),"/bar","YARP_CONFIG_DIRS second dir ok");

        Network::unsetEnvironment("YARP_CONFIG_DIRS");
        Network::setEnvironment("XDG_CONFIG_DIRS",foobar);
        dirs = ResourceFinder::getConfigDirs();
        checkEqual(dirs.size(),2,"XDG_CONFIG_DIRS gives two directories");
        checkEqual(dirs.get(0).asString().c_str(),yfoo.c_str(),"XDG_CONFIG_DIRS first dir ok");
        checkEqual(dirs.get(1).asString().c_str(),ybar.c_str(),"XDG_CONFIG_DIRS second dir ok");

        Network::unsetEnvironment("XDG_CONFIG_DIRS");
#ifdef __linux__
        dirs = ResourceFinder::getConfigDirs();
        checkEqual(dirs.size(),1,"CONFIG_DIRS default length 1");
        checkEqual(dirs.get(0).asString().c_str(),"/etc/yarp","CONFIG_DIRS default is ok");
#endif

        restoreEnvironment();
    }


    void mkdir(const ConstString& dirname) {
        ACE_stat sb;
        if (ACE_OS::stat(dirname.c_str(),&sb)<0) {
            yarp::os::mkdir(dirname.c_str());
        }
        int r = ACE_OS::stat(dirname.c_str(),&sb);
        if (r<0) {
            // show problem
            checkTrue(r>=0,"test directory present");
        }
    }

    ConstString pathify(const Bottle& dirs) {
        char buf[1000];
        char *result = getcwd(buf,sizeof(buf));
        if (!result) {
            checkTrue(result!=NULL,"cwd/pwd not too long");
            yarp::os::exit(1);
        }
        ConstString slash = Network::getDirectorySeparator();
        ConstString dir = buf;
        for (int i=0; i<dirs.size(); i++) {
            dir += slash;
            dir = dir + dirs.get(i).asString();
        }
        return dir;
    }

    void mkdir(const Bottle& dirs) {
        ConstString slash = Network::getDirectorySeparator();
        ConstString dir = "";
        for (int i=0; i<dirs.size(); i++) {
            if (i>0) dir += slash;
            dir = dir + dirs.get(i).asString();
            mkdir(dir);
        }
    }

    void setUpTestArea(bool etc_pathd) {
        ConstString colon = Network::getPathSeparator();
        ConstString slash = Network::getDirectorySeparator();
        FILE *fout;

        ConstString base = etc_pathd ? "__test_dir_rf_a1" : "__test_dir_rf_a2";
        Bottle yarp_data_home;
        yarp_data_home.addString(base);
        yarp_data_home.addString("home");
        yarp_data_home.addString("yarper");
        yarp_data_home.addString(".local");
        yarp_data_home.addString("share");
        yarp_data_home.addString("yarp");
        mkdir(yarp_data_home);

        Bottle yarp_data_home_shadow;
        yarp_data_home_shadow.addString(base);
        yarp_data_home_shadow.addString("home");
        yarp_data_home_shadow.addString("yarper");
        yarp_data_home_shadow.addString(".local");
        yarp_data_home_shadow.addString("share");
        yarp_data_home_shadow.addString("yarp");
        yarp_data_home_shadow.addString("contexts");
        yarp_data_home_shadow.addString("shadowtest");
        mkdir(yarp_data_home_shadow);

        Bottle yarp_config_home;
        yarp_config_home.addString(base);
        yarp_config_home.addString("home");
        yarp_config_home.addString("yarper");
        yarp_config_home.addString(".config");
        yarp_config_home.addString("yarp");
        mkdir(yarp_config_home);

        Bottle yarp_config_home_plugins;
        yarp_config_home_plugins.addString(base);
        yarp_config_home_plugins.addString("home");
        yarp_config_home_plugins.addString("yarper");
        yarp_config_home_plugins.addString(".config");
        yarp_config_home_plugins.addString("yarp");
        yarp_config_home_plugins.addString("plugins");
        mkdir(yarp_config_home_plugins);

        Bottle yarp_data_dir0;
        yarp_data_dir0.addString(base);
        yarp_data_dir0.addString("usr");
        yarp_data_dir0.addString("share");
        yarp_data_dir0.addString("yarp");
        mkdir(yarp_data_dir0);

        Bottle yarp_data_dir0_plugins;
        yarp_data_dir0_plugins.addString(base);
        yarp_data_dir0_plugins.addString("usr");
        yarp_data_dir0_plugins.addString("share");
        yarp_data_dir0_plugins.addString("yarp");
        yarp_data_dir0_plugins.addString("plugins");
        mkdir(yarp_data_dir0_plugins);

        Bottle yarp_context_dir;
        yarp_context_dir.addString(base);
        yarp_context_dir.addString("usr");
        yarp_context_dir.addString("share");
        yarp_context_dir.addString("yarp");
        yarp_context_dir.addString("contexts");
        yarp_context_dir.addString("my_app");
        mkdir(yarp_context_dir);

        Bottle yarp_context_dir2;
        yarp_context_dir2.addString(base);
        yarp_context_dir2.addString("usr");
        yarp_context_dir2.addString("share");
        yarp_context_dir2.addString("yarp");
        yarp_context_dir2.addString("contexts");
        yarp_context_dir2.addString("shadowtest");
        mkdir(yarp_context_dir2);

        Bottle yarp_data_dir1;
        yarp_data_dir1.addString(base);
        yarp_data_dir1.addString("usr");
        yarp_data_dir1.addString("local");
        yarp_data_dir1.addString("share");
        yarp_data_dir1.addString("yarp");
        // do not make this

        Bottle yarp_config_dir0;
        yarp_config_dir0.addString(base);
        yarp_config_dir0.addString("etc");
        yarp_config_dir0.addString("yarp");
        mkdir(yarp_config_dir0);

        Bottle pathd;
        if (etc_pathd) {
            pathd = yarp_config_dir0;
        } else {
            pathd = yarp_data_dir0;
        }
        pathd.addString("config");
        pathd.addString("path.d");
        mkdir(pathd);

        Bottle project1;
        project1.addString(base);
        project1.addString("usr");
        project1.addString("share");
        project1.addString("project1");
        mkdir(project1);

        Bottle project2;
        project2.addString(base);
        project2.addString("usr");
        project2.addString("share");
        project2.addString("project2");
        mkdir(project2);

        Bottle path_project1;
        path_project1.addString("path");
        path_project1.addString(pathify(project1));

        Bottle path_project2;
        path_project2.addString("path");
        path_project2.addString(pathify(project2));

        fout = fopen((pathify(pathd)+slash+"project1.ini").c_str(),"w");
        yAssert(fout!=NULL);
        fprintf(fout,"[search project1]\n");
        fprintf(fout,"%s\n", path_project1.toString().c_str());
        fclose(fout);
        fout = NULL;

        fout = fopen((pathify(pathd)+slash+"project2.ini").c_str(),"w");
        yAssert(fout!=NULL);
        fprintf(fout,"[search project2]\n");
        fprintf(fout,"%s\n", path_project2.toString().c_str());
        fclose(fout);
        fout = NULL;

        saveEnvironment("YARP_DATA_HOME");
        saveEnvironment("YARP_CONFIG_HOME");
        saveEnvironment("YARP_DATA_DIRS");
        saveEnvironment("YARP_CONFIG_DIRS");
        saveEnvironment("YARP_ROBOT_NAME");

        Network::setEnvironment("YARP_DATA_HOME",pathify(yarp_data_home));
        Network::setEnvironment("YARP_CONFIG_HOME",pathify(yarp_config_home));
        Network::setEnvironment("YARP_DATA_DIRS",
                                pathify(yarp_data_dir0) +
                                colon +
                                pathify(yarp_data_dir1));
        Network::setEnvironment("YARP_CONFIG_DIRS",pathify(yarp_config_dir0));
        Network::setEnvironment("YARP_ROBOT_NAME", "dummyRobot");


        fout = fopen((pathify(yarp_data_home)+slash+"data.ini").c_str(),"w");
        yAssert(fout!=NULL);
        fprintf(fout,"magic_number = 42\n");
        fprintf(fout,"[data_home]\n");
        fprintf(fout,"x = 2\n");
        fclose(fout);
        fout = NULL;

        fout = fopen((pathify(yarp_data_dir0)+slash+"data.ini").c_str(),"w");
        yAssert(fout!=NULL);
        fprintf(fout,"magic_number = 22\n");
        fprintf(fout,"[data_dir0]\n");
        fprintf(fout,"x = 3\n");
        fclose(fout);
        fout = NULL;

        fout = fopen((pathify(project1)+slash+"data.ini").c_str(),"w");
        yAssert(fout!=NULL);
        fprintf(fout,"magic_number = 101\n");
        fprintf(fout,"[project1]\n");
        fprintf(fout,"x = 3\n");
        fclose(fout);
        fout = NULL;

        fout = fopen((pathify(yarp_context_dir)+slash+"my_app.ini").c_str(),"w");
        yAssert(fout!=NULL);
        fprintf(fout,"magic_number = 1000\n");
        fclose(fout);
        fout = NULL;

        fout = fopen((pathify(yarp_context_dir2)+slash+"shadow.ini").c_str(),"w");
        yAssert(fout!=NULL);
        fprintf(fout,"magic_number = 5000\n");
        fclose(fout);
        fout = NULL;

        fout = fopen((pathify(yarp_data_home_shadow)+slash+"shadow.ini").c_str(),"w");
        yAssert(fout!=NULL);
        fprintf(fout,"magic_number = 5001\n");
        fclose(fout);
        fout = NULL;

        fout = fopen((pathify(yarp_context_dir2)+slash+"noshadow.ini").c_str(),"w");
        yAssert(fout!=NULL);
        fprintf(fout,"magic_number = 5002\n");
        fclose(fout);
        fout = NULL;

        fout = fopen((pathify(yarp_config_home_plugins)+slash+"fakedev1.ini").c_str(),"w");
        yAssert(fout!=NULL);
        fprintf(fout,"[plugin fakedev1]\n");
        fprintf(fout,"type device\n");
        fprintf(fout,"name fakedev1\n");
        fprintf(fout,"library yarp_fakedev1\n");
        fprintf(fout,"part fakedev1\n");
        fclose(fout);
        fout = NULL;

        fout = fopen((pathify(yarp_data_dir0_plugins)+slash+"fakedev2.ini").c_str(),"w");
        yAssert(fout!=NULL);
        fprintf(fout,"[plugin fakedev2]\n");
        fprintf(fout,"type device\n");
        fprintf(fout,"name fakedev2\n");
        fprintf(fout,"library yarp_fakedev2\n");
        fprintf(fout,"part fakedev2\n");
        fclose(fout);
        fout = NULL;
    }

    void breakDownTestArea() {
        restoreEnvironment();
    }

    void testReadConfig() {
        report(0,"test readConfig");

        for (int area=1; area<2; area++) {
            if (area==0) {
                // currently ruled out in spec, but may come back
                report(0,"test readConfig with pathd in /etc/...");
            } else {
                report(0,"test readConfig with pathd in /usr/share/yarp/...");
            }
            setUpTestArea(area==0);

            ResourceFinder rf;
            Property p;
            bool ok = rf.readConfig(p,"data.ini",
                                    ResourceFinderOptions::findFirstMatch());
            checkTrue(ok,"read a data.ini");
            checkEqual(p.find("magic_number").asInt(),42,"right version found");
            checkTrue(p.check("data_home"),"data_home found");
            checkFalse(p.check("data_dir0"),"data_dirs not found");
            checkFalse(p.check("project1"),"project1 not found");
            p.clear();
            //rf.setVerbose(true);
            rf.readConfig(p,"data.ini",
                          ResourceFinderOptions::findAllMatch());
            checkEqual(p.find("magic_number").asInt(),42,"right priority");
            checkTrue(p.check("data_home"),"data_home found");
            checkTrue(p.check("data_dir0"),"data_dirs found");
            checkTrue(p.check("project1"),"project1 found");

            breakDownTestArea();
        }
    }

    void testContextVer2() {
        report(0,"test context version 2");
        setUpTestArea(false);

        {
            ResourceFinder rf;
            rf.setDefaultContext("my_app");
            Property p;
            bool ok = rf.readConfig(p,"my_app.ini",
                                    ResourceFinderOptions::findFirstMatch());
            checkTrue(ok,"read a my_app.ini");
            checkEqual(p.find("magic_number").asInt(),1000,"right version found");
        }

        {
            ResourceFinder rf;
            rf.setDefaultContext("my_app");
            rf.setDefaultConfigFile("my_app.ini");
            rf.configure(0,NULL);
            checkEqual(rf.find("magic_number").asInt(),1000,"my_app.ini found as default config file");
        }

        {
            ResourceFinder rf;
            rf.setDefaultContext("shadowtest");
            rf.setDefaultConfigFile("shadow.ini");
            rf.configure(0,NULL);
            checkEqual(rf.find("magic_number").asInt(),5001,"shadow.ini found as correct location");
        }

        {
            ResourceFinder rf;
            rf.setDefaultContext("shadowtest");
            rf.setDefaultConfigFile("noshadow.ini");
            rf.configure(0,NULL);
            checkEqual(rf.find("magic_number").asInt(),5002,"noshadow.ini found as correct location");
        }

        {
            ResourceFinder rf;
            rf.setDefaultContext("shadowtest");
            rf.setDefaultConfigFile("noshadow.ini");
            rf.configure(0,NULL);
            checkEqual(rf.find("magic_number").asInt(),5002,"noshadow.ini found as correct location");
            Property p;
            ResourceFinderOptions opts;
            checkTrue(rf.readConfig(p,"shadow.ini",opts),"found shadow.ini");
            checkEqual(p.find("magic_number").asInt(),5001,"shadow.ini found as correct location");
        }

        breakDownTestArea();
    }

    void testCopy() {
        report(0,"test context version 2");
        ResourceFinder rf1;
        rf1.setDefault("testNumber", "fortytwo");
        rf1.configure(0, NULL);
        checkEqual(rf1.find("testNumber").asString(), "fortytwo", "Original RF finds the default value");
        ResourceFinder rf2(rf1);
        checkEqual(rf2.find("testNumber").asString(), "fortytwo", "Copied RF finds the default passed to the original one");
        ResourceFinder rf3;
        rf3=rf1;
        checkEqual(rf3.find("testNumber").asString(), "fortytwo", "Assigned RF finds the default passed to the original one");
    }


    void testGetHomeDirsForWriting()
    {
        ConstString slash = Network::getDirectorySeparator();
        report(0,"test get 'home' dirs for writing");
        setUpTestArea(false);

        {
            ResourceFinder rf;
            const char *fname1 = "_yarp_regression_test_rf1.txt";
            rf.setDefaultContext("my_app");
            rf.setDefaultConfigFile(fname1); // should be in pwd
            rf.configure(0,NULL);

            char buf[1000];
            char *result = getcwd(buf,sizeof(buf));
            checkEqual(rf.getHomeContextPath(),result,"cwd found as context directory for writing");
            checkEqual(rf.getHomeRobotPath(),result,"cwd found as robot directory for writing");
        }

        {
            ResourceFinder rf;
            rf.setDefaultContext("my_app");
            rf.setDefaultConfigFile("my_app.ini");
            rf.configure(0,NULL);

            bool found;
            ConstString robot = NetworkBase::getEnvironment("YARP_ROBOT_NAME",
                                                            &found);
            if (!found) robot = "default";
            checkEqual(rf.getHomeContextPath(),ResourceFinder::getDataHome() + slash + "contexts" + slash + "my_app","$YARP_DATA_HOME/contexts/my_app found as directory for writing");
            checkEqual(rf.getHomeRobotPath(),ResourceFinder::getDataHome() + slash + "robots" + slash + robot,"$YARP_DATA_HOME/robots/dummyRobot found as directory for writing");

        }
        breakDownTestArea();
    }


    void testFindPlugins() {
        report(0,"test get 'home' dirs for writing");
        setUpTestArea(false);
        YarpPluginSelector selector;
        selector.scan();
        Bottle lst = selector.getSelectedPlugins();
        checkTrue(lst.check("fakedev1"),"first device present");
        checkTrue(lst.check("fakedev2"),"second device present");
        checkFalse(lst.check("fakedev3"),"non-existent device absent");
        breakDownTestArea();
    }

    void testFailOnFrom() {
        report(0,"test fail behavior on --from / setDefaultConfigFile");
        setUpTestArea(false);

        {
            ResourceFinder rf;
            rf.setDefaultContext("my_app");
            rf.setDefaultConfigFile("my_app.ini");
            bool configures = rf.configure(0,NULL);
            checkTrue(configures,"ok with default file that exists");
        }

        {
            ResourceFinder rf;
            rf.setDefaultContext("my_app");
            const char *argv[] = { "ignore",
                                   "--from", "my_app.ini",
                                   NULL };
            int argc = 3;
            bool configures = rf.configure(argc,(char **)argv);
            checkTrue(configures,"ok with from file that exists");
        }

        {
            ResourceFinder rf;
            rf.setDefaultContext("my_app");
            rf.setDefaultConfigFile("my_app_not_there.ini");
            bool configures = rf.configure(0,NULL);
            checkTrue(configures,"ok with default file that does not exist");
        }

        {
            ResourceFinder rf;
            rf.setDefaultContext("my_app");
            const char *argv[] = { "ignore",
                                   "--from", "my_app_not_there.ini",
                                   NULL };
            int argc = 3;
            bool configures = rf.configure(argc,(char **)argv);
            checkFalse(configures,"fails with from file that is missing");
        }
    }

    virtual void runTests() {
        testBasics();
        testCommandLineArgs();
        testContext();
        testDefaults();
        testSubGroup();
        testGetDataHome();
        testGetConfigHome();
        testGetDataDirs();
        testGetConfigDirs();
        testReadConfig();
        testContextVer2();
        testCopy();
        testGetHomeDirsForWriting();
        testFindPlugins();
        testFailOnFrom();
    }
};

static ResourceFinderTest theResourceFinderTest;

UnitTest& getResourceFinderTest() {
    return theResourceFinderTest;
}

