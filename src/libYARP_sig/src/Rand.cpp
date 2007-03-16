#if 0 // do not compile yet
///////// YARPRndNormal
YARPRndNormal::YARPRndNormal(): YARPRnd()
{
  _y[0] = 0.0;
  _y[1] = 0.0;
  _last = 2;
}

void YARPRndNormal::init()
{
  YARPRnd::init();
  _last=2;
}

void YARPRndNormal::init(int aSeed)
{
  YARPRnd::init(aSeed);
  _last = 2;
}

double YARPRndNormal::getNumber()
{
  double ret;
  if (_last > 1)
    {
      _boxMuller();	//compute two normally distr random number
      _last = 0;
    }
		
  ret = _y[_last];
  _last++;
  return ret;
}

inline void YARPRndNormal::_boxMuller()
{
  double x1, x2;
  double w = 2.0;
  while (w >= 1.0)
    {
      x1 = 2.0 * YARPRnd::getNumber() - 1.0;
      x2 = 2.0 * YARPRnd::getNumber() - 1.0;
      w = x1 * x1 + x2 * x2;
    }

  w = sqrt( (-2.0 * log( w ) ) / w );
  _y[0] = x1 * w;
  _y[1] = x2 * w;
}

////////// YARPRndVector
void YARPRndVector::init(int s)
{
  _rndGen.init(s);
}

void YARPRndVector::init()
{
  _rndGen.init();
}

void YARPRndVector::resize(const YVector &max, const YVector &min)
{
//  ACE_ASSERT(max.Length() == min.Length());
//  ACE_ASSERT(max.Length() > 0);
	
  _size=max.Length();

  _size = max.Length();
  _max = max;
  _min = min;
  _random.Resize(_size);
}

////////// YARPRndGaussVector
void YARPRndGaussVector::resize(const YVector &av, const YVector &std)
{
//  ACE_ASSERT(av.Length() == std.Length());
 // ACE_ASSERT(av.Length() > 0);
	
  _rndGen=new YARPRndNormal[av.Length()];

  _size = av.Length();
  _average = av;
  _std = std;
  _random.Resize(_size);
}

void YARPRndGaussVector::init(int s)
{
  YARPRnd seeds;
  seeds.init(s);
    
  for(int k=0;k<_size;k++)
    {
      int r=(int) (seeds.getNumber()*INT_MAX+0.5);
      _rndGen[k].init(r);
    }
}

// initialize by using time
void YARPRndGaussVector::init()
{
  YARPRnd seeds;
  seeds.init();
    
  for(int k=0;k<_size;k++)
    {
      int r=(int) (seeds.getNumber()*INT_MAX+0.5);
      _rndGen[k].init(r);
    }
}

void YARPRndGaussVector::setStd(const YVector &std)
{
  _std=std;
}

void YARPRndGaussVector::setAv(const YVector &av)
{
  _average=av;
}

////////// YARPRndSafeGaussVector
void YARPRndSafeGaussVector::resize(const YVector &max, const YVector &min, const YVector &av, const YVector &std)
{
//  ACE_ASSERT((av.Length() == std.Length()) && (max.Length() == min.Length()) && (min.Length() == std.Length()));
 // ACE_ASSERT(av.Length() > 0);

  YARPRndGaussVector::resize(av, std);

  _max = max;
  _min = min;
}

#endif do not compile yet
