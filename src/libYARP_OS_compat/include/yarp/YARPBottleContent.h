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
///                    #nat#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPBottleContent.h,v 1.2 2006-05-15 15:57:58 eshuy Exp $
///
///

#ifndef __YARPBOTTLEPORTCONTENT__
#define __YARPBOTTLEPORTCONTENT__

#include <yarp/YARPConfig.h>
#include <ace/config.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

/**
 * \file YARPBottleContent.h Defines a class that allows sending the YARPBottle
 * through a port. This must always be included whenever sending bottles. It is
 * also included (circular include) from the YARPBottle.h just to make sure
 * the user doesn't forget to include it.
 */
#include <yarp/YARPBottle.h>
#include <yarp/YARPPort.h>
#include <yarp/YARPPortContent.h>

/**
 * YARPBottleContent is the content type for sending bottles through ports.
 */
class YARPBottleContent : public YARPBottle, public YARPPortContent
{
public:
	/**
	 * Default constructor.
	 */
	virtual ~YARPBottleContent () 
	{
		///ACE_DEBUG ((LM_DEBUG, "destroying a YARPBottleContent\n"));
	}

	/**
	 * Reads a bottle from the network. It calls methods in YARPPortReader
	 * to get buffers from the connection and traslates them into a
	 * port. This method is automatically called by the Port code.
	 * @param reader is the reference to the YARPPortReader object.
	 * @return YARP_OK if successful.
	 */
	virtual int Read(YARPPortReader& reader)
    {
        // read id
        reader.Read((char*)(&id.length), sizeof(id.length));
        reader.Read(id.text, id.length);
        // read id
        reader.Read((char*)(&len),sizeof(len));

        text.reserve(len);
        text.resize(len);

        top = len;
        int result = reader.Read((char*)(&text[0]),len);
        rewind();
        return result;
    }
  
	/**
	 * Writes a bottle to the network. It calls methods in YARPPortWriter
	 * to write buffers into the connection. This method is automatically called
	 * by the Port code.
	 * @param writer is the reference to the YARPPortWriter object.
	 * @return YARP_OK if successful.
	 */
	virtual int Write(YARPPortWriter& writer)
    {
        writer.Write((char*)(&id.length),sizeof(id.length));
        writer.Write((char*) id.text, id.length);
        len = top; 
      
        writer.Write((char*)(&len),sizeof(len));
        int r = writer.Write((char*)(&text[0]),len);
        return r;
    }
  
	/**
	 * In case the content has to be reused. Resets the internal bottle object.
	 */
	virtual int Recycle()
    { reset(); top = 0; return 0; }

	/**
	 * Copies operator. Useful for copying contents.
	 * @param vec is the object reference to copy from.
	 * @return a reference to this object.
	 */
    YARPBottle& operator=(const YARPBottle &vec) { return YARPBottle::operator= (vec); }
};


template<>
class YARPInputPortOf<YARPBottle> : public YARPBasicInputPort<YARPBottleContent>
{
public:
	YARPInputPortOf<YARPBottle>(int n_service_type = DEFAULT_BUFFERS, int n_protocol_type = YARP_DEFAULT_PROTOCOL) :
		YARPBasicInputPort<YARPBottleContent> (n_service_type, n_protocol_type) {}

	virtual ~YARPInputPortOf<YARPBottle> () {  YARPPort::End(); }
};

template<>
class YARPOutputPortOf<YARPBottle> : public YARPBasicOutputPort<YARPBottleContent>
{
public:
	YARPOutputPortOf<YARPBottle>(int n_service_type = DEFAULT_OUTPUTS, int n_protocol_type = YARP_DEFAULT_PROTOCOL) :
		YARPBasicOutputPort<YARPBottleContent> (n_service_type, n_protocol_type) {}

	virtual ~YARPOutputPortOf<YARPBottle> () {  YARPPort::End(); }
};

#endif
