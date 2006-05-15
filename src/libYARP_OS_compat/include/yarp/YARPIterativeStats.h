// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
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
/// $Id: YARPIterativeStats.h,v 1.2 2006-05-15 15:57:58 eshuy Exp $
///
///

// YARPIterativeStats.h: interface for the IterativeStats class.
//
// Commpute mean and std of samples in a iterative way
//
// Sept 2002 by nat
//
// TODO: check out n_elem for wrapping
//////////////////////////////////////////////////////////////////////

#ifndef __iterativestats__
#define __iterativestats__

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <math.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

///
///
/// this is only used by YARPRateThread to compute mean and stddev of
///	thread rate/interval.
///
class IterativeStats  
{
public:
	IterativeStats();
	virtual ~IterativeStats();

	void add_point(double new_elem)
	{
		n_elem++;

		if (n_elem == 1)
            {
                mean = new_elem;
                sum_square = new_elem*new_elem;
                var = 0.0;
            }
		else
            {
                mean = (mean*(n_elem-1) + new_elem)/(n_elem);
			
                sum_square = sum_square + new_elem * new_elem;
                var = (sum_square - n_elem * mean * mean)/(n_elem-1);
            }
	}

	IterativeStats& operator +=(double new_elem) { add_point(new_elem); return *this; }
	double get_mean() { return mean; }
	double get_var() { return var; }
	double get_std() { return sqrt(var); }
	
	void reset()
	{
		n_elem = 0;
		mean = 0.0;
		var = 0.0;
		sum_square = 0.0;
	};

	unsigned int elem() { return n_elem; }

private:
	double mean;		 // actual mean
	double var;			 // actual std
	double sum_square;	 // actual sum_square (to be used to compute std)

	unsigned int n_elem; // number of actual elem
};

#endif // __iteractivestats__
