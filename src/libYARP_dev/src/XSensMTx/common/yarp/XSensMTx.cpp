// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Radu Bogdan Rasu, Alexis Maldonado
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <ace/OS.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Time.h>
#include <yarp/os/Semaphore.h>
#include <string>

#include "MTComm.h"
#include "XSensMTx.h"

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
//using ACE_OS::printf;

class XSensMTxResources: public Thread
{
public:
    XSensMTxResources(void): _mutex(1)
    {
        _bStreamStarted=false;
        _bError=false;
            
        _last=0;

        _last=new double [12];
        for(int k=0;k<12;k++)
            _last[k]=0.0;
    }

    ~XSensMTxResources()
    {
        if (isRunning())
            stop();

        _bStreamStarted=false;

        if (_last!=0)
            {
                delete [] _last;
                _last=0;
            }
    }

    bool _bStreamStarted;
    bool _bError;

    CMTComm mtcomm;
    
    double *_last;

    Semaphore _mutex;

    virtual void run ();
};

void XSensMTxResources::run (void)
{   
    unsigned char data[MAXMSGLEN];
    float euler_data[3] = {0};
    float accel_data[3] = {0};
    float gyro_data [3] = {0};
    float magn_data [3] = {0};
    short datalen;
    int received;

    while (!Thread::isStopping ())
        {
            // Get data from the MTx device
            received = mtcomm.readDataMessage (data, datalen);
            // Parse and get value (EULER ORIENTATION)
            mtcomm.getValue (VALUE_ORIENT_EULER, euler_data, data, BID_MASTER);
            // Parse and get calibrated acceleration values
            mtcomm.getValue (VALUE_CALIB_ACC, accel_data, data, BID_MASTER);
            // Parse and get calibrated gyro values
            mtcomm.getValue (VALUE_CALIB_GYR, gyro_data, data, BID_MASTER);
            // Parse and get calibrated magnetometer values
            mtcomm.getValue (VALUE_CALIB_MAG, magn_data, data, BID_MASTER);
	    
            _mutex.wait ();
            
            _last[0]  = euler_data[0]; //roll
            _last[1]  = euler_data[1]; //pitch
            _last[2]  = euler_data[2]; //yaw

            _last[3]  = accel_data[0]; //accel-X
            _last[4]  = accel_data[1]; //accel-Y
            _last[5]  = accel_data[2]; //accel-Z
	    
            _last[6]  = gyro_data[0];  //gyro-X
            _last[7]  = gyro_data[1];  //gyro-Y
            _last[8]  = gyro_data[2];  //gyro-Z
	    
            _last[9]  = magn_data[0];  //magn-X
            _last[10] = magn_data[1];  //magn-Y
            _last[11] = magn_data[2];  //magn-Z

            if (received == MTRV_OK)
                _bError=false;
            else
                _bError=true;
		
            _mutex.post ();
        }
}

inline XSensMTxResources& RES(void *res) { return *(XSensMTxResources *)res; }

/**
 * Driver for XSens's MTx IMU unit.
 * @author Radu Bogdan Rusu, Alexis Maldonado
 */ 
XSensMTx::XSensMTx()
{
    system_resources = 0;
    nchannels        = 12;
}

XSensMTx::~XSensMTx()
{
    // stop thread first
    if (system_resources!=0)
        {
            delete ((XSensMTxResources *)(system_resources));
            system_resources=0;
        }
}

bool XSensMTx::read(Vector &out)
{
    XSensMTxResources &d= RES(system_resources);
    bool ret;
    
    if (d._bStreamStarted)
        {
            d._mutex.wait();
            
            // Euler+accel+gyro+magn orientation values
            for (int i = 0; i < nchannels; i++)
                out[i]=d._last[i];

            d._mutex.post();

            ret=!d._bError;
        }
    else
        ret=false;

    return ret;
}

bool XSensMTx::getChannels(int *nc)
{
    *nc=nchannels;
    return true;
}

bool XSensMTx::calibrate(int ch, double v)
{
    printf("Not implemented yet\n");
    return false;
}

bool XSensMTx::start()
{
    XSensMTxResources &d=RES(system_resources);

    d.start();
    d._bStreamStarted=true;

    return true;
}

bool XSensMTx::stop()
{
    XSensMTxResources &d=RES(system_resources);

    if (d.isRunning())
        d.stop();

    d._bStreamStarted=false;

    return true;
}

bool XSensMTx::open(yarp::os::Searchable &config)
{
    XSensMTxParameters par;
     
#ifdef WIN32
    par.comPort = config.check ("serial", Value(11),
        "numeric identifier of comport").asInt();
#else
    par.comPortString = config.check("serial",Value("/dev/ttyUSB0"),
                                     "device name of comport").asString().c_str();
#endif

    return open(par);
}

bool XSensMTx::open(const XSensMTxParameters &par)
{
    if (system_resources!=0)
        return false;

    system_resources=(void *) (new XSensMTxResources);

    XSensMTxResources &d=RES(system_resources);

    // Open the MTx device
#ifdef WIN32
    if (d.mtcomm.openPort (par.comPort) != MTRV_OK)
        return false;
#else
    if (d.mtcomm.openPort (par.comPortString.c_str ()) != MTRV_OK)
        return false;
#endif

    int outputSettings = OUTPUTSETTINGS_ORIENTMODE_EULER;
    
    unsigned long tmpOutputMode, tmpOutputSettings;
    unsigned short tmpDataLength;
		
    // Put MTi/MTx in Config State
    if(d.mtcomm.writeMessage (MID_GOTOCONFIG) != MTRV_OK){
        printf ("No device connected\n");
        return false;
    }

    unsigned short numDevices;
    // Get current settings and check if Xbus Master is connected
    if (d.mtcomm.getDeviceMode(&numDevices) != MTRV_OK) {
        if (numDevices == 1)
            printf ("MTi / MTx has not been detected\nCould not get device mode\n");
        else
            printf ("Not just MTi / MTx connected to Xbus\nCould not get all device modes\n");
        return false;
    }

    // Check if Xbus Master is connected
    d.mtcomm.getMode (tmpOutputMode, tmpOutputSettings, tmpDataLength, BID_MASTER);
    if (tmpOutputMode == OUTPUTMODE_XM)
        {
            // If Xbus Master is connected, attached Motion Trackers should not send sample counter
            printf ("Sorry, this driver only talks to one MTx device.\n");
            return false;
        }

    int outputMode = OUTPUTMODE_CALIB + OUTPUTMODE_ORIENT;
    // Set output mode and output settings for the MTi/MTx
    if (d.mtcomm.setDeviceMode(outputMode, outputSettings, BID_MASTER) != MTRV_OK) {
        printf ("Could not set device mode(s)\n");
        return false;
    }

    // Put MTi/MTx in Measurement State
    d.mtcomm.writeMessage (MID_GOTOMEASUREMENT);

    // start thread
    return XSensMTx::start();
}

bool XSensMTx::close()
{
    // stop thread
    if (system_resources==0)
        return false; //the device was never opened, or there was an error 

    XSensMTx::stop();

    XSensMTxResources &d=RES(system_resources);

    // Close the MTx device
    if (d.mtcomm.close () != MTRV_OK)
        {
            delete ((XSensMTxResources *)(system_resources));
            system_resources=0;
            return false;
        }
    else
        {
            delete ((XSensMTxResources *)(system_resources));
            system_resources=0;
            return true;
        }
}

