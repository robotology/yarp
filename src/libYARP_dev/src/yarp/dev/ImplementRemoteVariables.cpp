/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "yarp/dev/ControlBoardInterfacesImpl.h"
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/dev/ControlBoardHelpers.h>

#include <cstdio>
using namespace yarp::dev;

////////////////////////
// Encoder Interface Timed Implementation
ImplementRemoteVariables::ImplementRemoteVariables(IRemoteVariablesRaw *y)
{
    m_iraw=y;
    m_helper = nullptr;
}

ImplementRemoteVariables::~ImplementRemoteVariables()
{
    uninitialize();
}

bool ImplementRemoteVariables::initialize(int size, const int *amap)
{
    if (m_helper != nullptr) {
        return false;
    }

    m_helper=(void *)(new ControlBoardHelper(size, amap));
    yAssert (m_helper != nullptr);

    m_buffer_ints.resize   (size);
    m_buffer_doubles.resize(size);

    return true;
}

/**
* Clean up internal data and memory.
* @return true if uninitialization is executed, false otherwise.
*/
bool ImplementRemoteVariables::uninitialize()
{
    if (m_helper!=nullptr)
    {
        delete castToMapper(m_helper);
        m_helper=nullptr;
    }

    return true;
}

ReturnValue ImplementRemoteVariables::getRemoteVariable(std::string key, yarp::os::Bottle& val)
{
    std::lock_guard lock(m_imp_mutex);

    ReturnValue ret;
    ret = m_iraw->getRemoteVariableRaw(key, val);
    return ret;
}

ReturnValue ImplementRemoteVariables::setRemoteVariable(std::string key, const yarp::os::Bottle& val)
{
    std::lock_guard lock(m_imp_mutex);

    ReturnValue ret;
    ret = m_iraw->setRemoteVariableRaw(key, val);
    return ret;
}

ReturnValue ImplementRemoteVariables::getRemoteVariablesList(yarp::os::Bottle* listOfKeys)
{
    std::lock_guard lock(m_imp_mutex);
    POINTERCHECK(listOfKeys);

    ReturnValue ret;
    ret = m_iraw->getRemoteVariablesListRaw(listOfKeys);
    return ret;
}
