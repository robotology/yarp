// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Alessandro Scalzo
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <gtkmm.h>
#include "FrameGrabberGUIControl.h"

int main(int argc, char *argv[])
{
    Gtk::Main kit(argc, argv);

	if (argc == 3)
	{
		FrameGrabberGUIControl window(argv[1],argv[2]);
		Gtk::Main::run(window);
	}
	else
	{
		FrameGrabberGUIControl window("","");
		Gtk::Main::run(window);
	}

    return 0;
}
