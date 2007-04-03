// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Lorenzo Natale
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <yarp/os/Semaphore.h>
#include <yarp/math/Rand.h>
#include <time.h>
#include <stdio.h>
#include <math.h>

//constants, from NR
#define IA 16807
#define IM 2147483647
#define AM (1.0/IM)
#define IQ 127773
#define IR 2836
#define NDIV (1+(IM-1)/NTAB)
#define EPS 1.2e-7
#define RNMX (1.0-EPS)

using namespace yarp;
using namespace yarp::sig;
using namespace yarp::math;

class ThreadSafeRandScalar : public RandScalar
{
    yarp::os::Semaphore mutex;
public:
    ThreadSafeRandScalar(): RandScalar()
    {
        
    }

    void init()        
    {
        mutex.wait();
        RandScalar::init();
        mutex.post();
    }

    void init(int s)
    {
        mutex.wait();
        RandScalar::init(s);
        mutex.post();
    }
    
    double get(double min=0.0, double max=1.0)
    {
        double ret;
        mutex.wait();
        ret=RandScalar::get(min, max);
        mutex.post();
        return ret;
    }

} theRandScalar;

double Rand::scalar()
{
    return theRandScalar.get();
}

double Rand::scalar(double min, double max)
{
    return theRandScalar.get(min, max);
}

void Rand::init()
{
    return theRandScalar.init();
}

void Rand::init(int seed)
{
    return theRandScalar.init(seed);
}

Vector Rand::vector(int s)
{
    yarp::sig::Vector ret((size_t) s);
    for(int k=0;k<s;k++)
    {
        ret[k]=theRandScalar.get();
    }

    return ret;
}

Vector Rand::vector(const Vector &min, const Vector &max)
{
    int s=min.size();
    yarp::sig::Vector ret(s);
    for(int k=0;k<s;k++)
    {
        ret[k]=theRandScalar.get(min[k], max[k]);
    }

    return ret;
}

RandScalar::RandScalar()
{
  idum=0;
  iy=0;
  init();
}

RandScalar::RandScalar(int seed)
{
  idum=0;
  iy=0;
  init(seed);
}

double RandScalar::get()
{
  int k;
  double temp;
  int j;

  k=(idum)/IQ;
  idum=IA*(idum-k*IQ)-IR*k;
  if (idum<0)
    idum+=IM;
  j=iy/NDIV;
  iy=iv[j];
  iv[j]=idum;
  temp=AM*iy;
  if (temp > RNMX)
    temp=RNMX;

  return temp;
}

double RandScalar::get(double min, double max)
{
    double ret=RandScalar::get();
    ret=ret*(max-min)+min;
    return ret;
}

RandnScalar::RandnScalar()
{
  y[0] = 0.0;
  y[1] = 0.0;
  last = 2;
}

void RandnScalar::init()
{
  rnd.init();
  last=2;
}

void RandnScalar::init(int aSeed)
{
  rnd.init(aSeed);
  last = 2;
}

double RandnScalar::get()
{
  double ret;
  if (last > 1)
    {
        boxMuller();    //compute two normally distr random number
        last = 0;
    }
    
  ret = y[last];
  last++;
  return ret;
}

inline void RandnScalar::boxMuller()
{
  double x1, x2;
  double w = 2.0;
  while (w >= 1.0)
    {
      x1 = 2.0 * rnd.get() - 1.0;
      x2 = 2.0 * rnd.get() - 1.0;
      w = x1 * x1 + x2 * x2;
    }

  w = sqrt( (-2.0 * log( w ) ) / w );
  y[0] = x1 * w;
  y[1] = x2 * w;
}


// initialize with a call to "time"
void RandScalar::init()
{
  // initialize with time
  int t=(int)time(0);
  RandScalar::init(t);
}

void RandScalar::init(int s)
{
  idum=-s;
  int k;
  int j;

  if (idum<=0 || !iy)
    {
      if (-idum<1)
          idum=1;
      else
          idum=-idum;
      for (j=NTAB+7;j>=0;j--)
      {
          k=(idum)/IQ;
          idum=IA*(idum-k*IQ)-IR*k;
          if (idum<0)
              idum+=IM;
          if (j<NTAB)
              iv[j]=idum;
      }
      iy=iv[0];
    }
}

using namespace yarp::sig;

RandVector::RandVector(int s)
{
    data.resize(s);
}

void RandVector::resize(int s)
{
    data.resize(s);
}

void RandVector::init()
{
    rnd.init();
}

void RandVector::init(int seed)
{
    rnd.init(seed);
}

const Vector &RandVector::get()
{
    for (int k=0;k<data.size(); k++)
    {
        data[k]=rnd.get();
    }

    return data;
}

const Vector &RandVector::get(const Vector &min, const Vector &max)
{
    for (int k=0;k<data.size(); k++)
    {
        data[k]=rnd.get(min[k], max[k]);
    }

    return data;
}

RandnVector::RandnVector(int s)
{
    data.resize(s);
}

void RandnVector::resize(int s)
{
    data.resize(s);
}

void RandnVector::init()
{
    rnd.init();
}

void RandnVector::init(int seed)
{
    rnd.init(seed);
}

const Vector &RandnVector::get()
{
    for (int k=0;k<data.size(); k++)
    {
        data[k]=rnd.get();
    }

    return data;
}

