#include "mainwindow.h"
#include <QApplication>
#include <yarp/os/YarprunLogger.h>
#include <cstdio>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    yarp::os::Network yarp;
    if (!yarp.checkNetwork())
    {
        fprintf(stderr,"ERROR: check Yarp network.\n");
        return -1;
    }

    yarp::os::ResourceFinder rf;
    rf.setVerbose(true);
    rf.setDefaultConfigFile("yarprunLogger.ini");           //overridden by --from parameter
    rf.setDefaultContext("yarprunLogger");                  //overridden by --context parameter
    rf.configure(argc,argv);

    MainWindow w(rf);
    w.show();

    return a.exec();
}
