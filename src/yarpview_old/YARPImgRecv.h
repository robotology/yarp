// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Matteo Brunettini
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

// YARPImgRecv.h: interface for the YARPImgRecv class.
//
//////////////////////////////////////////////////////////////////////
 
#if !defined(AFX_YARPIMGRECV_H__2654CA92_2656_48E0_9A91_F6CD59F11AC4__INCLUDED_)
#define AFX_YARPIMGRECV_H__2654CA92_2656_48E0_9A91_F6CD59F11AC4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//=============================================================================
// YARP Includes - General
//=============================================================================
#include <ace/config.h>
#include <ace/OS.h>
//=============================================================================
// YARP Includes - Class Specific
//=============================================================================
#include <yarp/sig/Image.h>
//#include <yarp/sig/ImageFile.h>
#include <yarp/os/BufferedPort.h>
//#include <yarp/YARPLogpolar.h>
#include <yarp/os/Time.h>

// TO DO: convert to thread??


typedef yarp::os::BufferedPort<yarp::sig::FlexImage> PortType;

class YARPImgRecv  
{
public:
	bool GetLogpolar();
	bool GetFovea();
	void SetFovea(bool status=true);
	void SetLogopolar(bool status=true);
	double GetEstimatedInterval(void);
	bool Disconnect();
	int GetWidth();
	int GetHeight();
	bool Update();
    bool SaveLastImage(char *fileName);
    //bool SaveLastImage(char *fileName, int format=YARPImageFile::FORMAT_PPM);
    bool GetLastImage(yarp::sig::Image *data);
	bool Connect();
	bool Connect(char *portName, char *network=NULL);
	YARPImgRecv(char *portNamechar, char *network=NULL);
	YARPImgRecv();
	virtual ~YARPImgRecv();

private:
    // solaris preferred order
	yarp::sig::ImageOf<yarp::sig::PixelBgr> _img, _logImg;
	PortType _inPort;

	bool _registerPorts();
	bool _unregisterPorts();
    //void _logpolarConversion(bool fovea, YARPGenericImage *dest);

	char _portName[256];
	char _network[256];
    //YARPLogpolar _logopolarMapper;
	bool _connected;
	bool _portNameIsValid;
	int _width;
	int _height;
	bool _logpolar;
	bool _fovea;
	double _currentFrameTime;
	double _lastFrameTime;
};

#endif // !defined(AFX_YARPIMGRECV_H__2654CA92_2656_48E0_9A91_F6CD59F11AC4__INCLUDED_)
