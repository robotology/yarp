/*
    Example glade application. By Giorgio.
    NOTE: The application doesn't shut down properly.
 */

// just removes the pervading warning that plagues compilation of the gtkmm on Visual Studio
#pragma warning(disable: 4250)

// real code here!
#include <iostream>
#include <stdexcept>

#include <libglademm.h>
#include <gtkmm.h>

// a class for managing the main widget (approach copied from 
// http://www.pebble.org.uk/programming/libglademm_simple).
//
class MainWindow {
public:
    explicit MainWindow(Glib::RefPtr<Gnome::Glade::Xml> refXml);
    ~MainWindow();

    Gtk::Window& getWindow() const { return *mainWindow; }

protected:
    Gtk::Window *mainWindow;
};

MainWindow::MainWindow(Glib::RefPtr<Gnome::Glade::Xml> refXml) : mainWindow(0) {
    refXml->get_widget("window_main", mainWindow);
    if (!mainWindow)
        throw std::runtime_error("Couldn't find window_main");
}

MainWindow::~MainWindow() {
    //delete mainWindow; memory is managed by gtkmm.
}

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