//
// MainWindow class definition.
//
//#pragma warning(disable: 4250)

#include <iostream>
#include <stdexcept>

#include <libglademm.h>
#include <gtkmm.h>

/**
 * MainWindow, a helper class that wraps the access to the main window widget.
 */
class MainWindow {
public:
    explicit MainWindow(Glib::RefPtr<Gnome::Glade::Xml> refXml);
    ~MainWindow();

    Gtk::Window& getWindow() const { return *mainWindow; }

protected:
    Gtk::Window *mainWindow;
};