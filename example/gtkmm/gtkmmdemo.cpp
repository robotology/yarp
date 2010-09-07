
/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Giorgio Metta
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

/*
    Example glade application. By Giorgio.
    Originally based on:
       http://www.pebble.org.uk/programming/libglademm_simple
    Rewritten to support multithreading.
 */

#include <iostream>
#include <sstream>
#include <stdexcept>

#include <libglademm.h>
#include <gtkmm.h>
#include <sigc++/class_slot.h>
#include <sigc++/bind_return.h>

#include <yarp/os/all.h>

class MainWindow {
public:
    explicit MainWindow(Glib::RefPtr<Gnome::Glade::Xml> refXml);
    ~MainWindow();

    Gtk::Window& getWindow() const { return *mainWindow; }
    Glib::Dispatcher& getDispatcher() { return disp; }
    void onUpdateWidget ();

protected:
    Gtk::Window *mainWindow;
    Gtk::TextView *v;
    Glib::Dispatcher disp;
    sigc::connection connection;
    int counter;
};

MainWindow::MainWindow(Glib::RefPtr<Gnome::Glade::Xml> refXml) : mainWindow(0), v(0), disp(), counter(0) {
    refXml->get_widget("window_main", mainWindow);
    if (!mainWindow)
        throw std::runtime_error("Couldn't find window_main");

    refXml->get_widget("textview_main", v);
    if (!v)
        throw std::runtime_error("Couldn't find the text view widget as requested");

    connection = disp.connect(sigc::mem_fun(*this, &MainWindow::onUpdateWidget));
}

MainWindow::~MainWindow() {
    connection.disconnect();
    //delete mainWindow; memory is managed by gtkmm.
}

void MainWindow::onUpdateWidget () {
    std::ostringstream text;
    text << "hello = " << counter;
    if (v) {
        Glib::RefPtr<Gtk::TextBuffer> buffer = v->get_buffer();
        buffer->assign(Glib::ustring(text.str()));
        std::cout << "name " << text.str() << " set" << std::endl;
        text.seekp(0);
    }
    counter++;
}

void onButtonQuitClicked () {
    std::cout << "quit clicked!" << std::endl;
    exit(0);
    //throw std::exception("just exiting the application");
}


class MakeThingsMoreComplicated : public yarp::os::Thread {
public:
    Glib::Dispatcher *hello;

public:
    MakeThingsMoreComplicated() : hello(0) {}
    virtual ~MakeThingsMoreComplicated() {}

    virtual void run() {
        while (!isStopping()) {
            hello->emit();
            yarp::os::Time::delay(0.04);
        }
    }
};

// the very simplest glade application.
int main(int argc, char *argv[])
{
    Glib::thread_init();

    try {
        Gtk::Main kit(argc, argv);
        Glib::RefPtr<Gnome::Glade::Xml> refXml = Gnome::Glade::Xml::create("basic.glade");        
        MainWindow mainw(refXml);
        
        Gtk::Button* but = 0;
        refXml->get_widget("button_quit", but);
        if (!but)
            throw std::runtime_error("Couldn't find the quit button as requested");
        but->signal_clicked().connect(sigc::ptr_fun(onButtonQuitClicked));

        MakeThingsMoreComplicated mythread;
        mythread.hello = &mainw.getDispatcher();

        if (!mythread.start()) {
            throw std::runtime_error("Couldn't start my thread!");
        }

        kit.run(mainw.getWindow());

        mythread.stop();
        return 0;
    }
    catch(...) {
        return 1;
    }
}

