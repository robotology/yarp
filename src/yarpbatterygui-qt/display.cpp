/*
* Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
* Author: Marco Randazzo
* Date: June 2015
* email:   marco.randazzo@iit.it
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

#include <math.h>
#include "display.h"
#include <yarp/os/Time.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>


#ifndef M_PI
#define M_PI 3.1415927
#endif

using namespace std;

MainWindow::~MainWindow()
{
    if (mainTimer)
    {
        delete mainTimer;
        mainTimer = 0;
    }
    if (ui)
    {
        delete ui;
        ui = 0;
    }
}

void MainWindow::updateMain()
{
    static double voltage = 0;
    static double current = 0;
    static double charge = 0;

    if (ibat)
    {
        //yDebug("received battery info");
        ibat->getBatteryVoltage(voltage);
        ibat->getBatteryCurrent(current);
        ibat->getBatteryCharge(charge);
        //graphics->update_graphics(voltage,current,charge,true);
    }
    else
    {
        yError("TIMEOUT: unable to receive data from battery manager ");
        //graphics->update_graphics(voltage,current,charge,false);
    }
    return;
}

MainWindow::MainWindow(yarp::os::ResourceFinder rf, yarp::dev::IBattery* p_ibat, QWidget *parent) : QMainWindow(parent)
{
    ibat = p_ibat;
    ui = 0;
    //ui = new Ui_BatteryDlg;  //@@@check this?
    //ui->setupUi(this);       //@@@check this?

    mainTimer = new QTimer(this);
    connect(mainTimer, SIGNAL(timeout()), this, SLOT(updateMain()));
    mainTimer->start(1000*10); //10 seconds
}

/*
Gtk::Window* do_pixbufs()
{
  fprintf(stderr,"ERROR: program should not reach this point \n");
  return new GraphicsManager("default");
}

GraphicsManager::GraphicsManager(string pictures_path)
{
  m_back_width = 0;
  m_back_height = 0;


  set_title("Battery Monitor");
  set_resizable(false);
  //set_decorated(false);
  set_keep_above(true);
  stick();

  pics_path = pictures_path;
  load_pixbufs();

  set_size_request(m_back_width, m_back_height);
  m_refPixbuf = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, FALSE, 8, m_back_width, m_back_height);
  m_DrawingArea.signal_expose_event().connect(sigc::mem_fun(*this, &GraphicsManager::on_drawingarea_expose));
  add(m_DrawingArea);

  show_all();
}

GraphicsManager::~GraphicsManager()
{
}

void GraphicsManager::load_pixbufs()
{
  if(m_refPixbuf_Background)
    return; 

  Glib::RefPtr<Gdk::Pixbuf> tmp_Pixbuf_Numbers;
  Glib::RefPtr<Gdk::Pixbuf> tmp_Pixbuf_Blocks;
  Glib::RefPtr<Gdk::Pixbuf> tmp_Pixbuf_Connected;
  Glib::RefPtr<Gdk::Pixbuf> tmp_Pixbuf_Charge;

  string filename;
  filename = pics_path+"background.bmp";
  printf ("loading: %s\n", filename.c_str());
  try
  {
    m_refPixbuf_Background = Gdk::Pixbuf::create_from_file(filename.c_str());
  }
  catch (Glib::FileError e)
  {
    g_message("caught Glib::FileError %d", e.code());
    return;
  }
  catch (Gdk::PixbufError e)
  {
      g_message("Gdk::PixbufError: %d",e.code());
    return;
  }

  filename = pics_path+"background2.bmp";
  printf ("loading: %s\n", filename.c_str());
  m_refPixbuf_Background2 = Gdk::Pixbuf::create_from_file(filename.c_str());

  filename = pics_path+"numbers.bmp";
  printf ("loading: %s\n", filename.c_str());
  tmp_Pixbuf_Numbers    = Gdk::Pixbuf::create_from_file(filename.c_str());

  filename = pics_path+"batt_blocks.bmp";
  printf ("loading: %s\n", filename.c_str());
  tmp_Pixbuf_Blocks     = Gdk::Pixbuf::create_from_file(filename.c_str());

  filename = pics_path+"batt_blocks.bmp";
  printf ("loading: %s\n", filename.c_str());
  tmp_Pixbuf_Connected  = Gdk::Pixbuf::create_from_file(filename.c_str());

  filename = pics_path+"charge.bmp";
  printf ("loading: %s\n", filename.c_str());
  tmp_Pixbuf_Charge  = Gdk::Pixbuf::create_from_file(filename.c_str());

  m_refPixbuf_Numbers   = tmp_Pixbuf_Numbers->add_alpha   (true, 255,0,255);
  m_refPixbuf_Blocks    = tmp_Pixbuf_Blocks->add_alpha    (true, 255,0,255);
  m_refPixbuf_Connected = tmp_Pixbuf_Connected->add_alpha (true, 255,0,255);
  m_refPixbuf_Charge    = tmp_Pixbuf_Charge->add_alpha    (true, 255,0,255);

  m_back_width = m_refPixbuf_Background->get_width();
  m_back_height = m_refPixbuf_Background->get_height();
}

bool GraphicsManager::on_drawingarea_expose(GdkEventExpose *event)
{
  gint rowstride = m_refPixbuf->get_rowstride();

  const guchar* pixels = m_refPixbuf->get_pixels() + (rowstride * event->area.y) + (event->area.x * 3);

  Glib::RefPtr<Gdk::Window> refWindow = m_DrawingArea.get_window();
  Glib::RefPtr<Gdk::GC> refGC = m_DrawingArea.get_style()->get_black_gc();

  refWindow->draw_rgb_image_dithalign(refGC,
                event->area.x, event->area.y,
                event->area.width, event->area.height,
                Gdk::RGB_DITHER_NORMAL,
                pixels, rowstride,
                event->area.x, event->area.y);

  return true;
}


void GraphicsManager::update_graphics(double voltage, double current, double charge, bool connected)
{
  if (charge>12)
  {m_refPixbuf_Background->copy_area( 0, 0, m_back_width, m_back_height, m_refPixbuf, 0, 0);}
  else
  {
      static bool b = false;
      if (b)
      {
          m_refPixbuf_Background->copy_area( 0, 0, m_back_width, m_back_height, m_refPixbuf, 0, 0);
      }
      else
      {
          m_refPixbuf_Background2->copy_area( 0, 0, m_back_width, m_back_height, m_refPixbuf, 0, 0);
      }
      b = !b;
  }
 
  //voltage = 24.2;
  //current = 20.8;
  //charge = 100;
  
   //draw numbers
  char buff[10];
  sprintf(buff,"%4.1f",voltage);
  int len = strlen(buff);
  int point_off=0;
  for (int i=0;i<len;i++)
  {
      int off;
      int xpos;
      int ypos;
      GdkRectangle dest;
      if (buff[i]=='.')
          point_off=17;

      if (buff[i]>='0' && buff[i]<='9')
      {
        off=(buff[i]-'0')*29+point_off;    
        dest.x=19+i*29-point_off;
        dest.y=21;
        dest.width=29;
        dest.height=52;
        xpos=19+i*29-off;
        ypos=21;
      
        m_refPixbuf_Numbers->composite(m_refPixbuf,
                                    dest.x, dest.y,
                                    dest.width, dest.height,
                                    xpos, ypos,
                                    1, 1, Gdk::INTERP_NEAREST,255);
      }
  }

  sprintf(buff,"%4.1f",fabs(current));
  len = strlen(buff);
  point_off=0;
  for (int i=0;i<len;i++)
  {
      int xoff=0;
      int yoff=0;
      int xpos=0;
      int ypos=0;
      GdkRectangle dest;
      if (buff[i]=='.')
          point_off=17;

      if (buff[i]>='0' && buff[i]<='9')
      {
        xoff=(buff[i]-'0')*29+point_off;    
        dest.x=19+i*29-point_off;
        dest.y=88-yoff;
        dest.width=29;
        dest.height=52;
        xpos=19+i*29-xoff;
        ypos=88-yoff;
      
        m_refPixbuf_Numbers->composite(m_refPixbuf,
                                    dest.x, dest.y,
                                    dest.width, dest.height,
                                    xpos, ypos,
                                    1, 1, Gdk::INTERP_NEAREST,255);
      }
  }

  //Draw charge/discharge text
  if (current<-0.3) 
  {
      int xoff=0;
      int yoff=8;
      int xpos=0;
      int ypos=0;

      GdkRectangle dest;

      dest.x=56-xoff;
      dest.y=77;
      dest.width=48;
      dest.height=8;
      xpos=56-xoff;
      ypos=77-yoff;
      
      m_refPixbuf_Charge->composite(m_refPixbuf,
                                    dest.x, dest.y,
                                    dest.width, dest.height,
                                    xpos, ypos,
                                    1, 1, Gdk::INTERP_NEAREST,255);
  }
  else
  {
      int xoff=0;
      int yoff=0;
      int xpos=0;
      int ypos=0;

      GdkRectangle dest;

      dest.x=56-xoff;
      dest.y=77-yoff;
      dest.width=48;
      dest.height=8;
      xpos=56-xoff;
      ypos=77-yoff;
      
      m_refPixbuf_Charge->composite(m_refPixbuf,
                                    dest.x, dest.y,
                                    dest.width, dest.height,
                                    xpos, ypos,
                                    1, 1, Gdk::INTERP_NEAREST,255);
  }

  //draw charge indicator
  int n_blocks = int (charge*11 / 100.0);
  for (int i=0;i<n_blocks;i++)
  {
      int xoff=0;
      int yoff=0;
      int xpos=0;
      int ypos=0;

      if    (current<-0.3) yoff=0;   //draw charging arrows
      else                 yoff=15;  //draw standard boxes

      GdkRectangle dest;

      dest.x=166-xoff;
      dest.y=135-i*6;
      dest.width=14;
      dest.height=7;
      xpos=166-xoff;
      ypos=135-i*6-yoff;
      
      m_refPixbuf_Blocks->composite(m_refPixbuf,
                                    dest.x, dest.y,
                                    dest.width, dest.height,
                                    xpos, ypos,
                                    1, 1, Gdk::INTERP_NEAREST,255);
  }

  m_DrawingArea.queue_draw();
  m_frame_num++;
}
*/