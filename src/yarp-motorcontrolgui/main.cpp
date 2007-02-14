/*
    Motor control GUI. By Pasa.
 */

// real code here!
#include <iostream>
#include <stdexcept>

#include <libglademm.h>
#include <gtkmm.h>

#include "MainWindow.h"

void onButtonQuitClicked () {
    std::cout << "quit clicked!" << std::endl;
    exit(0);
    //throw std::exception("just exiting the application");
}

// the very simplest glade application.
int main(int argc, char *argv[])
{
    try {
        Gtk::Main kit(argc, argv);
        Glib::RefPtr<Gnome::Glade::Xml> refXml = Gnome::Glade::Xml::create("yarpmotorcontrol.glade");        
        MainWindow mainw(refXml);
        
        Gtk::Button* but = 0;
        refXml->get_widget("button_quit", but);
        if (!but)
            throw std::runtime_error("Couldn't find the quit button as requested");
        but->signal_clicked().connect(sigc::ptr_fun(onButtonQuitClicked));
        kit.run(mainw.getWindow());
        return 0;
    }
    catch(...) {
        return 1;
    }
}