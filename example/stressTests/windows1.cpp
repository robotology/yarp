#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <iostream>
#include <string>

int main()
{
    yarp::os::Network yarp;

    yarp::os::Port port;
    port.open("/test");

    bool done=false;
    while(!done)
    {
        std::cout<<"Type quit to exit\n";
        std::string tmp;
        std::cin>>tmp;
        if (tmp=="quit")
            done=true;
    }
    
    std::cout<<"You typed exit.\n";
   
    std::cout<<"Calling Port::close()...";
    port.close();
    std::cout<<"done!\n";
}
