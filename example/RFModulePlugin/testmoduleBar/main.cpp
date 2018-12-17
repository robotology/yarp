#include <module.h>
#include <yarp/os/Network.h>
int main(int argc, char** argv)
{
    yarp::os::Network yarp;
    moduleBar module;
    return module.runModule();
}