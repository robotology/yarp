/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/ImplementJointFault.h>
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/dev/impl/FixedSizeBuffersManager.h>

#include <cstdio>
using namespace yarp::dev;
using namespace yarp::os;

ImplementJointFault::ImplementJointFault(IJointFaultRaw *r):
    m_helper(nullptr),
    m_iraw(r)
{}

bool ImplementJointFault::initialize(int size, const int *amap)
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

ImplementJointFault::~ImplementJointFault()
{
    uninitialize();
}

bool ImplementJointFault::uninitialize ()
{
    if (m_helper!=nullptr)
    {
        delete castToMapper(m_helper);
        m_helper=nullptr;
    }

    return true;
}

ReturnValue ImplementJointFault::getLastJointFault(int j, int& fault, std::string& message)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    int k=castToMapper(m_helper)->toHw(j);
    return m_iraw->getLastJointFaultRaw(k, fault, message);
}
