// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 *
 * This is a legacy YARP1 YARPBottle implementation, imported untouched
 * (except for this header)
 *
 * -paulfitz
 *
 */





/*
 * Copyright (C) 2006 Paul Fitzpatrick, Giorgio Metta
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

///
///  $Id: YARPBottle.cpp,v 1.3 2006-10-24 16:43:50 eshuy Exp $
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

