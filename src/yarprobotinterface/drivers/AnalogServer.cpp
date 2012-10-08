/*
 * Copyright (C) 2012 iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Alberto Cardellino, Daniele E. Domenichelli
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "AnalogServer.h"

#include <yarp/os/Bottle.h>
#include <yarp/os/BufferedPort.h>

#include <yarp/dev/IAnalogSensor.h>
#include <yarp/dev/ControlBoardInterfaces.h>

#include <iostream>


AnalogServerHandler::AnalogServerHandler(const char* n)
{
    rpcPort.open(n);
    rpcPort.setReader(*this);
}

AnalogServerHandler::~AnalogServerHandler()
{
    rpcPort.close();
    is = 0;
}

void AnalogServerHandler::setInterface(yarp::dev::IAnalogSensor *is)
{
    this->is = is;
}

bool AnalogServerHandler::_handleIAnalog(yarp::os::Bottle &cmd, yarp::os::Bottle &reply)
{
    if (is==0)
        return false;

    int msgsize=cmd.size();

    int code=cmd.get(1).asVocab();
    switch (code)
    {
        case VOCAB_CALIBRATE:
            if (msgsize==2)
                is->calibrateSensor();
            else
            {
                //read Vector of values and pass to is->calibrate();
            }
            return true;
            break;
        case VOCAB_CALIBRATE_CHANNEL:
            if (msgsize==3)
            {
                int ch=cmd.get(2).asInt();
                is->calibrateChannel(ch);
            }
            if (msgsize==4)
            {
                int ch=cmd.get(2).asInt();
                double v=cmd.get(3).asDouble();
                is->calibrateChannel(ch, v);
            }

            return true;
            break;
        default:
            return false;
    }
}

bool AnalogServerHandler::read(yarp::os::ConnectionReader& connection)
{
    yarp::os::Bottle in;
    yarp::os::Bottle out;
    bool ok=in.read(connection);
    if (!ok)
        return false;

    // parse in, prepare out
    int code = in.get(0).asVocab();
    bool ret=false;
    if (code==VOCAB_IANALOG)
    {
        ret=_handleIAnalog(in, out);
    }

    if (!ret)
    {
        out.clear();
        out.addVocab(VOCAB_FAILED);
    }

    yarp::os::ConnectionWriter *returnToSender = connection.getWriter();
    if (returnToSender!=NULL) {
        out.write(*returnToSender);
    }
    return true;
}


AnalogPortEntry::AnalogPortEntry()
{
}

AnalogPortEntry::AnalogPortEntry(const AnalogPortEntry& alt)
{
    this->length = alt.length;
    this->offset = alt.offset;
    this->port_name = alt.port_name;
}

AnalogPortEntry& AnalogPortEntry::operator=(const AnalogPortEntry& alt)
{
    if (this != &alt)
    {
        this->length = alt.length;
        this->offset = alt.offset;
        this->port_name = alt.port_name;
    }
    return *this;
}



AnalogServer::AnalogServer(const char* name, int rate) :
        RateThread(rate)
{
    is=0;
    analogPorts.resize(1);
    analogPorts[0].offset = 0;
    analogPorts[0].length = -1; // max length
    analogPorts[0].port_name = std::string(name);
    setHandlers();
}

AnalogServer::AnalogServer(const std::vector<AnalogPortEntry> &_analogPorts, int rate) :
        RateThread(rate)
{
    is=0;
    this->analogPorts=_analogPorts;
    setHandlers();
}

AnalogServer::~AnalogServer()
{
    threadRelease();
    is=0;
}

void AnalogServer::attach(yarp::dev::IAnalogSensor* s)
{
    is=s;
    for(unsigned int i=0;i<analogPorts.size(); i++){
        handlers[i]->setInterface(is);
    }
}

bool AnalogServer::threadInit()
{
    for(unsigned int i=0; i<analogPorts.size(); i++){
        // open data port
        if (!analogPorts[i].port.open(analogPorts[i].port_name.c_str()))
            return false;
    }
    return true;
}


void AnalogServer::threadRelease()
{
    for(unsigned int i=0; i<analogPorts.size(); i++){
        analogPorts[i].port.close();
    }
}

void AnalogServer::run()
{
    int first, last, ret;
    if (is!=0)
    {
        // read from the analog sensor
        yarp::sig::Vector v;

        ret=is->read(v);

        if (ret==yarp::dev::IAnalogSensor::AS_OK)
        {
            if (v.size()>0)
            {
                lastStateStamp.update();
                // send the data on the port(s), splitting them as specified in the config file
                for(unsigned int i=0; i<analogPorts.size(); i++){
                    yarp::sig::Vector &pv = analogPorts[i].port.prepare();
                    first = analogPorts[i].offset;
                    if(analogPorts[i].length==-1)   // read the max length available
                        last = v.size()-1;
                    else
                        last = analogPorts[i].offset + analogPorts[i].length - 1;
                    // check vector limit
                    if(last>=(int)v.size()){
                        std::cerr << "Error while sending analog sensor output on port " << analogPorts[i].port_name << std::endl;
                        std::cerr <<"Vector size expected to be at least "<< last <<" whereas it is " << v.size() << std::endl;
                        continue;
                    }
                    pv = v.subVector(first, last);
                    analogPorts[i].port.setEnvelope(lastStateStamp);
                    analogPorts[i].port.write();
                }
            }
        }
        else
        {
            // TODO release
        }
    }
}

void AnalogServer::setHandlers()
{
    for(unsigned int i=0;i<analogPorts.size(); i++){
        std::string rpcPortName = analogPorts[i].port_name;
        rpcPortName += "/rpc:i";
        AnalogServerHandler* ash = new AnalogServerHandler(rpcPortName.c_str());
        handlers.push_back(ash);
    }
}
