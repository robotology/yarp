/*
  Example taken from GTKMM+ 2.4 Tutorial.
	http://www.gtkmm.org/

  Copyright information from tutorial:
    Copyright (c) 2002-2006 Murray Cumming
    License of the original is FDL.
 */

// just removes the pervading warning that plagues compilation of the gtkmm
#pragma warning(disable: 4250)

// real code here!
#include <gtkmm.h>

int main(int argc, char *argv[])
{
    Gtk::Main kit(argc, argv);
    Gtk::Window window;
    Gtk::Main::run(window);
    return 0;
}