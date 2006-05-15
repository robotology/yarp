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
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #pasa, paulfitz#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPImageUtils.h,v 1.3 2006-05-15 15:57:59 eshuy Exp $
///
///

//
// YARPImageUtils.h 
//

//
// Utilities. I'm not trying to be exhaustive. When we need them we can 
//	simply add them here.
//
#ifndef __YARPImageUtilsh__
#define __YARPImageUtilsh__

#include <yarp/YARPConfig.h>
#include <ace/config.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

#include <yarp/YARPImage.h>

/**
 * Legacy namespace for general image utilities.
 */
namespace YARPImageUtils 
{
	void GetRed (const YARPImageOf<YarpPixelRGB>& in, YARPImageOf<YarpPixelMono>& out);
	void GetRed (const YARPImageOf<YarpPixelBGR>& in, YARPImageOf<YarpPixelMono>& out);
	void GetGreen (const YARPImageOf<YarpPixelRGB>& in, YARPImageOf<YarpPixelMono>& out);
	void GetGreen (const YARPImageOf<YarpPixelBGR>& in, YARPImageOf<YarpPixelMono>& out);
	void GetBlue (const YARPImageOf<YarpPixelRGB>& in, YARPImageOf<YarpPixelMono>& out);
	void GetBlue (const YARPImageOf<YarpPixelBGR>& in, YARPImageOf<YarpPixelMono>& out);

	void SetRed (const YARPImageOf<YarpPixelMono>& in, YARPImageOf<YarpPixelRGB>& out);
	void SetRed (const YARPImageOf<YarpPixelMono>& in, YARPImageOf<YarpPixelBGR>& out);
	void SetGreen (const YARPImageOf<YarpPixelMono>& in, YARPImageOf<YarpPixelRGB>& out);
	void SetGreen (const YARPImageOf<YarpPixelMono>& in, YARPImageOf<YarpPixelBGR>& out);
	void SetBlue (const YARPImageOf<YarpPixelMono>& in, YARPImageOf<YarpPixelRGB>& out);
	void SetBlue (const YARPImageOf<YarpPixelMono>& in, YARPImageOf<YarpPixelBGR>& out);

	void GetValue (const YARPImageOf<YarpPixelHSV>& in, YARPImageOf<YarpPixelMono>& out);
	void GetSaturation (const YARPImageOf<YarpPixelHSV>& in, YARPImageOf<YarpPixelMono>& out);
	void GetHue (const YARPImageOf<YarpPixelHSV>& in, YARPImageOf<YarpPixelMono>& out);

	void PasteInto (const YARPImageOf<YarpPixelMono>& src, int x, int y, int zoom, YARPImageOf<YarpPixelMono>& dst);
};

#endif

