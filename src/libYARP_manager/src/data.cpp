/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/manager/data.h>

using namespace yarp::manager;

/**
 * Class InputData
 */
InputData::InputData() : Node(INPUTD)
{
    bRequired = false;
    bWithPriority = false;
    modOwner = nullptr;
    portType = STREAM_PORT;
}

InputData::InputData(const char* szName) : Node(INPUTD)
{
    bRequired = false;
    bWithPriority = false;
    modOwner = nullptr;
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


Node* InputData::clone()
{
    InputData* input = new InputData(*this);
    return input;
}




/**
 * Class OutputData
 */
OutputData::OutputData() : Node(OUTPUTD)
{
    modOwner = nullptr;
    portType = STREAM_PORT;
}


OutputData::OutputData(const char* szName) : Node(OUTPUTD)
{
    modOwner = nullptr;
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


Node* OutputData::clone()
{
    OutputData* output = new OutputData(*this);
    return output;
}
