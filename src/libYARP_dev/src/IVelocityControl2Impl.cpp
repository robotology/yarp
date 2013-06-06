// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Alberto Cardellino
 * email:   alberto.cardellino@iit.it
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 */


#include <stdio.h>

#include <yarp/dev/ControlBoardInterfacesImpl.h>
#include <yarp/dev/ControlBoardHelper.h>

using namespace yarp::dev;

ImplementVelocityControl2::ImplementVelocityControl2(IVelocityControl2Raw *y)
{
    iVelocity2 = y;
    helper = 0;
    temp_double = 0;
    temp_int = 0;
}

ImplementVelocityControl2::~ImplementVelocityControl2()
{
    uninitialize();
}

bool ImplementVelocityControl2::initialize(int size, const int *axis_map, const double *enc, const double *zeros)
{
  if (helper!=0)
    return false;

  helper=(void *)(new ControlBoardHelper(size, axis_map, enc, zeros,0));
  _YARP_ASSERT (helper != 0);
  temp_double = new double [size];
  _YARP_ASSERT (temp_double != 0);
  temp_int = new int [size];
  _YARP_ASSERT (temp_int != 0);
  tempPids = new Pid [size];
  _YARP_ASSERT (tempPids != 0);

  return true;
}

bool ImplementVelocityControl2::uninitialize()
{
    if(helper!=0)
    {
        delete castToMapper(helper);
        helper=0;
    }
    checkAndDestroy(temp_double);
    checkAndDestroy(temp_int);
    checkAndDestroy(tempPids);

    return true;
}

bool ImplementVelocityControl2::getAxes(int *ax)
{
  (*ax)=castToMapper(helper)->axes();
  return true;
}


bool ImplementVelocityControl2::setVelocityMode()
{
  return iVelocity2->setVelocityModeRaw();
}

bool ImplementVelocityControl2::setVelocityMode(const int n_joint, const int *joints)
{
  int j;
  for(int idx=0; idx<n_joint; idx++)
  {
      j = joints[idx];
      temp_int[idx] = castToMapper(helper)->toHw(j);
  }
  return iVelocity2->setVelocityModeRaw(n_joint, temp_int);
}


bool ImplementVelocityControl2::velocityMove(int j, double sp)
{
  int k;
  double enc;
  castToMapper(helper)->velA2E(sp, j, enc, k);
  return iVelocity2->velocityMoveRaw(k, enc);
}

bool ImplementVelocityControl2::velocityMove(const int n_joint, const int *joints, const double *spds)
{
  for(int idx=0; idx<n_joint; idx++)
  {
      castToMapper(helper)->velA2E(spds[idx], joints[idx], temp_double[idx], temp_int[idx]);
  }
  return iVelocity2->velocityMoveRaw(n_joint, temp_int, temp_double);
}

bool ImplementVelocityControl2::velocityMove(const double *sp)
{
  castToMapper(helper)->velA2E(sp, temp_double);
  return iVelocity2->velocityMoveRaw(temp_double);
}

bool ImplementVelocityControl2::setRefAcceleration(int j, double acc)
{
  int k;
  double enc;
  castToMapper(helper)->accA2E_abs(acc, j, enc, k);
  return iVelocity2->setRefAccelerationRaw(k, enc);
}

bool ImplementVelocityControl2::setRefAccelerations(const int n_joint, const int *joints, const double *accs)
{
  for(int idx=0; idx<n_joint; idx++)
  {
      castToMapper(helper)->accA2E_abs(accs[idx], joints[idx], temp_double[idx], temp_int[idx]);
  }
  return iVelocity2->setRefAccelerationsRaw(n_joint, temp_int, temp_double);
}

bool ImplementVelocityControl2::setRefAccelerations(const double *accs)
{
  castToMapper(helper)->accA2E_abs(accs, temp_double);
  return iVelocity2->setRefAccelerationsRaw(temp_double);
}

bool ImplementVelocityControl2::getRefAcceleration(int j, double *acc)
{
  int k;
  double enc;
  k=castToMapper(helper)->toHw(j);
  bool ret = iVelocity2->getRefAccelerationRaw(k, &enc);
  *acc=castToMapper(helper)->accE2A_abs(enc, k);
  return ret;
}


bool ImplementVelocityControl2::getRefAccelerations(const int n_joint, const int *joints, double *accs)
{
    for(int idx=0; idx<n_joint; idx++)
    {
        temp_int[idx]=castToMapper(helper)->toHw(joints[idx]);
    }

    bool ret = iVelocity2->getRefAccelerationsRaw(n_joint, temp_int, temp_double);

    for(int idx=0; idx<n_joint; idx++)
    {
        accs[idx]=castToMapper(helper)->accE2A_abs(temp_double[idx], temp_int[idx]);
    }
    return ret;
}


bool ImplementVelocityControl2::getRefAccelerations(double *accs)
{
  bool ret=iVelocity2->getRefAccelerationsRaw(temp_double);
  castToMapper(helper)->accE2A_abs(temp_double, accs);
  return ret;
}


bool ImplementVelocityControl2::stop(int j)
{
  int k;
  k=castToMapper(helper)->toHw(j);
  return iVelocity2->stopRaw(k);
}


bool ImplementVelocityControl2::stop(const int n_joint, const int *joints)
{
  for(int idx=0; idx<n_joint; idx++)
  {
      temp_int[idx] = castToMapper(helper)->toHw(joints[idx]);
  }
  return iVelocity2->stopRaw(n_joint, temp_int);
}


bool ImplementVelocityControl2::stop()
{
  return iVelocity2->stopRaw();
}


bool ImplementVelocityControl2::setVelPid(int j, const Pid &pid)
{
  int k=castToMapper(helper)->toHw(j);
  return iVelocity2->setVelPidRaw(k,pid);
}


bool ImplementVelocityControl2::setVelPids(const int n_joint, const int *joints, const Pid *pids)
{
    for(int idx=0; idx<n_joint; idx++)
    {
      temp_int[idx] = castToMapper(helper)->toHw(joints[idx]);
//      tempPids[idx]=pids[temp_int[idx]]; no need to changes here
    }
    return iVelocity2->setVelPidsRaw(n_joint, temp_int, tempPids);
}


bool ImplementVelocityControl2::setVelPids(const Pid *pids)
{
  int tmp=0;
  int nj=castToMapper(helper)->axes();

  for(int j=0;j<nj;j++)
  {
      tmp=castToMapper(helper)->toHw(j);
      tempPids[tmp]=pids[j];     // here the conversion consists into reordering the Pids array
  }

  return iVelocity2->setVelPidsRaw(tempPids);
}


bool ImplementVelocityControl2::getVelPid(int j, Pid *pid)
{
    int k;
    k=castToMapper(helper)->toHw(j);

    return iVelocity2->getVelPidRaw(k, pid);
}


bool ImplementVelocityControl2::getVelPids(const int n_joint, const int *joints, Pid *pids)
{
    for(int idx=0; idx<n_joint; idx++)
    {
        temp_int[idx]=castToMapper(helper)->toHw(joints[idx]);
    }

    bool ret = iVelocity2->getVelPidsRaw(n_joint, temp_int, pids);
    return ret;
}


bool ImplementVelocityControl2::getVelPids(Pid *pids)
{
    bool ret=iVelocity2->getVelPidsRaw(tempPids);
    int nj=castToMapper(helper)->axes();

    for(int j=0;j<nj;j++)
        pids[castToMapper(helper)->toUser(j)]=tempPids[j];

    return ret;
}


bool ImplementVelocityControl2::getVelError(int j, double *err)
{
    int k;
    double enc;
    k=castToMapper(helper)->toHw(j);

    bool ret=iVelocity2->getVelErrorRaw(k, &enc);

    *err=castToMapper(helper)->velE2A(enc, k);

    return ret;
}

bool ImplementVelocityControl2::getVelErrors(const int n_joint, const int *joints, double *errs)
{
    for(int idx=0; idx<n_joint; idx++)
    {
        temp_int[idx]=castToMapper(helper)->toHw(joints[idx]);
    }

    bool ret = iVelocity2->getVelErrorsRaw(n_joint, temp_int, temp_double);

    for(int idx=0; idx<n_joint; idx++)
    {
        errs[idx]=castToMapper(helper)->velE2A(temp_double[idx], temp_int[idx]);
    }
    return ret;
}

bool ImplementVelocityControl2::getVelErrors(double *errs)
{
    bool ret;
    ret=iVelocity2->getVelErrorsRaw(temp_double);

    castToMapper(helper)->velE2A(temp_double, errs);

    return ret;
}


