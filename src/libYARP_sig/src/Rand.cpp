#include <yarp/os/Semaphore.h>
#include <yarp/sig/Rand.h>
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
    
    double get()
    {
        double ret;
        mutex.wait();
        ret=RandScalar::get();
        mutex.post();
        return ret;
    }

} theRandScalar;

double Random::rand()
{
    return theRandScalar.get();
}

void Random::init()
{
    return theRandScalar.init();
}

void Random::init(int seed)
{
    return theRandScalar.init(seed);
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
