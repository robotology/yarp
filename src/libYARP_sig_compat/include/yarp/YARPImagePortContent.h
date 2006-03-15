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
/// $Id: YARPImagePortContent.h,v 1.1 2006-03-15 09:31:28 eshuy Exp $
///
///

#ifndef YARPIMAGEPORTCONTENT_INC
#define YARPIMAGEPORTCONTENT_INC

#include <yarp/YARPConfig.h>
#include <ace/config.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

#include <yarp/YARPImage.h>
#include <yarp/YARPPort.h>
#include <yarp/YARPPortContent.h>

#include <yarp/begin_pack_for_net.h>

class YARPImagePortContentHeader
{
public:
  NetInt32 len;
  NetInt32 w;
  NetInt32 id;
  NetInt32 h;
  NetInt32 depth;
  double timestamp;
} PACKED_FOR_NET;

#include <yarp/end_pack_for_net.h>

class YARPImagePortContent : public YARPGenericImage, public YARPPortContent
{
public:
	YARPImagePortContentHeader header;
	virtual ~YARPImagePortContent () 
	{
		///ACE_DEBUG ((LM_DEBUG, "destroying a YARPImagePortContent\n"));
	}

	virtual int Read(YARPPortReader& reader)
	{
		if (reader.Read((char*)(&header),sizeof(header)))
		{
			//	  cout.flush();
			SetID(header.id);
			//SetPixelSize(header.depth);
			///int r = GetWidth();
			if (GetWidth()!=header.w || GetHeight()!=header.h)
			{
				//	      cout << "CREATING!! " << header.w << " " << GetWidth() << endl;
				Resize(header.w,header.h);
			}

			char *mem = GetRawBuffer();
			ACE_ASSERT(mem!=NULL);
			ACE_ASSERT(GetWidth()==header.w && GetHeight()==header.h && GetPixelSize()==header.depth);
			reader.Read(mem,header.len);
			//timestamp = header.timestamp;
		}
		return 1;
	}

	virtual int Write(YARPPortWriter& writer)
	{
		//      cout << "HIT Write called" << endl;
		header.h = GetHeight();
		header.w = GetWidth();
		header.depth = GetPixelSize();
		header.id = GetID();
		header.len = header.h*(header.w*header.depth+GetPadding());      
		header.timestamp = 0;
		writer.Write((char*)(&header),sizeof(header));
		char *mem = GetRawBuffer();
		ACE_ASSERT(mem!=NULL);
		writer.Write(mem,header.len);
		return 1;
	}

	// Called when communications code is finished with the object, and
	// it will be passed back to the user.
	// Often fine to do nothing here.
	virtual int Recycle() { return 0; }
};


// The following has not been tested yet 

template<>
class YARPInputPortOf<YARPGenericImage> : public YARPBasicInputPort<YARPImagePortContent>
{
public:
	YARPInputPortOf<YARPGenericImage>(int n_service_type = DEFAULT_BUFFERS, 
					  int n_protocol_type = YARP_DEFAULT_PROTOCOL) :
		YARPBasicInputPort<YARPImagePortContent> (n_service_type, n_protocol_type) {}

	virtual ~YARPInputPortOf<YARPGenericImage> () { YARPPort::End(); }
};

template<>
class YARPOutputPortOf<YARPGenericImage> : public YARPBasicOutputPort<YARPImagePortContent>
{
public:
	YARPOutputPortOf<YARPGenericImage>(int n_service_type = DEFAULT_OUTPUTS, int n_protocol_type = YARP_DEFAULT_PROTOCOL) :
		YARPBasicOutputPort<YARPImagePortContent> (n_service_type, n_protocol_type) {}

	virtual ~YARPOutputPortOf<YARPGenericImage> () { YARPPort::End(); }
};


#endif
