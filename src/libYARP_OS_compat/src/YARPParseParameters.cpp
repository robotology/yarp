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
///                    #Add our name(s) here#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
///  $Id: YARPParseParameters.cpp,v 1.1 2006-03-13 12:52:42 eshuy Exp $
///
///

#include <yarp/YARPParseParameters.h>

///using namespace std;

bool YARPParseParameters::parse (int argc, char *argv[], const YARPString &key, YARPString &out)
{
	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			// found parameter, check key
			if (key == YARPString(argv[i]+1)) 
			{
				// found key
				i++;
		
				if (i==argc)
					return false;
				else if ( (argv[i][0] == '-') && (!isdigit(argv[i][1])))
					return false;
				else if (key == YARPString("name"))	// exception, "name"
				{
					// append "/"
					out = "/";	
					out.append(YARPString(argv[i]));
					return true;
				}
				else
				{
					out = YARPString (argv[i]);
					return true;
				}
			}
		}
	}
	return false; 
}

bool YARPParseParameters::parse (int argc, char *argv[], const YARPString &key)
{
	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			// found parameter, check key
			if (key == YARPString(argv[i]+1))
				return true;
		}
	}
	return false; 
}

bool YARPParseParameters::parse (int argc, char *argv[], const YARPString &key, int *out) 
{
	YARPString dummy;
	if (YARPParseParameters::parse(argc, argv, key, dummy))
	{
		*out = atoi(dummy.c_str());
		return true;
	}
	else
		return false;

}

bool YARPParseParameters::parse (int argc, char *argv[], const YARPString &key, double *out) 
{
	YARPString dummy;
	if (YARPParseParameters::parse(argc, argv, key, dummy))
	{
		*out = atof(dummy.c_str());
		return true;
	}
	else
		return false;

}

bool YARPParseParameters::parse (int argc, char *argv[], const YARPString &key, char *out) 
{
	YARPString dummy;
	if (YARPParseParameters::parse(argc, argv, key, dummy))
	{
		ACE_OS::strcpy(out, dummy.c_str());
		return true;
	}
	else
		return false;

}


