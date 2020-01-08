/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#define _USE_MATH_DEFINES

#include "laserHokuyo.h"

#include <yarp/os/Time.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>

//#define LASER_DEBUG

#ifndef DEG2RAD
#define DEG2RAD M_PI/180.0
#endif

using namespace std;

bool laserHokuyo::open(yarp::os::Searchable& config)
{
    internal_status = HOKUYO_STATUS_NOT_READY;
    info = "Hokuyo Laser";
    device_status = DEVICE_OK_STANBY;

#ifdef LASER_DEBUG
    yDebug("%s\n", config.toString().c_str());
#endif

    bool br = config.check("GENERAL");
    if (br == false)
    {
        yError("cannot read 'GENERAL' section");
        return false;
    }
    yarp::os::Searchable& general_config = config.findGroup("GENERAL");

    //list of mandatory options
    //TODO change comments
    period = general_config.check("Period", Value(50), "Period of the sampling thread").asInt32() / 1000.0;

    if (general_config.check("max_angle") == false) { yError() << "Missing max_angle param"; return false; }
    if (general_config.check("min_angle") == false) { yError() << "Missing min_angle param"; return false; }
    max_angle = general_config.find("max_angle").asFloat64();
    min_angle = general_config.find("min_angle").asFloat64();

    start_position = general_config.check("Start_Position", Value(0), "Start position").asInt32();
    end_position = general_config.check("End_Position", Value(1080), "End Position").asInt32();

    error_codes = general_config.check("Convert_Error_Codes", Value(0), "Substitute error codes with legal measurments").asInt32();
    std::string s = general_config.check("Laser_Mode", Value("GD"), "Laser Mode (GD/MD").asString();

    if (general_config.check("Measurement_Units"))
    {
        yError() << "Deprecated parameter 'Measurement_Units'. Please Remove it from the configuration file.";
    }

    if (error_codes==1)
    {
        yInfo("'error_codes' option enabled. Invalid samples will be substituted with out-of-range measurements.");
    }
    if (s=="GD")
    {
        laser_mode = GD_MODE;
        yInfo("Using GD mode (single acquisition)");
    }
    else if (s=="MD")
    {
        laser_mode = MD_MODE;
        yInfo("Using MD mode (continuous acquisition)");
    }
    else
    {
        laser_mode = GD_MODE;
        yError("Laser_mode not found. Using GD mode (single acquisition)");
    }
    setPeriod(period);

    bool br2 = config.check("SERIAL_PORT_CONFIGURATION");
    if (br2 == false)
    {
        yError("cannot read 'SERIAL_PORT_CONFIGURATION' section");
        return false;
    }
    yarp::os::Searchable& serial_config = config.findGroup("SERIAL_PORT_CONFIGURATION");
    string ss = serial_config.toString();
    Property prop;
    prop.fromString(ss);
    prop.put("device", "serialport");

    driver.open(prop);
    if (!driver.isValid())
    {
        yError("Error opening PolyDriver check parameters");
        return false;
    }

    pSerial=nullptr;
    driver.view(pSerial);

    if (!pSerial)
    {
        yError("Error opening serial driver. Device not available");
        return false;
    }

    Bottle b;
    Bottle b_ans;
    string ans;

    // *** Check if the URG device is present ***
    b.addString("SCIP2.0\n");
    pSerial->send(b);
    yarp::os::SystemClock::delaySystem(0.040);
    pSerial->receive(b_ans);
    if (b_ans.size()>0)
    {
        yInfo("URG device successfully initialized.\n");
        yDebug("%s\n", b_ans.get(0).asString().c_str());
    }
    else
    {
        yError("Error: URG device not found.\n");
        //return false;
    }
    b.clear();
    b_ans.clear();

    // *** Change the baud rate to 115200 ***
    /*b.addString("SS01152001\n");
    pSerial->send(b);
    yarp::os::SystemClock::delaySystem(0.040);
    pSerial->receive(b_ans);
    ans = b_ans.get(0).asString();
    yDebug("%s\n",ans.c_str());
    b.clear();
    b_ans.clear();*/

    // *** Read the firmware version parameters ***
    b.addString("VV\n");
    pSerial->send(b);
    yarp::os::SystemClock::delaySystem(0.040);
    pSerial->receive(b_ans);
    ans = b_ans.get(0).asString();
    yDebug("%s\n",ans.c_str());
    b.clear();
    b_ans.clear();

    // *** Read the sensor specifications ***
    b.addString("II\n");
    pSerial->send(b);
    yarp::os::SystemClock::delaySystem(0.040);
    pSerial->receive(b_ans);
    ans = b_ans.get(0).asString();
    yDebug("%s\n", ans.c_str());
    b.clear();
    b_ans.clear();

    // *** Read the URG configuration parameters ***
    b.addString("PP\n");
    pSerial->send(b);
    yarp::os::SystemClock::delaySystem(0.040);
    pSerial->receive(b_ans);
    /*
    syntax of the answer:
    MODL ... Model information of the sensor.
    DMIN ... Minimum measurable distance [mm]
    DMAX ... Maximum measurable distance [mm]
    ARES ... Angular resolution(Number of splits in 360 degree)
    AMIN ... First Step of the Measurement Range
    AMAX ... Last Step of the Measurement Range
    AFRT ... Step number on the sensor's front axis
    SCAN ... Standard angular velocity
    */
    ans = b_ans.get(0).asString();
    yDebug( "%s\n", ans.c_str());
    //parsing the answer
    size_t found;
    found = ans.find("MODL");
    if (found!=string::npos) sensor_properties.MODL = string(ans.c_str()+found+5);
    found = ans.find("DMIN");
    if (found!=string::npos) sensor_properties.DMIN = atoi(ans.c_str()+found+5);
    found = ans.find("DMAX");
    if (found!=string::npos) sensor_properties.DMAX = atoi(ans.c_str()+found+5);
    found = ans.find("ARES");
    if (found!=string::npos) sensor_properties.ARES = atoi(ans.c_str()+found+5);
    found = ans.find("AMIN");
    if (found!=string::npos) sensor_properties.AMIN = atoi(ans.c_str()+found+5);
    found = ans.find("AMAX");
    if (found!=string::npos) sensor_properties.AMAX = atoi(ans.c_str()+found+5);
    found = ans.find("AFRT");
    if (found!=string::npos) sensor_properties.AFRT = atoi(ans.c_str()+found+5);
    found = ans.find("SCAN");
    if (found!=string::npos) sensor_properties.SCAN = atoi(ans.c_str()+found+5);
    b.clear();
    b_ans.clear();

    // *** Turns on the Laser ***
    b.addString("BM\n");
    pSerial->send(b);
    yarp::os::SystemClock::delaySystem(0.040);
    pSerial->receive(b_ans);
    // @@@TODO: Check the answer
    b.clear();
    b_ans.clear();



    //elements are:
    sensorsNum=16*12;
    laser_data.resize(sensorsNum);

    if (laser_mode==MD_MODE)
    {
        // *** Starts endless acquisition mode***
        char message [255];
        sprintf (message,"MD%04d%04d%02d%01d%02d\n",start_position,end_position,1,1,0);
        b.addString(message);
        pSerial->send(b);
        b.clear();
        b_ans.clear();
    }

    else if (laser_mode==GD_MODE)
    {
        // *** Starts one single acquisition ***
        char message [255];
        sprintf (message,"GD%04d%04d%02d\n",start_position,end_position,1);
        b.addString(message);
        pSerial->send(b);
        b.clear();
        b_ans.clear();
    }

    return PeriodicThread::start();
}

bool laserHokuyo::close()
{
    PeriodicThread::stop();

    Bottle b;
    Bottle b_ans;

    // *** Turns off the Laser ***
    b.addString("QT\n");
    pSerial->send(b);
    pSerial->receive(b_ans);

    // @@@TODO: Check the answer

    b.clear();
    b_ans.clear();

    driver.close();
    return true;
}

bool laserHokuyo::getDistanceRange(double& min, double& max)
{
    //should return range 0.1-30m (or 100, 30000mm depending on the measurement units)
    min = 0.1;
    max = 30;
    return true;
}

bool laserHokuyo::setDistanceRange(double min, double max)
{
    yError("setDistanceRange NOT YET IMPLEMENTED");
    return false;
}

bool laserHokuyo::getScanLimits(double& min, double& max)
{
    //degrees
    min = min_angle;
    max = max_angle;
    return true;
}

bool laserHokuyo::setScanLimits(double min, double max)
{
    yError("setScanLimits NOT YET IMPLEMENTED");
    return false;
}

bool laserHokuyo::getHorizontalResolution(double& step)
{
    step = 0.25; //deg //1080*0.25=270
    return true;
}

bool laserHokuyo::setHorizontalResolution(double step)
{
    yError("setHorizontalResolution NOT YET IMPLEMENTED");
    return false;
}

bool laserHokuyo::getScanRate(double& rate)
{
    rate = 20; //20 Hz = 50 ms
    return true;
}

bool laserHokuyo::setScanRate(double rate)
{
    yError("setScanRate NOT YET IMPLEMENTED");
    return false;
}

bool laserHokuyo::getRawData(yarp::sig::Vector &out)
{
    if (internal_status != HOKUYO_STATUS_NOT_READY)
    {
        mutex.lock();
#ifdef LASER_DEBUG
        //yDebug("data: %s\n", laser_data.toString().c_str());
#endif
        out = laser_data;
        mutex.unlock();
        device_status = yarp::dev::IRangefinder2D::DEVICE_OK_IN_USE;
        return true;
    }
    device_status = yarp::dev::IRangefinder2D::DEVICE_GENERAL_ERROR;
    return false;
}

bool laserHokuyo::getLaserMeasurement(std::vector<LaserMeasurementData> &data)
{
    if (internal_status != HOKUYO_STATUS_NOT_READY)
    {
        mutex.lock();
#ifdef LASER_DEBUG
        //yDebug("data: %s\n", laser_data.toString().c_str());
#endif
        size_t size = laser_data.size();
        data.resize(size);
        if (max_angle < min_angle)
        {
            yError() << "getLaserMeasurement failed";
            mutex.unlock();
            return false;
        }

        double laser_angle_of_view = max_angle - min_angle;
        for (size_t i = 0; i < size; i++)
        {
            double angle = (i / double(size)*laser_angle_of_view + min_angle)* DEG2RAD;
            data[i].set_polar(laser_data[i], angle);
        }
        mutex.unlock();
        device_status = yarp::dev::IRangefinder2D::DEVICE_OK_IN_USE;
        return true;
    }

    device_status = yarp::dev::IRangefinder2D::DEVICE_GENERAL_ERROR;
    return false;
}
bool laserHokuyo::getDeviceStatus(Device_status &status)
{
    mutex.lock();
    status = device_status;
    mutex.unlock();
    return true;
}

bool laserHokuyo::threadInit()
{
#ifdef LASER_DEBUG
    yDebug("laserHokuyo:: thread initialising...\n");
    yDebug("... done!\n");
#endif

    return true;
}

inline int laserHokuyo::calculateCheckSum(const char* buffer, int size, char actual_sum)
{
  char expected_sum = 0x00;
  int i;

  for (i = 0; i < size; ++i)
  {
        expected_sum += buffer[i];
  }
  expected_sum = (expected_sum & 0x3f) + 0x30;

  return (expected_sum == actual_sum) ? 0 : -1;
}

inline long laserHokuyo::decodeDataValue(const char* data, int data_byte)
{
  long value = 0;
  for (int i = 0; i < data_byte; ++i)
  {
        value <<= 6;
        value &= ~0x3f;
        value |= data[i] - 0x30;
  }
  return value;
}

int laserHokuyo::readData(const Laser_mode_type laser_mode, const char* text_data, const int text_data_len, int current_line, yarp::sig::Vector& data)
{
    static char data_block [4000];

    if (text_data_len==0)
    {
        return  HOKUYO_STATUS_ERROR_NOTHING_RECEIVED;
    }

//     long int timestamp = 0 ;

    // termination complete check
    if (text_data_len == 1 &&
        text_data[0] == '\n')
        {
            //Decode the data
            int len = strlen(data_block);
            for (int value_counter =0; value_counter < len; value_counter+=3)
            {
                int value = decodeDataValue(data_block+value_counter, 3);
                if (value<sensor_properties.DMIN && error_codes==1)
                {
                    value=sensor_properties.DMAX;
                }
                //units are m
                 data.push_back(value/1000.0);
            }
            return HOKUYO_STATUS_ACQUISITION_COMPLETE;
        }

    // check in the first line if it is a valid answer to GD command
    if (current_line == 0)
    {
        data_block[0]='\0'; //resets the datablock;
        if ((laser_mode == MD_MODE && (text_data[0] != 'M' || text_data[1] != 'D')) ||
            (laser_mode == GD_MODE && (text_data[0] != 'G' || text_data[1] != 'D')))
            {
                #ifdef LASER_DEBUG
                yDebug("Invalid answer to a MD command: %s\n", text_data);
                #endif
                return HOKUYO_STATUS_ERROR_INVALID_COMMAND;
            }
            else
                return  HOKUYO_STATUS_OK;
    }

    // check in the second line if the status of the sensor is ok
    if (current_line == 1)
    {
        if ((laser_mode == MD_MODE && (text_data[0] != '9' || text_data[1] != '9' || text_data[2] != 'b' )) ||
            (laser_mode == GD_MODE && (text_data[0] != '0' || text_data[1] != '0' || text_data[2] != 'P' )))
            {
                #ifdef LASER_DEBUG
                yDebug("Invalid sensor status: %s\n", text_data);
                #endif
                return HOKUYO_STATUS_ERROR_BUSY;
            }
            else
                return HOKUYO_STATUS_OK;
    }

    // verify the checksum for all the following lines
    if (current_line >= 2)
    {
        char expected_checksum = text_data[text_data_len - 2];
        if (calculateCheckSum(text_data, text_data_len - 2, expected_checksum) < 0)
        {
            #ifdef LASER_DEBUG
            yDebug("Checksum error, line: %d: %s\n", current_line, text_data);
            #endif
            return HOKUYO_STATUS_ERROR_INVALID_CHECKSUM;
        }
    }

    // read the sensor time stamp in the third line
    if (current_line == 2)
    {
//         timestamp = decodeDataValue(text_data, 4);
    }

    // read the data in the next lines
    if (current_line >= 3)
    {
        strncat (data_block, text_data, text_data_len-2 );
    }

    //increments the lines counter
    //current_line++;

    return HOKUYO_STATUS_OK;
}

void laserHokuyo::run()
{
    //send the GD command: get one single measurement, D precision
    Bottle b;
    Bottle b_ans;
    const int buffer_size = 128;
    char command [buffer_size];
    char answer  [buffer_size];
#ifdef LASER_DEBUG
    static double old;
#endif
    yarp::sig::Vector data_vector;

    string data_text;
    double t1 = yarp::os::SystemClock::nowSystem();
    double t2 = 0;
    bool timeout = false;
    bool rx_completed = false;
    bool error = false;
    int current_line =0;
    double maxtime=1;
    do
    {
        int answer_len = pSerial->receiveLine(answer, buffer_size);
        internal_status = readData(laser_mode, answer,answer_len,current_line,data_vector);
        if (internal_status <  0 && internal_status != HOKUYO_STATUS_ERROR_NOTHING_RECEIVED)
        {
            error = true;
        }
        if (internal_status == HOKUYO_STATUS_OK)
        {
            current_line++;
        }
        if (internal_status == HOKUYO_STATUS_ACQUISITION_COMPLETE)
        {
            rx_completed = true;
        }
        t2 = yarp::os::SystemClock::nowSystem();
        if (t2-t1>maxtime) timeout = true;
    }
    while (rx_completed == false && timeout == false && error == false);

    if (timeout)
    {
        yError ("laserHokuyo Timeout!");
    }
    if (error)
    {
        yError("laserHokuyo Communication Error, internal status=%d",internal_status);
    }
    #ifdef LASER_DEBUG
    yDebug ("time: %.3f %.3f\n",t2-t1, t2-old);
    old = t2;
    #endif

    mutex.lock();

    if (rx_completed)
    {
        laser_data=data_vector;
        // static int countt=0;
        // yDebug() << countt++ << getEstPeriod() << getEstUsed();
    }
    mutex.unlock();

    if (laser_mode==GD_MODE)
    {
        sprintf (command,"GD%04d%04d%02d\n",start_position,end_position,1);
        b.addString(command);
        pSerial->send(b);
    }

    //SystemClock::delaySystem (0.100);
}

void laserHokuyo::threadRelease()
{
#ifdef LASER_DEBUG
    yDebug("laserHokuyo Thread releasing...");
    yDebug("... done.");
#endif
}

bool laserHokuyo::getDeviceInfo(std::string &device_info)
{
    this->mutex.lock();
    device_info = info;
    this->mutex.unlock();
    return true;
}
