
#include <yarp/os/Network.h>
#include <stdio.h>

#include "client.hpp"

using namespace yarp;
using namespace yarp::os;


int main(int argc, char *argv[])
{
    if (argc<2)
    {
        fprintf(stdout,"port name is missing\n");
        return -1;
    }

    Network yarp;
    if (!yarp.checkNetwork())
        return -2;

    CollatzClient client(argv[1]);

    client.start();

    // wait until a key is pressed
    char ch;
    scanf("%c",&ch);

    client.stop();

    return 0;
}


