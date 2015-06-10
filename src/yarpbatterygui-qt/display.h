/*
* Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
* Author: Marco Randazzo
* Date: June 2015
* email:   marco.randazzo@iit.it
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

#ifndef DISPLAY_H
#define DISPLAY_H

#include <QMainWindow>
#include <QTimer>

#include <yarp/os/Os.h>
#include <yarp/os/Network.h>
#include <yarp/os/ResourceFinder.h>
#include <yarp/dev/IBattery.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/Drivers.h>

#include "ui_display.h"

class MainWindow :public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(yarp::os::ResourceFinder rf, yarp::dev::IBattery* ibat, QWidget *parent = 0);
    ~MainWindow();

    private slots:

    void updateMain();

public:
    yarp::os::Network                yarp;
    yarp::dev::IBattery*             ibat;
    yarp::dev::PolyDriver*           drv;

private:
    Ui_BatteryDlg                    *ui;
    QTimer*                          mainTimer;
};

/*
#include <gtkmm.h>
#include <string>
#define N_IMAGES 10

class GraphicsManager : public Gtk::Window
{
public:
  GraphicsManager(std::string pictures_path);
  virtual ~GraphicsManager();
  void update_graphics(double voltage, double current, double charge, bool connected);

protected:
  virtual void load_pixbufs();

  //signal handlers:
  virtual bool on_drawingarea_expose(GdkEventExpose *event);


  //Member widgets:
  Glib::RefPtr<Gdk::Pixbuf> m_refPixbuf;
  Glib::RefPtr<Gdk::Pixbuf> m_refPixbuf_Background;
  Glib::RefPtr<Gdk::Pixbuf> m_refPixbuf_Background2;
  Glib::RefPtr<Gdk::Pixbuf> m_refPixbuf_Numbers;
  Glib::RefPtr<Gdk::Pixbuf> m_refPixbuf_Blocks;
  Glib::RefPtr<Gdk::Pixbuf> m_refPixbuf_Connected;
  Glib::RefPtr<Gdk::Pixbuf> m_refPixbuf_Charge;
  Gtk::DrawingArea m_DrawingArea;

  guint m_back_width, m_back_height;
  gint m_frame_num;
  std::string pics_path;
};
*/

#endif
