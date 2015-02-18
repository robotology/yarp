#include "include/mainwindow.h"
#include <QApplication>

#if defined(WIN32)
    #pragma warning (disable : 4099)
    #pragma warning (disable : 4250)
    #pragma warning (disable : 4520)
#endif

#include <iostream>
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Network.h>

#if defined(WIN32)
    #include <windows.h>
#else
    #include <errno.h>
    #include <sys/types.h>
    #include <signal.h>
#endif

using namespace std;
using namespace yarp::os;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Network yarp;
    if (!yarp.checkNetwork()){
        fprintf(stderr, "Sorry YARP network does not seem to be available, is the yarp server available?\n");
        return -1;
    }

    yarp::os::ResourceFinder rf;
    rf.setVerbose( true );
    rf.setDefaultConfigFile( "config.ini" );        //overridden by --from parameter
    rf.setDefaultContext( "yarpdataplayer" );        //overridden by --context parameter
    rf.configure( argc, argv );



    MainWindow w(rf);



    if (rf.check("hidden")){
        w.hide();
    }else{
        w.show();
    }

    return a.exec();
}
