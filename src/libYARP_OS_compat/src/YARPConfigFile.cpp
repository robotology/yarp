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
///                    #Add our name(s) here#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
///  $Id: YARPConfigFile.cpp,v 1.1 2006-03-13 12:52:42 eshuy Exp $
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
