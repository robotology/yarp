// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
* Author: Marco Randazzo <marco.randazzo@iit.it>
* CopyPolicy: Released under the terms of the GPLv2 or later, see GPL.TXT
*/
 
#include <rpLidar.h>

#include <yarp/os/Time.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>

//#define LASER_DEBUG 1
#define FORCE_SCAN 1

using namespace std;

bool RpLidar::open(yarp::os::Searchable& config)
{
    bool correct=true;
    info = "Fake Laser device for test/debugging";
    device_status = DEVICE_OK_STANBY;

#if LASER_DEBUG
    yDebug("%s\n", config.toString().c_str());
#endif

    bool br = config.check("GENERAL");
    if (br != false)
    {
        yarp::os::Searchable& general_config = config.findGroup("GENERAL");
        period = general_config.check("Period", Value(50), "Period of the sampling thread").asInt();
    }

    min_distance = 0.1; //m
    max_distance = 2.5;  //m
    min_angle = 0;      //degrees
    max_angle = 359;    //degrees
    resolution = 1.0;   //degrees

    sensorsNum = (int)((max_angle-min_angle)/resolution);
    laser_data.resize(sensorsNum);
    
    yInfo("Starting debug mode");
    yInfo("max_dist %f, min_dist %f", max_distance, min_distance);
    yInfo("max_angle %f, min_angle %f", max_angle, min_angle);
    yInfo("resolution %f", resolution);
    yInfo("sensors %d", sensorsNum);
    Time::turboBoost();

    yarp::os::Searchable& general_config = config.findGroup("GENERAL");
    bool ok = general_config.check("Serial_Configuration");
    if (!ok)
    {
        yError("Cannot find configuration file for serial port communication!");
        return false;
    }
    yarp::os::ConstString serial_filename = general_config.find("Serial_Configuration").asString();

    Property prop;
    prop.put("device", "serialport");
    ok = prop.fromConfigFile(serial_filename.c_str(), config, false);
    if (!ok)
    {
        yError("Unable to read from serial port configuration file");
        return false;
    }

    pSerial = 0;
    driver.open(prop);
    if (!driver.isValid())
    {
        yError("Error opening PolyDriver check parameters");
        return false;
    }
    driver.view(pSerial);
    if (!pSerial)
    {
        yError("Error opening serial driver. Device not available");
        return false;
    }

    //
    int cleanup = pSerial->flush();
    if (cleanup > 0)
    {
        yDebug("cleanup performed, flushed %d chars", cleanup);
    }

    // check health
    bool b_health = HW_getHealth();
    if (b_health == false)
    {
        yWarning("Sensor in error status, attempt to recover");
        HW_reset();
        b_health = HW_getHealth();
        if (b_health == false)
        {
            yError("Unable to recover error");
            return false;
        }
        else
        {
            yInfo("Sensor recovered from a previous error status");
        }
    }
    yInfo("Sensor ready");

    string info;
    bool b_info = HW_getInfo(info);

    RateThread::start();
    return true;
}

bool RpLidar::close()
{
    RateThread::stop();

    driver.close();
    return true;
}

bool RpLidar::getDistanceRange(double& min, double& max)
{
    mutex.wait();
    min = min_distance;
    max = max_distance;
    mutex.post();
    return true;
}

bool RpLidar::setDistanceRange(double min, double max)
{
    mutex.wait();
    min_distance = min;
    max_distance = max;
    mutex.post();
    return true;
}

bool RpLidar::getScanLimits(double& min, double& max)
{
    mutex.wait();
    min = min_angle;
    max = max_angle;
    mutex.post();
    return true;
}

bool RpLidar::setScanLimits(double min, double max)
{
    mutex.wait();
    min_angle = min;
    max_angle = max;
    mutex.post();
    return true;
}

bool RpLidar::getHorizontalResolution(double& step)
{
    mutex.wait();
    step = resolution;
    mutex.post();
    return true;
}

bool RpLidar::setHorizontalResolution(double step)
{
    mutex.wait();
    resolution = step;
    mutex.post();
    return true;
}

bool RpLidar::getScanRate(double& rate)
{
    mutex.wait();
    rate = 1.0 / (period * 1000);
    mutex.post();
    return true;
}

bool RpLidar::setScanRate(double rate)
{
    mutex.wait();
    period = (int)((1.0 / rate) / 1000.0);
    mutex.post();
    return false;
}


bool RpLidar::getMeasurementData(yarp::sig::Vector &out)
{
    mutex.wait();
    out = laser_data;
    mutex.post();
    device_status = yarp::dev::IRangefinder2D::DEVICE_OK_IN_USE;
    return true;
}

bool RpLidar::getDeviceStatus(Device_status &status)
{
    mutex.wait();
    status = device_status;
    mutex.post();
    return true; 
}

bool RpLidar::threadInit()
{
#if LASER_DEBUG
    yDebug("RpLidar:: thread initialising...\n");
    yDebug("... done!\n");
#endif 

    if (!HW_start())
    {
        yError("Unable to put sensor in scan mode!");
        return false;
    }
    return true;
}

bool RpLidar::HW_getInfo(string& s_info)
{
    int r = 0;
    unsigned char cmd_arr[2];
    cmd_arr[0] = 0xA5;
    cmd_arr[1] = 0x50;
    pSerial->send((char *)cmd_arr, 2);

    yarp::os::Time::delay(0.010);

    unsigned char s[255];
    r = pSerial->receiveBytes(s, 7);
    if (r!=7)
    {
        yError("Received answer with wrong length: %d", r);
        return false;
    }

    if ((unsigned char)s[0] != 0xA5 || (unsigned char)s[1] != 0x5A || (unsigned char)s[2] != 0x14 || (unsigned char)s[6] != 0x04)
    {
        yError("Invalid answer header");
        return false;
    }

    r = pSerial->receiveBytes(s, 20);
    if (r!=20)
    {
        yError("Received answer with wrong length: %d", r);
        return false;
    }
    char info[512];
    sprintf(info, "model %d fw_major %d fw_minor %d hardware %d serial number %c%c%c%c%c %c%c%c%c%c %c%c%c%c%c",
                   s[0],  s[1],  s[2],  s[3],
                   s[4],  s[5],  s[6],  s[7],  s[8],
                   s[9],  s[10], s[11], s[12], s[13],
                   s[14], s[15], s[16], s[17], s[18], s[19]);
    s_info.append(info);
    return true;
}

bool RpLidar::HW_getHealth()
{
    pSerial->flush();

    int r = 0;
    unsigned char cmd_arr[2];
    cmd_arr[0] = 0xA5;
    cmd_arr[1] = 0x52;
    pSerial->send((char *)cmd_arr, 2);

    yarp::os::Time::delay(0.010);
    
    unsigned char s[255];
    memset(s, 0, 255);
    r = pSerial->receiveBytes(s,7);
    if (r!=7)
    {
        yError("Received answer with wrong length: %d", r);
        return false;
    }

    if ((unsigned char)s[0] != 0xA5 || (unsigned char)s[1] != 0x5A || (unsigned char)s[2] != 3 || (unsigned char)s[6] != 0x06)
    {
        yError("Invalid answer header");
        return false;
    }

    memset(s, 0, 255);
    r = pSerial->receiveBytes(s,3);
    if (r !=3)
    {
        yError("Received answer with wrong length: %d", r);
        return false;
    }

    int status = s[0];
    int code = s[1] << 8 | s[2];
    if (status == 0)
    {
        return true;
    }
    else if (status == 1)
    {
        yWarning("sensor in warning status, code %d", code);
        return true;
    }
    else if (status == 2)
    {
        yError("sensor in error status, code %d", code);
        return true;
    }
    yError("Unkwnon answer code");
    return false;
}

bool RpLidar::HW_reset()
{
    pSerial->flush();

    unsigned char cmd_arr[2];
    cmd_arr[0] = 0xA5;
    cmd_arr[1] = 0x40;
    pSerial->send((char *)cmd_arr, 2);

    yarp::os::Time::delay(0.010);
    return true;
}

bool RpLidar::HW_start()
{
    pSerial->flush();

    int r = 0;

    unsigned char cmd_arr[2];
    cmd_arr[0] = 0xA5;
#if FORCE_SCAN
    cmd_arr[1] = 0x21;
#else
    cmd_arr[1] = 0x20;
#endif
    pSerial->send((char *)cmd_arr,2);

    yarp::os::Time::delay(0.010);

    unsigned char s[255];
    memset(s, 0, 255);
    r = pSerial->receiveBytes(s, 7);
    if (r != 7)
    {
        yError("Received answer with wrong length: %d", r);
        return false;
    }

    if ((unsigned char)s[0] != 0xA5 || (unsigned char)s[1] != 0x5A || (unsigned char)s[2] != 0x05 ||
        (unsigned char)s[5] != 0x40 || (unsigned char)s[6] != 0x81)
    {
        yError("Invalid answer header");
        return false;
    }

    return true;
}

bool RpLidar::HW_stop()
{
    pSerial->flush();

    unsigned char cmd_arr[2];
    cmd_arr[0] = 0xA5;
    cmd_arr[1] = 0x25;
    pSerial->send((char*)cmd_arr,2);

    yarp::os::Time::delay(0.010);
    return true;
}

void RpLidar::run()
{
    mutex.wait();
    laser_data.clear();
    unsigned char buff[1000];
    memset(buff, 0, 1000);
    unsigned int r = pSerial->receiveBytes(buff, 1000);
    //yDebug() << "received " << r << " bytes";
    if (r < 5)
    {
        //yError() << "Wrong scan size: " << r;
        mutex.post();
        return;
    }

    bool new_scan = false;
    int laser_data_size = 0;

    for (size_t i = 0; i < r-(4+5); )
    {
        int start = (buff[i+0]) & 0x01;
        int lock  = (buff[i+0] >> 1) & 0x01;
        int check = (buff[i+1] & 0x01);

        int n_start = (buff[i + 5]) & 0x01;
        int n_lock  = (buff[i + 5] >> 1) & 0x01;
        int n_check = (buff[i + 6] & 0x01);

        int quality = (buff[i + 0] >> 2);
        int i_angle =     (  (buff[i+2] >> 1 ) << 8) | (buff[i+1]);
        int i_distance =     (buff[i+4] << 8)        | (buff[i+3]); //this is ok!

        if (start == lock)
        {
            yError() << "lock error 1 ";
            i++ ;
            new_scan = false;
            continue;
        }
        if (n_start == n_lock)
        {
            yError() << "lock error 2 ";
            i++;
            new_scan = false;
            continue;
        }
        if (start == 1 && n_start == 1)
        {
            yError() << "lock error 3 ";
            i++;
            new_scan = false;
            continue;
        }
        if (check != 1)
        {
            yError() << "checksum error 1";
            i++;
            new_scan = false;
            continue;
        }
        if (n_check != 1)
        {
            yError() << "checksum error 2";
            i++;
            new_scan = false;
            continue;
        }
        if (start == 1 && new_scan == false)
        {
            //this is a new scan
            new_scan = true;
            laser_data_size = laser_data.size(); //size should be zero
        }
        else if (start == 1 && new_scan == true)
        {
            //end of data
            new_scan = false;
            laser_data_size = laser_data.size(); //size should be 
            break;
        }

        double distance = i_distance / 4.0; //mm
        double angle = i_angle / 64.0; //deg

        if (i_distance == 0)
        {
       //     yWarning() << "Invalid Measurement " << i/5;
        }
        if (quality == 0)
        {
      //      yWarning() << "Quality Low" << i / 5;
        }
        if (angle > 360)
        {
            yWarning() << "Invalid angle" << i / 5;
            i++;
            continue;
        }

        if (distance < min_distance) distance = min_distance;
        if (distance > max_distance) distance = max_distance;
        laser_data.push_back(distance);
        i += 5;
    }

    /*--------------------------------------------------------------*/
   /* {
        yError() << "Wrong scan size: " << r;
        bool b_health = HW_getHealth();
        if (b_health == true)
        {
            if (!HW_reset())
            {
                yError("Unable to reset sensor!");
            }
            yWarning("Sensor reset after error");
            if (!HW_start())
            {
                yError("Unable to put sensor in scan mode!");
            }
            mutex.post();
            return;
        }
        else
        {
            yError() << "System in error state";
        }
    }*/

    mutex.post();
    return;
}

void RpLidar::threadRelease()
{
#if LASER_DEBUG
    yDebug("RpLidar Thread releasing...");
    yDebug("... done.");
#endif

    if (!HW_stop())
    {
        yError("Unable to stop sensor!");
        HW_reset();
    }
    return;
}

bool RpLidar::getDeviceInfo(yarp::os::ConstString &device_info)
{
    this->mutex.wait();
    device_info = info;
    this->mutex.post();
    return true;
}
