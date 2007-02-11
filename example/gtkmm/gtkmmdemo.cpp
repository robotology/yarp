/*
    Example glade application. By Giorgio.
    NOTE: The application doesn't shut down properly.
 */

// just removes the pervading warning that plagues compilation of the gtkmm on Visual Studio
#pragma warning(disable: 4250)

// real code here!
#include <libglademm.h>
#include <gtkmm.h>

// the very simplest glade application.
int main(int argc, char *argv[])
{
    Gtk::Main kit(argc, argv);
    Glib::RefPtr<Gnome::Glade::Xml> refXml = Gnome::Glade::Xml::create("basic.glade");
    kit.run();

    return 0;
}