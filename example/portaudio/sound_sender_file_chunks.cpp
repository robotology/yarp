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
    yarp::os::Time::turboBoost();

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
    yarp::sig::file::soundStreamReader soundReader;
    printf("opening file %s\n",filename);
    if (soundReader.open(filename)==false)
    {
        printf("cannot open file\n");
        return -1;
    }

    yarp::os::Network::connect("/sender","/yarphear");
    //yarp::os::Time::delay (0.1);

    //send data on the network
    int CHUNK_SIZE = 4096; //samples
    for (int i=0; i<3; i++)
    {

        printf("sending data...\n",i);

        bool complete=false;
        soundReader.rewind();

        do
        {
            int current_pos  = soundReader.getIndex(); 
            int read_samples = soundReader.readBlock(s,CHUNK_SIZE);
            if (read_samples<CHUNK_SIZE) complete=true;

            static double old = yarp::os::Time::now();
            printf("from sample %d to sample %d, time %.3f\n", current_pos, current_pos+read_samples, yarp::os::Time::now()-old); 
            old = yarp::os::Time::now();

#ifdef  USE_PORTS
            p.write(s); //use ports
#else
            Sound &r = p.prepare(); //use buffered ports
            r=s;
            p.write(false);
#endif
        }
        while (!complete);

        printf("loop %d/3 complete, press enter to continue\n", i+1);
        while ((getchar()) != '\n');
    }

    //close file
    printf("closing file...\n");
    soundReader.close();

    printf("finished!\n");
    return 0;
}

