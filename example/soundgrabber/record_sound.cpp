/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Authors: Julio Gomes, Alexandre Bernardino
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


/***********************************************************************

  record_sound

  Example application for acquiring sound from the sound grabber server

  Data samples are written in text files "channel0_client.dat" and "channel1_client.dat"

  File format is compatible with matlab 'load' command. 
  Read and play sounds with matlab commands:
  >> load -ascii sound.dat
  >> wavplay(sound, 44100);

  Author: Julio Gomes, Alexandre Bernardino, VisLab, ISR-IST

  Contact: jgomes(a)isr.ist.utl.pt, alex(a)isr.ist.utl.pt

************************************************************************/

#include <yarp/os/all.h>

#include <yarp/sig/Sound.h>
#include <yarp/os/BufferedPort.h>

#include <iostream>

using namespace std;
using namespace yarp::os;
using namespace yarp::sig;

#undef main



int main (void) 
{
	//in order to record to a file
	FILE *outFile;
    
	int i,j;
	
	char OutputFileName[]="sound.dat";
    
	outFile = fopen (OutputFileName,"w");
    
	double val;
    
	//-----------------------------------------------------




    Network yarp;

	BufferedPort<Sound> port;
	port.setStrict();

    port.open("/sound_recorder");
    

	yarp::os::Network::connect("/sound_grabber","/sound_recorder","tcp");
	printf("connected port\n\n");


	while (true) {

		Sound *input = port.read();
		if (input!=NULL)
		{
			//divide-se por 32768 para normalizar        
			for (i=0;i<input->getSamples();i++)
			{
				for(j = 0; j < input->getChannels(); j++)
				{
				   val=(double)input->get(i,j)/32768;
				   fprintf(outFile, "%1.6f ", val);
				}
				fprintf(outFile, "\n");
			} 
		}
		else
			break;
	}

	//-------closing output files ------------------
    fclose(outFile);
	printf("\n\nrecording terminated\n");
	//-----------------------------------
	yarp::os::Network::disconnect("/sound_grabber","/sound_recorder");
	printf("\n disconnected port\n");
    return 1;
}
 


