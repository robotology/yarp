/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <cstdio>

#include <yarp/dev/ImplementImpedanceControl.h>
#include <yarp/dev/ControlBoardHelper.h>
#include <yarp/dev/ControlBoardHelpers.h>
#include <yarp/os/LogStream.h>
#include <cmath>

using namespace yarp::dev;

/////////////// implement ImplementImpedanceControl
ImplementImpedanceControl::ImplementImpedanceControl(IImpedanceControlRaw *r)
{
    m_iraw=r;
    m_helper=nullptr;
}

bool ImplementImpedanceControl::initialize(int size, const int *amap, const double *enc, const double *zos, const double *nw)
{
    if (m_helper != nullptr) {
        return false;
    }

    m_helper=(void *)(new ControlBoardHelper(size, amap, enc, zos, nw));
    yAssert (m_helper != nullptr);

    m_buffer_ints.resize   (size);
    m_buffer_doubles.resize(size);

    return true;
}

ImplementImpedanceControl::~ImplementImpedanceControl()
{
    uninitialize();
}

bool ImplementImpedanceControl::uninitialize ()
{
    if (m_helper!=nullptr)
    {
        delete castToMapper(m_helper);
        m_helper=nullptr;
    }

    return true;
}

ReturnValue ImplementImpedanceControl::getAxes(int *axes)
{
    POINTERCHECK(axes)
    return m_iraw->getAxes(axes);
}

ReturnValue ImplementImpedanceControl::setImpedance(int j, double stiffness, double damping)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    int k;
    double stiff;
    double damp;
    castToMapper(m_helper)->impN2S(stiffness,j,stiff,k);
    castToMapper(m_helper)->impN2S(damping,j,damp,k);
    return m_iraw->setImpedanceRaw(k, stiff, damp);
}

ReturnValue ImplementImpedanceControl::getImpedance(int j, double *stiffness, double *damping)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)
    POINTERCHECK(stiffness)
    POINTERCHECK(damping)

    int k;
    k=castToMapper(m_helper)->toHw(j);
    ReturnValue ret=m_iraw->getImpedanceRaw(k, stiffness, damping);
    *stiffness = (castToMapper(m_helper)->impS2N(*stiffness, k));
    *damping   = (castToMapper(m_helper)->impS2N(*damping, k));
    //prevent negative stiffness
    *stiffness = fabs (*stiffness);
    *damping   = fabs (*damping);
    return ret;
}

ReturnValue ImplementImpedanceControl::setImpedanceOffset(int j, double offset)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)

    int k;
    double off;
    castToMapper(m_helper)->trqN2S(offset,j,off,k);
    return m_iraw->setImpedanceOffsetRaw(k, off);
}

ReturnValue ImplementImpedanceControl::getImpedanceOffset(int j, double *offset)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)
    POINTERCHECK(offset)

    int k;
    k=castToMapper(m_helper)->toHw(j);
    ReturnValue ret = m_iraw->getImpedanceOffsetRaw(k, offset);
    *offset    = (castToMapper(m_helper)->trqS2N(*offset,k));
    return ret;
}

ReturnValue ImplementImpedanceControl::getCurrentImpedanceLimit(int j, double *min_stiff, double *max_stiff, double *min_damp, double *max_damp)
{
    std::lock_guard lock(m_imp_mutex);
    JOINTIDCHECK(j)
    POINTERCHECK(min_stiff)
    POINTERCHECK(max_stiff)
    POINTERCHECK(min_damp)
    POINTERCHECK(max_damp)

    int k;
    k=castToMapper(m_helper)->toHw(j);
    return m_iraw->getCurrentImpedanceLimitRaw(k, min_stiff, max_stiff, min_damp, max_damp);
}
