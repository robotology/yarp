// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Matteo Brunettini
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


// YARPImgRecv.cpp: implementation of the YARPImgRecv class.
//
//////////////////////////////////////////////////////////////////////

#include "YARPImgRecv.h"

#include <yarp/sig/ImageFile.h>

using namespace yarp::os;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
 
YARPImgRecv::YARPImgRecv()
{
    _portName[0] = '\0';
    _network[0] = '\0';
	//sprintf(_portName,"\0");
	//sprintf(_network,"\0");
	_connected = false;
	_portNameIsValid = false;
	_logpolar = false;
	_fovea = false;
	_currentFrameTime = 0.0;
	_lastFrameTime = 0.0;
}

YARPImgRecv::~YARPImgRecv()
{
	if (_connected)
		Disconnect();
}

YARPImgRecv::YARPImgRecv(char *portName, char *network)
{
	sprintf(_portName, portName);
	if (network == NULL)
        {
            sprintf(_network, "default");
        }
	else
        {
            sprintf(_network, network);
        }
	_connected = false;
	_portNameIsValid = true;
	_width = 0;
	_height = 0;
	_logpolar = false;
	_fovea = false;
	_currentFrameTime = 0;
	_lastFrameTime = 0;
}

bool YARPImgRecv::Connect()
{
	if ( _connected || !_portNameIsValid)
		return false;

	bool res = false;
	res = _registerPorts();
	if (res == true)
        {
            _connected = true;
            return true;
        }
	else
        {
            _connected = false;
            return false;
        }
}

bool YARPImgRecv::Connect(char *portName, char *network)
{
	bool res = false;

	if ( _connected )
		return false;
	sprintf(_portName,portName);
	if (network == NULL)
        {
            sprintf(_network, "default");
        }
	else
        {
            sprintf(_network, network);
        }
	_portNameIsValid = true;

	res = _registerPorts();
	if (res == true)
        {
            _connected = true;
            return true;
        }
	else
        {
            _connected = false;
            return false;
        }
	_width = 0;
	_height = 0;
}

bool YARPImgRecv::Update()
{
	
	if ( _connected == false)
        return false;
	
	PortType::ContentType *content = _inPort.read(0);

	if (content==NULL) {
        _width = 0;
        _height = 0;
        return false;
    }
    
	_width = content->width();
	_height = content->height();
	
	_lastFrameTime = _currentFrameTime;
	_currentFrameTime = Time::now();
	return true;
}

bool YARPImgRecv::GetLastImage(yarp::sig::Image *data)
{
	if ( _connected == false)
		return false;
	
	if ((_width == 0) || (_height == 0))
		return false;

	if (_logpolar)
        {
            yarp::sig::Image *last = _inPort.lastRead();
            if (last!=NULL) {
                _img.copy(*last);
            }
            //_logpolarConversion(_fovea, data);
            exit(1);
        }
	else
        {
            if ( (_width != data->width()) || (_height != data->height()) )
                data->resize(_width, _height);
            yarp::sig::Image *last = _inPort.lastRead();
            if (last!=NULL) {
                data->copy(*last);
            }
        }
	
	return true;
}


bool YARPImgRecv::SaveLastImage(char *fileName) {

    if ( _connected == false)
        return false;
	
    if ((_width == 0) || (_height == 0))
        return false;
	
    if (_logpolar)
        {
            printf("LOGPOLAR is not supported\n");
            /*
              _img.CastCopy(_inPort.Content());
              _logpolarConversion(_fovea, &_logImg);
              YARPImageFile::Write(fileName, _logImg, format);
            */
        }
    else
        {
            //_img.CastCopy(_inPort.Content());
            //YARPImageFile::Write(fileName, _img, format);
            yarp::sig::FlexImage *img = _inPort.lastRead();
            if (img!=NULL) {
                yarp::sig::file::write(*img,fileName);
            }
        }

    return true;
}


bool YARPImgRecv::Disconnect()
{
	bool res = false;

	if ( _connected == false)
		return false;

	res = _unregisterPorts();
	if (res == true)
        {
            _connected = false;
            return true;
        }
	else
		return false;

}

bool YARPImgRecv::_registerPorts()
{
	int res = 0;
	res = _inPort.open(_portName);
    _inPort.setStrict(false);

	if  (res)
		return true;
	else 
		return false;
}

bool YARPImgRecv::_unregisterPorts()
{
	//int res = 0;
	_inPort.close();

    return true;
}
int YARPImgRecv::GetWidth()
{
	return _width;
}

int YARPImgRecv::GetHeight()
{
	return _height;
}

double YARPImgRecv::GetEstimatedInterval()
{
	double estInterval;
	estInterval = _currentFrameTime - _lastFrameTime;
	return estInterval;
}

void YARPImgRecv::SetLogopolar(bool status)
{
	_logpolar = status;
}

void YARPImgRecv::SetFovea(bool status)
{
	_fovea = status;
}

bool YARPImgRecv::GetFovea()
{
	return _fovea;
}

bool YARPImgRecv::GetLogpolar()
{
	return _logpolar;
}

/*

void YARPImgRecv::_logpolarConversion(bool fovea, YARPGenericImage *dest)
{
_img.Refer (_inPort.Content());
if (fovea)
{
    _logopolarMapper.SafeLogpolar2CartesianFovea (_img, _logImg);
		
}
 else
     {
         _logopolarMapper.SafeLogpolar2Cartesian (_img, _logImg);
     }
 *dest = _logImg;
}
*/
