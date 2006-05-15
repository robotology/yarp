// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 *
 * This is a legacy YARP1 YARPBottle implementation, imported untouched
 * (except for this header)
 *
 * -paulfitz
 *
 */




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
///  $Id: YARPBottle.cpp,v 1.2 2006-05-15 15:57:58 eshuy Exp $
///
///

#include <yarp/YARPBottle.h>

#include <iostream>
using namespace std;

YARPBottle::YARPBottle()
{
	id.set(YBLabelNULL);
	top = 0;
	index = 0;
	len = 0;
	lastReadSeq = 0;
}

void YARPBottle::dump()
{
	printf ("Starting dump of internal string\n");
	printf ("Seq length: %d\n", text.size());
	YARPVector<char>::iterator it(text);
	while (!it.done())
        {
            printf("%d\n", *it);
            it++;
        }
}

void YARPBottle::display()
{
    ACE_OS::printf("%s: ", id.c_str());
    rewind();
    int first = 1;
    while (more())
        {
            char *str;
            int ch = readRawInt();
            _moveOn(sizeof(enum __YBTypeCodes));
            if (!first)
                {
                    printf(" ");
                }
            first = 0;
            switch(ch)
                {
                case YBTypeInt:
                    printf("%d", (int)readRawInt());
                    _moveOn(sizeof(NetInt32));
                    break;
                case YBTypeVocab:
                    str = (char *) readRawText();
                    printf("<%s>", str);
                    _moveOn(ACE_OS::strlen(str)+1+sizeof(int));
                    break;
                case YBTypeDouble:
                    printf("%g", readRawFloat());
                    _moveOn(sizeof(double));
                    break;
                case YBTypeString:
                    str = (char *) readRawText();
                    printf("(%s)", str);
                    _moveOn(ACE_OS::strlen(str)+1+sizeof(int));
                    break;
                case YBTypeDoubleVector:
                    {
                        int l = readRawInt();
                        _moveOn(sizeof(int));

                        printf("<%g", readRawFloat());
                        _moveOn(sizeof(double));
                        if (l>1)
                            {
                                for(int i = 0; i < l-2; i++)
                                    {
                                        printf("\t%g", readRawFloat());
                                        _moveOn(sizeof(double));
                                    }
                                printf("\t%g", readRawFloat());
                                _moveOn(sizeof(double));
                            }
                        printf(">");
                    }
                    break;
                case YBTypeIntVector:
                    {
                        int l = readRawInt();
                        _moveOn(sizeof(int));

                        printf("<%d", readRawInt());
                        _moveOn(sizeof(int));
                        if (l>1)
                            {
                                for(int i = 0; i < l-2; i++)
                                    {
                                        printf("\t%d", readRawInt());
                                        _moveOn(sizeof(int));
                                    }
                                printf("\t%d", readRawInt());
                                _moveOn(sizeof(int));
                            }
                        printf(">");
                    }
                    break;	
                default:
                    printf("???");
                    break;
                }
        }
    printf("\n");
    rewind();
}

