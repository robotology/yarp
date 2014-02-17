/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <yarp/manager/data.h>

using namespace yarp::manager;

/**
 * Class InputData
 */
InputData::InputData(void) : Node(INPUTD)
{
    bRequired = false;
    bWithPriority = false;
    modOwner = NULL;
    portType = STREAM_PORT;
}

InputData::InputData(const char* szName) : Node(INPUTD)
{
    bRequired = false;
    bWithPriority = false;
    modOwner = NULL;
    setName(szName);
    portType = STREAM_PORT;
}

InputData::InputData(const InputData &input) : Node(input)
{
    strName = input.strName;
    strPort = input.strPort;
    carrier = input.carrier;
    bWithPriority = input.bWithPriority;
    bRequired = input.bRequired;
    strDescription = input.strDescription;
    modOwner = input.modOwner;
    portType = input.portType;
}


InputData::~InputData() { }


Node* InputData::clone(void)
{
    InputData* input = new InputData(*this);
    return input;
}




/**
 * Class OutputData
 */
OutputData::OutputData(void) : Node(OUTPUTD)
{
    modOwner = NULL;
    portType = STREAM_PORT;
}


OutputData::OutputData(const char* szName) : Node(OUTPUTD)
{
    modOwner = NULL;
    setName(szName);
    portType = STREAM_PORT;
}


OutputData::OutputData(const OutputData &output) : Node(output)
{
    strName = output.strName;
    strPort = output.strPort;
    carrier = output.carrier;
    strDescription = output.strDescription;
    modOwner = output.modOwner;
    portType = output.portType;
}


OutputData::~OutputData() { }


Node* OutputData::clone(void)
{
    OutputData* output = new OutputData(*this);
    return output;
}
