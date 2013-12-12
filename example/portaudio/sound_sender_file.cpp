/*
 * Copyright: (C) 2013
 * Authors: Marco Randazzo, Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <stdio.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/AudioGrabberInterfaces.h>
#include <yarp/os/Property.h>

#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/sig/Sound.h>
#include <yarp/sig/SoundFile.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

int main(int argc, char *argv[])
{
    //open the network
    Network yarp;

    //open the output port
    Port p;
    p.open("/sender");

    //get the filename
    char filename [255];
    if (argc==3)
    {
        if (strcmp(argv[1],"--file")==0)
            strcpy (filename, argv[2]);
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

    //send data on the network
    while (true)
    {
        p.write(s);
    }

    return 0;
}

