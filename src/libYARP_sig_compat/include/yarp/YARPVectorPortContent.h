/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2004           ///
///                                                                   ///
///                    #Add our name(s) here#                         ///
///                                                                   ///
///     "Licensed under the Academic Free License Version 1.0"        ///
///                                                                   ///
/// The complete license description is contained in the              ///
/// licence.template file included in this distribution in            ///
/// $YARP_ROOT/conf. Please refer to this file for complete           ///
/// information about the licensing of YARP                           ///
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
/////////////////////////////////////////////////////////////////////////

///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPVectorPortContent.h,v 1.1 2006-03-15 09:31:28 eshuy Exp $
///
///

#ifndef YARPVECTORPORTCONTENT_INC
#define YARPVECTORPORTCONTENT_INC

#include <yarp/YARPConfig.h>
#include <ace/config.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

#include <yarp/YARPMath.h>
#include <yarp/YARPPort.h>
#include <yarp/YARPPortContent.h>

#include <yarp/begin_pack_for_net.h>

class YARPVectorPortContentHeader
{
public:
  NetInt32 len;
} PACKED_FOR_NET;

class YARPMatrixPortContentHeader
{
public:
  NetInt32 r;
  NetInt32 c;
} PACKED_FOR_NET;

#include <yarp/end_pack_for_net.h>

class YARPVectorPortContent : public YVector, public YARPPortContent
{
public:
	YARPVectorPortContentHeader header;

	virtual ~YARPVectorPortContent () {}

	//// of course this only works on machines w/ the same floating point representation.
	virtual int Read (YARPPortReader& reader)
	{
		if (reader.Read ((char*)(&header), sizeof(header)))
		{
			if (header.len != Length()) 
				Resize (header.len);

			char *mem = (char *)data();
			ACE_ASSERT (mem != NULL);
			reader.Read (mem, header.len * sizeof(double));
		}
		return 1;
	}

	virtual int Write (YARPPortWriter& writer)
	{
		header.len = Length();
		writer.Write ((char*)(&header), sizeof(header));

		char *mem = (char *)data();
		ACE_ASSERT (mem != NULL);
		writer.Write (mem, header.len * sizeof(double));
		return 1;
	}

	// Called when communications code is finished with the object, and
	// it will be passed back to the user.
	// Often fine to do nothing here.
	virtual int Recycle() { return 0; }

    YVector& operator=(const YVector &vec) { return YVector::operator= (vec); }
};

template <>
class YARPInputPortOf<YVector> : public YARPBasicInputPort<YARPVectorPortContent>
{
public:
	YARPInputPortOf<YVector>(int n_service_type = DEFAULT_BUFFERS, int n_protocol_type = YARP_DEFAULT_PROTOCOL) :
		YARPBasicInputPort<YARPVectorPortContent> (n_service_type, n_protocol_type) {}

	virtual ~YARPInputPortOf<YVector> () { YARPPort::End(); }
};

template <>
class YARPOutputPortOf<YVector> : public YARPBasicOutputPort<YARPVectorPortContent>
{
public:
	YARPOutputPortOf<YVector>(int n_service_type = DEFAULT_OUTPUTS, int n_protocol_type = YARP_DEFAULT_PROTOCOL) :
		YARPBasicOutputPort<YARPVectorPortContent> (n_service_type, n_protocol_type) {}

	virtual ~YARPOutputPortOf<YVector> () { YARPPort::End(); }
};

class YARPMatrixPortContent : public YMatrix, public YARPPortContent
{
public:
	YARPMatrixPortContentHeader header;

	virtual ~YARPMatrixPortContent () {}

	//// of course this only works on machines w/ the same floating point representation.
	virtual int Read (YARPPortReader& reader)
	{
		if (reader.Read ((char*)(&header), sizeof(header)))
		{
			if (header.r != NRows() || header.c != NCols())
				Resize (header.r, header.c);

			char *mem = (char *)(data()[0]);		/// mem is contiguous.
			ACE_ASSERT (mem != NULL);
			reader.Read (mem, header.r * header.c * sizeof(double));
		}
		return 1;
	}

	virtual int Write (YARPPortWriter& writer)
	{
		header.r = NRows();
		header.c = NCols();
		writer.Write ((char*)(&header), sizeof(header));

		char *mem = (char *)(data()[0]);
		ACE_ASSERT (mem != NULL);
		writer.Write (mem, header.r * header.c * sizeof(double));
		return 1;
	}

	// Called when communications code is finished with the object, and
	// it will be passed back to the user.
	// Often fine to do nothing here.
	virtual int Recycle() { return 0; }

    YMatrix& operator=(const YMatrix &mat) { return YMatrix::operator= (mat); }
};

template <>
class YARPInputPortOf<YMatrix> : public YARPBasicInputPort<YARPMatrixPortContent>
{
public:
	YARPInputPortOf<YMatrix>(int n_service_type = DEFAULT_BUFFERS, int n_protocol_type = YARP_DEFAULT_PROTOCOL) :
		YARPBasicInputPort<YARPMatrixPortContent> (n_service_type, n_protocol_type) {}

	virtual ~YARPInputPortOf<YMatrix> () { YARPPort::End(); }
};

template <>
class YARPOutputPortOf<YMatrix> : public YARPBasicOutputPort<YARPMatrixPortContent>
{
public:
	YARPOutputPortOf<YMatrix>(int n_service_type = DEFAULT_OUTPUTS, int n_protocol_type = YARP_DEFAULT_PROTOCOL) :
		YARPBasicOutputPort<YARPMatrixPortContent> (n_service_type, n_protocol_type) {}

	virtual ~YARPOutputPortOf<YMatrix> () { YARPPort::End(); }
};

#endif
