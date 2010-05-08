// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef __YARPOSH__
#define __YARPOSH__

/*
* Copyright (C) 2009 RobotCub Consortium
* Author: Lorenzo natale
* CopyPolicy: Released under the terms of the GNU GPL v2.0.
*
*/

namespace yarp {
	namespace os {
		enum 
		{
			YARP_SIGTERM,
			YARP_SIGINT,
		};

		/**
		* Wrapper for the ACE_OS::signal signal.
		* Attach a function handler to a signal.
		* @param signum: signal number (e.g. SIGTERM/SIGINT) to handle
		* @param sighandler: handler function
		*/
        typedef void(*YarpSignalHandler)(int);
		YarpSignalHandler signal(int signum, YarpSignalHandler sig);

		/**
		* Wrapper for ACE_OS::exit().
		*/
		void exit(int v);

		/**
		* Wrapper for ACE_OS::getenv().
		* @param v: string that containt the environment variable name
		* @return the value corresponding to the envarionment variable v
		*/
		const char *getenv(const char *v);

		/**
		* Wrapper for ACE_OS::mkdir(). Create a directory.
		* @param p: name of the new directory.
		*/
		int mkdir(const char *p);

		/**
		* Wrapper for ACE_OS::stat() function.
		*/
		int stat(const char *f);
	}	     
}

#endif
