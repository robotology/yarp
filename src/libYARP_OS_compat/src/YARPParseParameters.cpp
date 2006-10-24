// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick, Giorgio Metta
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

///
///  $Id: YARPParseParameters.cpp,v 1.3 2006-10-24 16:43:50 eshuy Exp $
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


