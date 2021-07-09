/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
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


InputData::~InputData() = default;


Node* InputData::clone()
{
    auto* input = new InputData(*this);
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


OutputData::~OutputData() = default;


Node* OutputData::clone()
{
    auto* output = new OutputData(*this);
    return output;
}
