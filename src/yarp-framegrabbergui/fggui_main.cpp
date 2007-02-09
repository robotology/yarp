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
