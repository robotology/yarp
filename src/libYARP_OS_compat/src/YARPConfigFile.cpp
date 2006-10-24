// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick, Giorgio Metta
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

///
///  $Id: YARPConfigFile.cpp,v 1.3 2006-10-24 16:43:50 eshuy Exp $
///
///


//
// YARPIniFile.cpp

#include <yarp/YARPConfigFile.h>

//
// gestione del path di default.
//
using namespace std;

bool YARPConfigFile::_open(const YARPString &path, const YARPString &filename)
{
	YARPString tmp = path;
	tmp.append(filename);
	_pFile = ACE_OS::fopen(tmp.c_str(), "r");
	if (_pFile != NULL)
        {
            _openFlag = true;
            return true;
        }
	else 
		return false;
}

int YARPConfigFile::get(const char *section, const char *name, double *out, int n)
{
	if (_get(section, name) == YARP_FAIL)
		return YARP_FAIL;

	int i = 0;
	while ( (i<n) && (fscanf(_pFile, "%lf", out++) != EOF) )
		i++;
			
	_close();
	
	if (i == n)
		return YARP_OK;		// found all the values
	else	
		return YARP_FAIL;	
}

int YARPConfigFile::get(const char *section, const char *name, short *out, int n)
{
	if (_get(section, name) == YARP_FAIL)
		return YARP_FAIL;

	int i = 0;
	while ( (i<n) && (fscanf(_pFile, "%hd", out++) != EOF) )
		i++;
			
	_close();
	
	if (i == n)
		return YARP_OK;		// found all the values
	else	
		return YARP_FAIL;	
}

int YARPConfigFile::get(const char *section, const char *name, char *out, int n)
{
	if (_get(section, name) == YARP_FAIL)
		return YARP_FAIL;

	int i = 0;
	while ( (i<n) && (fscanf(_pFile, "%c", out++) != EOF) )
		i++;
			
	_close();
	
	if (i == n)
		return YARP_OK;		// found all the values
	else	
		return YARP_FAIL;	
}

int YARPConfigFile::getHex(const char *section, const char *name, char *out, int n)
{
	if (_get(section, name) == YARP_FAIL)
		return YARP_FAIL;

	int i = 0;
	int tmp;
	while ( (i<n) && (fscanf(_pFile, "%x", &tmp) != EOF) )
        {
            *out = (char) tmp;
            out++;
            i++;
        }
			
	_close();
	
	if (i == n)
		return YARP_OK;		// found all the values
	else	
		return YARP_FAIL;	
}

int YARPConfigFile::getHex(const char *section, const char *name, short *out, int n)
{
	if (_get(section, name) == YARP_FAIL)
		return YARP_FAIL;

	int i = 0;
	int tmp;
	while ( (i<n) && (fscanf(_pFile, "%x", &tmp) != EOF) )
        {
            *out = (short) tmp;
            i++;
            out++;
        }
			
	_close();
	
	if (i == n)
		return YARP_OK;		// found all the values
	else	
		return YARP_FAIL;	
}

int YARPConfigFile::get(const char *section, const char *name, int *out, int n)
{
	if (_get(section, name) == YARP_FAIL)
		return YARP_FAIL;

	int i = 0;
	while ( (i<n) && (fscanf(_pFile, "%d", out++) != EOF) )
		i++;
			
	_close();
	
	if (i == n)
		return YARP_OK;		// found all the values
	else	
		return YARP_FAIL;	
}

int YARPConfigFile::get(const char *section, const char *name, unsigned int *out, int n)
{
	if (_get(section, name) == YARP_FAIL)
		return YARP_FAIL;

	int i = 0;
	while ( (i<n) && (fscanf(_pFile, "%u", out++) != EOF) )
		i++;
			
	_close();
	
	if (i == n)
		return YARP_OK;		// found all the values
	else	
		return YARP_FAIL;	
}

int YARPConfigFile::get(const char *section, const char *name, double **matrix, int n, int m)
{
	if (_get(section, name) == YARP_FAIL)
		return YARP_FAIL;

	// we assume the matrix is implemented as a C bidimensional vector
	double *out = matrix[0];

	int i = 0;
	while ( (i<n*m) && (fscanf(_pFile, "%lf", out++) != EOF) )
		i++;
			
	_close();
	
	if (i == n*m)
		return YARP_OK;		// found all the values
	else	
		return YARP_FAIL;		
}

int YARPConfigFile::getString(const char *section, const char *name, char *out)
{
	if (_get(section, name) == YARP_FAIL)
		return YARP_FAIL;

	int i = 0;
	i = fscanf(_pFile, "%s", out);
					
	_close();
	
	if (i > 0)
		return YARP_OK;		// found at least a single char
	else 
		return YARP_FAIL;
}

int YARPConfigFile::getString(const char *section, const char *name, YARPString &out)
{
	if (_get(section, name) == YARP_FAIL)
		return YARP_FAIL;

	int i = 0;
	char tmp[255];
	i = fscanf(_pFile, "%s", tmp);
	out = YARPString(tmp);
					
	_close();
	
	if (i > 0)
		return YARP_OK;		// found at least a single char
	else 
		return YARP_FAIL;
}

int YARPConfigFile::_get(const char *section, const char *name)
{
	if (!_open(_path.c_str(), _filename.c_str()))
		return YARP_FAIL;

	if (!_findSection(section))
        {
            _close();
            return YARP_FAIL;
        }

	if (_findString(name))
		return YARP_OK;
	
	_close();
	return YARP_FAIL;
}

bool YARPConfigFile::_findSection(const char *sec)
{
	char row[255];
	
	while (fscanf(_pFile, "%s", row) != EOF)
        {
            if (ACE_OS::strcmp(sec, row) == 0)
                return true;
        }

	return false;
}

bool YARPConfigFile::_findString(const char *str)
{
	char row[255];

	int l = ACE_OS::strlen(str);
	if (l == 0)
		return false;
	
	while (fscanf(_pFile, "%s", row) != EOF)
        {
            if (row[0] == '[')
                return false;		//end of section
		
            if (ACE_OS::strcmp(row, str) == 0)
                return true;
        }

	return false;
}
