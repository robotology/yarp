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
    char filename  [255];
    char filename2 [255];

    if (argc==3)
    {
        if (strcmp(argv[1],"--file")==0)
        {
            strcpy (filename,  argv[2]);
            strcpy (filename2, argv[2]); //using the same file, result will be a sund repeated twice
        }
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
    Sound a;
    printf("opening first file %s\n",filename);
    if (yarp::sig::file::read(a,filename)==false)
    {
        printf("cannot open file\n");
        return -1;
    }
    Sound b;
    printf("opening second file %s\n",filename2);
    if (yarp::sig::file::read(b,filename)==false)
    {
        printf("cannot open file\n");
        return -1;
    }

    //concatenate sound a with sound b
    Sound c=a;
    c+=b;
    printf ("sound a has %d samples\n",a.getSamples());
    printf ("sound b has %d samples\n",b.getSamples());
    printf ("sound c=a+b has %d samples\n",c.getSamples());

    yarp::os::Network::connect("/sender","/yarphear");
    //yarp::os::Time::delay (0.1);

    //send data on the network
    while (true)
    {
        printf("sending data...");
#ifdef  USE_PORTS
        p.write(c); //use ports
#else
        Sound &r = p.prepare(); //use buffered ports
        r=c;
        p.write(false);
#endif
         printf("done.\n");

        printf("Press enter to repeat, CTRL+C to close\n");
        while ((getchar()) != '\n');
    }

    return 0;
}

