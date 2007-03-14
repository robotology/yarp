// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Alessandro Scalzo
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <gtkmm.h>
#include "FrameGrabberGUIControl.h"

//#include <ace/config.h>
//#include <ace/OS.h>
#include <yarp/os/Property.h> 
#include <yarp/os/Network.h> 

int main(int argc, char *argv[])
{
    Gtk::Main kit(argc, argv);

	if (argc == 3)
	{
		FrameGrabberGUIControl window(argv[1],argv[2]);
		Gtk::Main::run(window);
	}
	else if (argc<3)
	{
		FrameGrabberGUIControl window("","");
		Gtk::Main::run(window);
	}
	else if (argc>3)
	{
		yarp::os::Property options;
        options.fromCommand(argc,argv);
    
		// switch to subsections if available
		yarp::os::Searchable *Network = &options.findGroup("NETWORK");
		yarp::os::Searchable *Window = &options.findGroup("WINDOW");
		//yarp::os::Searchable *Program = &options.findGroup("PROGRAM");

		if (Network->isNull()) { Network = &options; }
		if (Window->isNull()) { Window = &options; }
		//if (Program->isNull()) { Program = &options; }

		yarp::os::Value *val;
    
		char portName[256]="",outPortName[256]="";
		int posX=0,posY=0;
		int width=280,height=100;

		if (Network->check("PortName",val)||Network->check("local",val)) 
		{
			strcpy(portName, val->asString().c_str());
		}

		if (Network->check("OutPortName",val)||Network->check("remote",val)) 
		{
			strcpy(outPortName, val->asString().c_str());
		}

		if (Window->check("PosX",val)||Window->check("x",val)) posX = val->asInt();
		if (Window->check("PosY",val)||Window->check("y",val)) posY = val->asInt();
		if (Window->check("width",val)) width = val->asInt();
		if (Window->check("height",val)) height = val->asInt();

		//printf("using local=%s remote=%s x=%d y=%d\n",portName,outPortName,posX,posY);
		//fflush(stdout);

		FrameGrabberGUIControl window(portName,outPortName);
		window.set_size_request(width,height);
		window.move(posX,posY);
		Gtk::Main::run(window);
	}

    return 0;
}
