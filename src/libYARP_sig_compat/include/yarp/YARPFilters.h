/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///                                                                   ///
/// This Academic Free License applies to any software and associated ///
/// documentation (the "Software") whose owner (the "Licensor") has   ///
/// placed the statement "Licensed under the Academic Free License    ///
/// Version 1.0" immediately after the copyright notice that applies  ///
/// to the Software.                                                  ///
/// Permission is hereby granted, free of charge, to any person       ///
/// obtaining a copy of the Software (1) to use, copy, modify, merge, ///
/// publish, perform, distribute, sublicense, and/or sell copies of   ///
/// the Software, and to permit persons to whom the Software is       ///
/// furnished to do so, and (2) under patent claims owned or          ///
/// controlled by the Licensor that are embodied in the Software as   ///
/// furnished by the Licensor, to make, use, sell and offer for sale  ///
/// the Software and derivative works thereof, subject to the         ///
/// following conditions:                                             ///
/// Redistributions of the Software in source code form must retain   ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers. ///
/// Redistributions of the Software in executable form must reproduce ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers  ///
/// in the documentation and/or other materials provided with the     ///
/// distribution.                                                     ///
///                                                                   ///
/// Neither the names of Licensor, nor the names of any contributors  ///
/// to the Software, nor any of their trademarks or service marks,    ///
/// may be used to endorse or promote products derived from this      ///
/// Software without express prior written permission of the Licensor.///
///                                                                   ///
/// DISCLAIMERS: LICENSOR WARRANTS THAT THE COPYRIGHT IN AND TO THE   ///
/// SOFTWARE IS OWNED BY THE LICENSOR OR THAT THE SOFTWARE IS         ///
/// DISTRIBUTED BY LICENSOR UNDER A VALID CURRENT LICENSE. EXCEPT AS  ///
/// EXPRESSLY STATED IN THE IMMEDIATELY PRECEDING SENTENCE, THE       ///
/// SOFTWARE IS PROVIDED BY THE LICENSOR, CONTRIBUTORS AND COPYRIGHT  ///
/// OWNERS "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, ///
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   ///
/// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO      ///
/// EVENT SHALL THE LICENSOR, CONTRIBUTORS OR COPYRIGHT OWNERS BE     ///
/// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   ///
/// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN ///
/// CONNECTION WITH THE SOFTWARE.                                     ///
///                                                                   ///
/// This license is Copyright (C) 2002 Lawrence E. Rosen. All rights  ///
/// reserved. Permission is hereby granted to copy and distribute     ///
/// this license without modification. This license may not be        ///
/// modified without the express written permission of its copyright  ///
/// owner.                                                            ///
///                                                                   ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #pasa, paulfitz#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPFilters.h,v 1.1 2006-03-15 09:31:28 eshuy Exp $
///
///

///////////////////////////////////////////////////////////////////////////////
//
// YARPfilters.h
//  minimal definition of filters types.
//	this is just to derive from a common base class.
// 

#ifndef __YARPFiltersh__
#define __YARPFiltersh__

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

#include <math.h>

#include <yarp/YARPImage.h>

//
//
// I haven't added an "apply" method because the parameters could
// be different for different filters.
// I'll try to use an Apply of the form Apply(const src, dest, params).
class YARPFilter
{
private:
	YARPFilter (const YARPFilter&);
	void operator= (const YARPFilter&);

public:
	YARPFilter () {}
	virtual ~YARPFilter () {}

	virtual void Cleanup () {}
	virtual bool InPlace () const { return false; }
};

//
// I'm trying to keep this class minimal and do not commit to any 
//	 style. I'll try to use "Apply" most of the time...
//
template <class T>
class YARPFilterOf : public YARPFilter
{
private:
	// filters are not supposed to be copied around.
	YARPFilterOf (const YARPFilterOf<T>&);
	void operator= (const YARPFilterOf<T>&);

protected:
	// support for logpolar border handling.
	///void AddBorderLP(YARPImageOf<T>& id, const YARPImageOf<T>& is, int kLR, int kUD);

public:
	YARPFilterOf () : YARPFilter() {}
	virtual ~YARPFilterOf () {}
};


/*
template <class T>
void YARPFilterOf<T>::AddBorderLP(YARPImageOf<T>& id, const YARPImageOf<T>& is, int kLR, int kUD)
{
	int nAng = is.GetHeight();
	int nEcc = is.GetWidth();
	
	unsigned char **dst = (unsigned char **) id.GetArray();
	unsigned char **src = (unsigned char **) is.GetArray();
	
	ACE_ASSERT((nAng%2) == 0);
	
	const int d = sizeof(T);

	// optimize most common formats.
	switch (d)
	{
	case 1:
		{
			// left border
			int r, c;
			for (c = 0; c < kLR ; c++)
			{
				for (r = 0; r < (nAng/2); r++)
					dst[kUD+nAng/2+r][(kLR-1-c)] = src[r][c];

				for (r = nAng/2; r < nAng; r++)
					dst[r-nAng/2+kUD][(kLR-1-c)] = src[r][c];
			}

			// top & bottom borders.	
			for (c = 0; c < nEcc+kLR; c++)
			{
				for (r = 0; r < kUD; r++)
					dst[kUD+nAng+r][c] = dst[r+kUD][c];

				int t;
				for (t = 0, r = nAng; r < nAng + kUD; t++, r++)
					dst[t][c*d] = dst[r][c*d];
			} 
		}
		break;

	case 3:
		{
			// left border
			int r, c;
			for (c = 0; c < kLR ; c++)
			{
				for (r = 0; r < (nAng/2); r++)
				{
					dst[kUD+nAng/2+r][(kLR-1-c)*3] = src[r][c*3];
					dst[kUD+nAng/2+r][(kLR-1-c)*3+1] = src[r][c*3+1];
					dst[kUD+nAng/2+r][(kLR-1-c)*3+2] = src[r][c*3+2];
				}

				for (r = nAng/2; r < nAng; r++)
				{
					dst[r-nAng/2+kUD][(kLR-1-c)*3] = src[r][c*3];
					dst[r-nAng/2+kUD][(kLR-1-c)*3+1] = src[r][c*3+1];
					dst[r-nAng/2+kUD][(kLR-1-c)*3+2] = src[r][c*3+2];
				}
			}

			// top & bottom borders.	
			for (c=0; c<nEcc+kLR; c++)
			{
				for (r = 0; r < kUD; r++)
				{
					dst[kUD+nAng+r][c*3] = dst[r+kUD][c*3];
					dst[kUD+nAng+r][c*3+1] = dst[r+kUD][c*3+1];
					dst[kUD+nAng+r][c*3+2] = dst[r+kUD][c*3+2];
				}

				int t;
				for (t = 0, r = nAng; r < nAng + kUD; t++, r++)
				{
					dst[t][c*3] = dst[r][c*3];
					dst[t][c*3+1] = dst[r][c*3+1];
					dst[t][c*3+2] = dst[r][c*3+2];
				}
			} 
		}
		break;

	default:
		{
			// left border
			int r, c;
			for (c = 0; c < kLR ; c++)
			{
				for (r = 0; r < (nAng/2); r++)
				{
					memcpy (&dst[kUD+nAng/2+r][(kLR-1-c)*d], 
							&src[r][c*d],
							d);
				}

				for (r = nAng/2; r < nAng; r++)
				{
					memcpy (&dst[r-nAng/2+kUD][(kLR-1-c)*d],
							&src[r][c*d],
							d);
				}
			}

			// top & bottom borders.	
			for (c=0; c<nEcc+kLR; c++)
			{
				for (r = 0; r < kUD; r++)
				{
					memcpy (&dst[kUD+nAng+r][c*d],
							&dst[r+kUD][c*d],
							d);
				}

				int t;
				for (t = 0, r = nAng; r < nAng + kUD; t++, r++)
				{
					memcpy (&dst[t][c*d],
							&dst[r][c*d],
							d);
				}
			} 
		}
		break;
	}
}
*/

#endif
