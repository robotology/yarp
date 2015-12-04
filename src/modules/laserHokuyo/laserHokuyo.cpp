// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2010 Marco Randazzo
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */
 
// ********************************************************
// *** THIS FILE IS CURRENTLY UNDER DEVELOPMENT / DEBUG ***
// ********************************************************

#include <laserHokuyo.h>

#include <yarp/os/Time.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>

//#define LASER_DEBUG 1

using namespace std;

bool laserHokuyo::open(yarp::os::Searchable& config)
{
    bool correct=true;
    internal_status = STATUS_NOT_READY;
    info = "Hokuyo Laser";
    device_status = LASER_OK_STANBY;

#if LASER_DEBUG
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
    fake = general_config.check("fake");
    period = general_config.check("Period", Value(50), "Period of the sampling thread").asInt();
    start_position = general_config.check("Start_Position", Value(0), "Start position").asInt();
    end_position = general_config.check("End_Position", Value(1080), "End Position").asInt();
    error_codes = general_config.check("Convert_Error_Codes", Value(0), "Substitute error codes with legal measurments").asInt();
    yarp::os::ConstString s = general_config.check("Laser_Mode", Value("GD"), "Laser Mode (GD/MD").asString();

    yarp::os::ConstString u = general_config.check("Measurement_Units", Value("M"), "Measurment units (M/MM/INCH/FEET").asString();
    if      (u=="MM")   measurement_units= UNITS_MM;
    else if (u=="M")    measurement_units= UNITS_M;
    else if (u=="INCH") measurement_units= UNITS_INCH;
    else if (u=="FEET") measurement_units= UNITS_FEET;
    else                measurement_units= UNITS_M;


    if (error_codes==1)
    {
        yInfo("'error_codes' option enabled. Invalid samples will be substituded with out-of-range measurements.");
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

    if (fake)
    {
        sensorsNum=16*12;
        laser_data.resize(sensorsNum);
        laser_mode = FAKE_MODE;
        yInfo("fake option found. Entering debug mode: Using recorded data, not the real sensor");
        Time::turboBoost();
        RateThread::start();
        return true;
    }

    bool ok = general_config.check("Serial_Configuration");
    if (!ok)
    {
        yError("Cannot find configuration file for serial port communication!");
        return false;
    }
    yarp::os::ConstString serial_filename = general_config.find("Serial_Configuration").asString();

    //string st = config.toString();
    setRate(period);

    Property prop;

    prop.put("device", "serialport");
    ok = prop.fromConfigFile(serial_filename.c_str(),config,false);
    if (!ok)
    {
        yError("Unable to read from serial port configuration file");
        return false;
    }

    pSerial=0;

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

    Bottle b;
    Bottle b_ans;
    string ans;

    // *** Check if the URG device is present ***
    b.addString("SCIP2.0\n");
    pSerial->send(b);
    yarp::os::Time::delay(0.040);
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
    yarp::os::Time::delay(0.040);
    pSerial->receive(b_ans);
    ans = b_ans.get(0).asString();
    yDebug("%s\n",ans.c_str());
    b.clear();
    b_ans.clear();*/

    // *** Read the firmware version parameters ***
    b.addString("VV\n");
    pSerial->send(b);
    yarp::os::Time::delay(0.040);
    pSerial->receive(b_ans);
    ans = b_ans.get(0).asString();
    yDebug("%s\n",ans.c_str());
    b.clear();
    b_ans.clear();

    // *** Read the sensor specifications ***
    b.addString("II\n");
    pSerial->send(b);
    yarp::os::Time::delay(0.040);
    pSerial->receive(b_ans);
    ans = b_ans.get(0).asString();
    yDebug("%s\n", ans.c_str());
    b.clear();
    b_ans.clear();

    // *** Read the URG configuration parameters ***
    b.addString("PP\n");
    pSerial->send(b);
    yarp::os::Time::delay(0.040);
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
    yarp::os::Time::delay(0.040);
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

    Time::turboBoost();
    RateThread::start();
    return true;
}

bool laserHokuyo::close()
{
    RateThread::stop();

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

int laserHokuyo::getRangeData(yarp::sig::Vector &out)
{
    if (internal_status!=STATUS_NOT_READY)
    {
        mutex.wait();
    #if LASER_DEBUG
        //yDebug("data: %s\n", laser_data.toString().c_str());
    #endif
        out=laser_data;
        mutex.post();
        return yarp::dev::ILaserRangefinder2D::LASER_OK_IN_USE;
    }
    else
        return yarp::dev::ILaserRangefinder2D::LASER_GENERAL_ERROR;
}

bool laserHokuyo::getDeviceStatus(int &status)
{
    mutex.wait();
    status = device_status;
    mutex.post();
    return true; 
}

bool laserHokuyo::threadInit()
{
#if LASER_DEBUG
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

int laserHokuyo::readData(const laser_mode_type laser_mode, const char* text_data, const int text_data_len, int current_line, yarp::sig::Vector& data)
{ 
    static char data_block [4000];

    if (text_data_len==0)
    {
        return  STATUS_ERROR_NOTHING_RECEIVED; 
    }

    long int timestamp = 0 ;

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
                switch (measurement_units)
                {
                    case UNITS_M:
                        data.push_back(value/1000.0); break;
                    case UNITS_MM:
                        data.push_back(value); break;
                    case UNITS_INCH:
                        data.push_back(value/25.4); break;
                    case UNITS_FEET:
                        data.push_back(value/304.8); break;
                    default:
                        data.push_back(value/1000.0); break;
                }
            }
            return STATUS_ACQUISITION_COMPLETE; 
        }

    // check in the first line if it is a valid answer to GD command
    if (current_line == 0) 
    {
        data_block[0]='\0'; //resets the datablock; 
        if ((laser_mode == MD_MODE && (text_data[0] != 'M' || text_data[1] != 'D')) ||
            (laser_mode == GD_MODE && (text_data[0] != 'G' || text_data[1] != 'D')))
            {
                #if LASER_DEBUG
                yDebug("Invalid answer to a MD command: %s\n", text_data);
                #endif
                return STATUS_ERROR_INVALID_COMMAND; 
            }
            else
                return  STATUS_OK; 
    }

    // check in the second line if the status of the sensor is ok
    if (current_line == 1)
    {
        if ((laser_mode == MD_MODE && (text_data[0] != '9' || text_data[1] != '9' || text_data[2] != 'b' )) ||
            (laser_mode == GD_MODE && (text_data[0] != '0' || text_data[1] != '0' || text_data[2] != 'P' )))
            {
                #if LASER_DEBUG
                yDebug("Invalid sensor status: %s\n", text_data);
                #endif
                return STATUS_ERROR_BUSY; 
            }
            else
                return STATUS_OK; 
    }

    // verify the checksum for all the following lines
    if (current_line >= 2)
    {
        char expected_checksum = text_data[text_data_len - 2];
        if (calculateCheckSum(text_data, text_data_len - 2, expected_checksum) < 0)
        {
            #if LASER_DEBUG
            yDebug("Cheksum error, line: %d: %s\n", current_line, text_data);
            #endif
            return STATUS_ERROR_INVALID_CHECKSUM; 
        }
    }

    // read the sensor time stamp in the third line
    if (current_line == 2)
    {
        timestamp = decodeDataValue(text_data, 4);
    }

    // read the data in the next lines
    if (current_line >= 3)
    {
        strncat (data_block, text_data, text_data_len-2 );
    }

    //increments the lines counter
    //current_line++;

  return STATUS_OK;
}

void laserHokuyo::run()
{
    if (fake)
    {
        mutex.wait();
        internal_status=STATUS_OK;
        laser_data.clear();
        for (int i=0; i<1080; i++)
            laser_data.push_back(i/100.0);
        mutex.post();
        return;
    }

    mutex.wait();

    //send the GD command: get one single measurement, D precision
    Bottle b;
    Bottle b_ans;
    const int buffer_size = 128;
    char command [buffer_size];
    char answer  [buffer_size];
    static double old;
    yarp::sig::Vector data_vector;
    int data_vector_size = 0; 

    char last_char1 = 0;
    char last_char2 =0;
    string data_text;
    double t1 = yarp::os::Time::now();
    double t2 = 0;
    bool timeout = false;
    bool rx_completed = false;
    bool error = false;
    int current_line =0;
    double maxtime=1;
    do 
    {
        //yDebug ("1status: %d!\n",internal_status);
        int answer_len = pSerial->receiveLine(answer, buffer_size);
        internal_status = readData(laser_mode, answer,answer_len,current_line,data_vector);
        if (internal_status <  0 && internal_status != STATUS_ERROR_NOTHING_RECEIVED) 
            error = true;
        if (internal_status == STATUS_OK) current_line ++;
        if (internal_status == STATUS_ACQUISITION_COMPLETE) rx_completed = true;
        t2 = yarp::os::Time::now();
        if (t2-t1>maxtime) timeout = true;
    }
    while (rx_completed == false && timeout == false && error == false);

    if (timeout)
    {
        yError ("Timeout!");
    }
    if (error)
    {
        yError("Communication Error!");
    }

    #if LASER_DEBUG
    yDebug ("time: %.3f %.3f\n",t2-t1, t2-old);
    #endif
    old = t2;

    if (rx_completed)
    {
        //data_vector_size = data_vector.size();
        laser_data=data_vector;
    }

    if (laser_mode==GD_MODE)
    {
        sprintf (command,"GD%04d%04d%02d\n",start_position,end_position,1);
        b.addString(command);
        pSerial->send(b);
    }

    //Time::delay (0.100);
    mutex.post();
}

void laserHokuyo::threadRelease()
{
#if LASER_DEBUG
    yDebug("laserHokuyo Thread releasing...");
    yDebug("... done.");
#endif
}

bool laserHokuyo::getDeviceInfo(yarp::os::ConstString &device_info)
{
    this->mutex.wait();
    device_info = info;
    this->mutex.post();
    return true;
}
