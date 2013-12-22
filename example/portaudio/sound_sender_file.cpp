/*
 * Copyright: (C) 2013
 * Authors: Marco Randazzo, Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <stdio.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/AudioGrabberInterfaces.h>
#include <yarp/os/Property.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>

#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/sig/Sound.h>
#include <yarp/sig/SoundFile.h>
#include <string.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

#define USE_PORTS 1

int main(int argc, char *argv[])
{
    //open the network
    Network yarp;

    //open the output port
#ifdef USE_PORTS
    Port p;                //use port
#else
    BufferedPort<Sound> p; // use buffered port 
#endif

    p.open("/sender");

    //get the filename
    char filename [255];
    if (argc==3)
    {
        if (strcmp(argv[1],"--file")==0)
            strcpy (filename, argv[2]);
        else
        {
            printf ("usage: sound_sender_file --file <filename.wav>\n");
            return -1;
        }
    }
    else
    {
        printf ("usage: sound_sender_file --file <filename.wav>\n");
        return -1;
    }

    //read sound from file and put data in yarp::sig::Sound
    Sound s;
    printf("opening file %s\n",filename);
    if (yarp::sig::file::read(s,filename)==false)
    {
        printf("cannot open file\n");
        return -1;
    }

    yarp::os::Network::connect("/sender","/yarphear");
    //yarp::os::Time::delay (0.1);

    //send data on the network
    while (true)
    {
        printf("sending data...");
#ifdef  USE_PORTS
        p.write(s); //use ports
#else
        Sound &r = p.prepare(); //use buffered ports
        r=s;
        p.write(false);
#endif
         printf("done.\n");

        printf("Press enter to repeat, CTRL+C to close\n");
        while ((getchar()) != '\n');
    }

    return 0;
}

