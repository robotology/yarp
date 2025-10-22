/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/ImplementAmplifierControl.h>
#include <yarp/dev/ControlBoardHelper.h>

#include <cmath>

// Be careful: this file contains template implementations and is included by translation
// units that use the template (e.g. .cpp files). Avoid putting here non-template functions to
// avoid multiple definitions.

using namespace yarp::dev;

ImplementAmplifierControl::ImplementAmplifierControl(yarp::dev::IAmplifierControlRaw  *y)
{
    iAmplifier= y;
    helper = nullptr;
    dTemp=nullptr;
    iTemp=nullptr;
}

ImplementAmplifierControl::~ImplementAmplifierControl()
{
    uninitialize();
}

bool ImplementAmplifierControl:: initialize (int size, const int *amap, const double *enc, const double *zos, const double *ampereFactor, const double *voltFactor)
{
    if (helper != nullptr) {
        return false;
    }

    helper=(void *)(new ControlBoardHelper(size, amap, enc, zos,nullptr, ampereFactor, voltFactor));
    yAssert (helper != nullptr);
    dTemp=new double[size];
    yAssert (dTemp != nullptr);
    iTemp=new int[size];
    yAssert (iTemp != nullptr);

    return true;
}

/**
* Clean up internal data and memory.
* @return true if uninitialization is executed, false otherwise.
*/
bool ImplementAmplifierControl::uninitialize ()
{
    if (helper != nullptr) {
        delete castToMapper(helper);
    }

    delete [] dTemp;
    delete [] iTemp;

    helper=nullptr;
    dTemp=nullptr;
    iTemp=nullptr;
    return true;
}

ReturnValue ImplementAmplifierControl::enableAmp(int j)
{
    int k=castToMapper(helper)->toHw(j);

    return iAmplifier->enableAmpRaw(k);
}

ReturnValue ImplementAmplifierControl::disableAmp(int j)
{
    int k=castToMapper(helper)->toHw(j);

    return iAmplifier->disableAmpRaw(k);
}

ReturnValue ImplementAmplifierControl::getCurrents(double *currs)
{
    ReturnValue ret=iAmplifier->getCurrentsRaw(dTemp);
    castToMapper(helper)->ampereS2A(dTemp, currs);
    return ret;
}

ReturnValue ImplementAmplifierControl::getCurrent(int j, double *c)
{
    double temp = 0;
    int k = castToMapper(helper)->toHw(j);
    ReturnValue ret = iAmplifier->getCurrentRaw(k, &temp);
    castToMapper(helper)->ampereS2A(temp, k, *c, j);
    return ret;
}

ReturnValue ImplementAmplifierControl::getAmpStatus(int *st)
{
    ReturnValue ret=iAmplifier->getAmpStatusRaw(iTemp);
    castToMapper(helper)->toUser(iTemp, st);

    return ret;
}

ReturnValue ImplementAmplifierControl::getAmpStatus(int k, int *st)
{
    int j=castToMapper(helper)->toHw(k);
    ReturnValue ret=iAmplifier->getAmpStatusRaw(j, st);

    return ret;
}

ReturnValue ImplementAmplifierControl::setMaxCurrent(int m, double v)
{
    int k;
    double curr;
    castToMapper(helper)->ampereA2S(v, m, curr, k);
    return iAmplifier->setMaxCurrentRaw(k, curr);
}

ReturnValue ImplementAmplifierControl::getMaxCurrent(int j, double* v)
{
    double val;
    int k=castToMapper(helper)->toHw(j);
    ReturnValue ret = iAmplifier->getMaxCurrentRaw(k, &val);
    *v = castToMapper(helper)->ampereS2A(val, k);
    return ret;
}

ReturnValue ImplementAmplifierControl::getNominalCurrent(int m, double *curr)
{
    int k;
    ReturnValue ret;
    double tmp;

    k=castToMapper(helper)->toHw(m);
    ret=iAmplifier->getNominalCurrentRaw(k, &tmp);
    *curr=castToMapper(helper)->ampereS2A(tmp, k);
    return ret;
}

ReturnValue ImplementAmplifierControl::getPeakCurrent(int m, double *curr)
{
    int k;
    ReturnValue ret;
    double tmp;

    k=castToMapper(helper)->toHw(m);
    ret=iAmplifier->getPeakCurrentRaw(k, &tmp);
    *curr=castToMapper(helper)->ampereS2A(tmp, k);
    return ret;
}

ReturnValue ImplementAmplifierControl::setPeakCurrent(int m, const double curr)
{
    int k;
    double val;
    castToMapper(helper)->ampereA2S(curr, m, val, k);
    return iAmplifier->setPeakCurrentRaw(k, val);
}

ReturnValue ImplementAmplifierControl::setNominalCurrent(int m, const double curr)
{
    int k;
    double val;
    castToMapper(helper)->ampereA2S(curr, m, val, k);
    return iAmplifier->setNominalCurrentRaw(k, val);
}

ReturnValue ImplementAmplifierControl::getPWM(int m, double* pwm)
{
    int k;
    k=castToMapper(helper)->toHw(m);
    return iAmplifier->getPWMRaw(k, pwm);
}

ReturnValue ImplementAmplifierControl::getPWMLimit(int m, double* limit)
{
    int k;
    k=castToMapper(helper)->toHw(m);
    return iAmplifier->getPWMLimitRaw(k, limit);
}

ReturnValue ImplementAmplifierControl::setPWMLimit(int m, const double limit)
{
    int k;
    k=castToMapper(helper)->toHw(m);
    return iAmplifier->setPWMLimitRaw(k, limit);
}

ReturnValue ImplementAmplifierControl::getPowerSupplyVoltage(int m, double *voltage)
{
    int k;
    k=castToMapper(helper)->toHw(m);
    return iAmplifier->getPowerSupplyVoltageRaw(k, voltage);
}
