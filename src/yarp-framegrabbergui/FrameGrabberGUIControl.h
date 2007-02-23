// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Alessandro Scalzo
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef __FRAME_GRABBER_GUI_CONTROL_H__
#define __FRAME_GRABBER_GUI_CONTROL_H__

#include <gtkmm.h>
#include <yarp/dev/RemoteFrameGrabber.h>

class FrameGrabberGUIControl : public Gtk::Window, public yarp::dev::RemoteFrameGrabber
{
public:
	FrameGrabberGUIControl(char* loc, char* rem) : 
	  // base classes
	  Gtk::Window(),
	  yarp::dev::RemoteFrameGrabber(),
	  
	  //members
	  m_bri(0.0,1.0,0.02),
	  m_gai(0.0,1.0,0.02),
	  m_shu(0.0,1.0,0.02)
	{
		// yarp2

		yarp::os::Property config;
		config.put("remote",rem);
		config.put("local",loc);
		//config.put("stream","mcast");

		open(config);

		// GTKMM

		set_size_request(560,200);
		set_title("Grabber Remote GUI");

		m_bri.set_update_policy(Gtk::UPDATE_DISCONTINUOUS);
		m_gai.set_update_policy(Gtk::UPDATE_DISCONTINUOUS);
		m_shu.set_update_policy(Gtk::UPDATE_DISCONTINUOUS);
		
		double bri0=getBrightness();
		double gai0=getGain();
		double shu0=getShutter();

		if (bri0>=0.0 && bri0<=1.0)
			m_bri.set_value(bri0);
		else
			m_bri.set_value(0.5);

		if (gai0>=0.0 && gai0<=1.0)
			m_gai.set_value(gai0);
		else
			m_gai.set_value(0.5);

		if (shu0>=0.0 && shu0<=1.0)
			m_shu.set_value(shu0);
		else
			m_shu.set_value(0.5);

		m_bri.signal_value_changed().connect(sigc::mem_fun(*this,&FrameGrabberGUIControl::on_bri_changed));
		m_gai.signal_value_changed().connect(sigc::mem_fun(*this,&FrameGrabberGUIControl::on_gai_changed));
		m_shu.signal_value_changed().connect(sigc::mem_fun(*this,&FrameGrabberGUIControl::on_shu_changed));

		add(m_Box);

		m_Box.pack_start(*Gtk::manage(new Gtk::Label("Brightness", 0)),  Gtk::PACK_SHRINK);
		m_Box.pack_start(m_bri);
		m_Box.pack_start(*Gtk::manage(new Gtk::Label("Gain", 0)),  Gtk::PACK_SHRINK);
		m_Box.pack_start(m_gai);
		m_Box.pack_start(*Gtk::manage(new Gtk::Label("Shutter", 0)),  Gtk::PACK_SHRINK);
		m_Box.pack_start(m_shu);

		show_all_children();
	}

	virtual ~FrameGrabberGUIControl()
	{
		close();
	}

protected:
	Gtk::VBox m_Box;
	Gtk::HScale m_bri,m_gai,m_shu;

	// signal handlers
	virtual void on_bri_changed()
	{
		double val=m_bri.get_value();
		g_print("on_bri_changed(%f)\n",val);
		setBrightness(val);
	}
	virtual void on_gai_changed()
	{
		double val=m_gai.get_value();
		g_print("on_gai_changed(%f)\n",val);
		setGain(val);
	}
	virtual void on_shu_changed()
	{
		double val=m_shu.get_value();
		g_print("on_shu_changed(%f)\n",m_shu.get_value());
		setShutter(val);
	}
};

#endif
