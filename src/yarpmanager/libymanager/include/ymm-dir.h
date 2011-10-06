// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 * 
 */


#ifndef __YMMDIR__
#define __YMMDIR__

//namespace ymm {

#include <ace/OS.h>
	
#if defined(WIN32) || defined(WIN64)
	#define PATH_SEPERATOR		"\\"
#else
	#define PATH_SEPERATOR		"/"
#endif 


//}
#endif //__YMMDIR__
