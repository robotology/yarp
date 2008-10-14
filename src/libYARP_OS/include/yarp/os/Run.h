// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Alessandro Scalzo, Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

/**
 * @class Run
 *
 * @brief Run
 *
 * yarp run provides a server able to run/kill commands/scripts on a remote machine.
 * 
 * To run a server on a machine:             $ yarp run --server SERVERPORT
 *
 * To run a script by the remote server:     $ yarp run --on SERVERPORT --as TAG --script SCRIPTNAME
 * (in this case the script is copied and executed on the remote server)
 *
 * To run a command by the remote server:    $ yarp run --on SERVERPORT --as TAG --cmd COMMAND [ARGLIST]
 * 
 * To kill a command/script:                 $ yarp run --on SERVERPORT --kill TAG
 *
 * To kill all commands/scripts on a server: $ yarp run --on SERVERPORT --killall
 * 
 * To clean shutdown a server:				 $ yarp run --on SERVERPORT --exit
 */

#ifndef _YARP2_RUN_
#define _YARP2_RUN_

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace yarp {
  namespace os {
    class Run;
  }
}


// watch out for ACE randomly redefining main, depending on configuration
#ifdef main
#undef main
#endif

/**
 * Starting and stopping processes.
 * Experimental.  Use and lose.
 */
class yarp::os::Run {
public:
    static int main(int argc, char *argv[]);

protected:
	static void printHelp();
	static bool checkBash(const ConstString& workdir);
	static int run();
	static int runServerBash(const ConstString& portname,
                             const ConstString& workdir);
#ifdef WIN32
	static int runServerDos(const ConstString& portname,
                            const ConstString& workdir);
#endif
	static int runConnect(Searchable& config);
	static int runClient(Searchable& config);
	static int report(Searchable& config);
};

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/


#endif
