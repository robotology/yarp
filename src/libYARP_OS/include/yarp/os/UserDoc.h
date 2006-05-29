// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef _YARP2_USERDOC_
#define _YARP2_USERDOC_

/**
 * @mainpage Welcome to YARP
 *
 * @section welcome_disclaimer Disclaimer
 *
 * \code
       Through Me Pass into the Painful City,
       Through Me Pass into Eternal Grief,
       Through Me Pass among the Lost People.
       Justice Moved My Master-Builder:
       Heavenly Power First Fashioned Me
       With Highest Wisdom and with Primal Love.
       Before Me Nothing Was Created That
       Was Not Eternal, and I Last Eternally.
       All Hope Abandon, You Who Enter Here.
 * \endcode
 *
 * @section welcome_topics Topics
 *
 * @ref what_is_yarp
 *
 * @ref requirements
 *
 * Need help installing and compiling YARP?  See @ref install.
 *
 * Want to start using the library?  Start with @ref yarp_os.
 *
 */

/**
 * @page what_is_yarp What is YARP?
 *
 * YARP is written by and for researchers in robotics, particularly
 * humanoid robotics, who find themselves with a complicated pile of
 * hardware to control with an equally complicated pile of
 * software. At the time of writing (2005), running decent visual,
 * auditory, and tactile perception while performing elaborate motor
 * control in real-time requires a lot of computation. The easiest and
 * most scalable way to do this right now is to have a cluster of
 * computers. Every year what one machine can do grows, but so do our
 * demands. YARP is a set of tools we have found useful for meeting
 * our computational needs for controlling various humanoid robots.
 *
 * The components of YARP can be broken down into:

    * libYARP_OS - interfacing with the operating system(s) to support
    * easy streaming of data across many threads across many
    * machines. YARP is written to be OS neutral, and explicitly
    * supports Linux, Microsoft Windows, Mac Os (aka Darwin) and the
    * QNX realtime operating system.

    * libYARP_sig - performing common signal processing tasks (visual,
    * auditory) in an open manner easily interfaced with other
    * commonly used libraries.

    * libYARP_sig_logpolar - performing the logpolar subsampling of
    * images. This is the author's preference when doing image
    * processing for robots. It offers full field of view with reduced
    * number of pixels to process. For a full reference please check
    * the relative manual.

    * libYARP_dev - interfacing with common devices used in robotics:
    * framegrabbers, digital cameras, motor control boards, etc.

    * libYARP_robot - common interface to different robotic platform
    * with easy to replace device drivers as in the libYARP_dev
    * library.

    * tools - various support tools, utilities, and executable components.

    * These components are maintained separately. The core component
    * is libYARP_OS, which must be available before the other
    * components can be used. This manual describes how to install and
    * use that component first, then the others.

 *
 */


/**
 *
 * @page requirements Requirements

 * YARP is tested on Windows (2000/XP), Linux (Debian/SuSE), MacOs
 * (Darwin), and QNX6. It is based on the open-source ACE (ADAPTIVE
 * Communication Environment) library, which is portable across a very
 * broad range of environments, and YARP should inherit that
 * portability. YARP is written almost entirely in C++.

 * For real-time operation, network overhead has to be minimized, so
 * YARP is designed to operate on an isolated network or behind a
 * firewall. If you expose machines running YARP to the internet,
 * expect your robot to one day be commanded to make a crude gesture
 * at your funders by a script kiddie in New Zealand (or, if you are
 * in New Zealand, New York).

 * For interfacing with hardware, we are at the mercy of which
 * operating systems particular companies choose to support - few are
 * enlightened enough to provide source. The libYARP_dev library is
 * structured to interface easily with vendor-supplied code, but to
 * shield the rest of your system from that code so that future
 * hardware replacements are possible. Check the requirements imposed
 * by your current hardware; YARP will not reduce these, only make
 * future changes easier.

 * With the same spirit the libYARP_robot tries to provide a common
 * interface to different robotic platforms. Still the idea is that of
 * shielding higher level code from the nitty-gritty details of the
 * robot hardware and configuration. Porting to a new platform is not
 * guaranteed to be anything trivial but for the reuse of higher level
 * code. The amount of recycling clearly depends on the differences
 * among control cards, frame grabbers, etc. Unfortunately, this
 * doesn't shield you from knowing the platform you're going to work
 * on, but we bet you need to know it anyway.

 * YARP has consequently three levels of configuration: operating
 * system, hardware, and robot level. The first level of configuration
 * should concern you only if you're planning to compile YARP on a new
 * operating system. There are a few places where to intervene that we
 * chose not to relay to ACE (for now!) since they would have required
 * fairly complex contructs or, for instance, CORBA data types and
 * functions (e.g. dealing with endianism). This means you have to
 * prepare yourself a few lines of code. Details are provided later on
 * in this manual.

 * The second level is the hardware. A new addition on an existing
 * platform or a new platform altogether might require preparing a few
 * YARP device drivers. These are to all effects C++ classes that
 * support the methods for accessing the hardware which is normally
 * implemented through function calls to whatever provided by the
 * hardware vendor. This comes typically in the form of either a DLL
 * or a static library.

 * Finally, you might need to prepare the configuration files for a
 * new robotic platform and unfortunately this requires fiddling with
 * the libYARP_robot library.

 * You should not be scared by tweaking into YARP's code since the
 * level of customization required is fairly simple and hopefully well
 * documented. Once the appropriate classes are in place the
 * compilation and link to application and tools should proceed
 * smoothly - or at least this has been our experience so far. In
 * fact, YARP has been compiled and presently runs on four different
 * operating systems - meaning that it can run on a heterogeneous
 * cluster of PCs - and it has been customized to run on something
 * like six different robots.
 *
 */

/**
 * @page install Installation Tips
 *
 * @section install_start Getting started
 *
 * YARP is available from http://yarp0.sourceforge.net
 *
 * We suggest that, if you are a first-time user of YARP, you install
 * it on one or two machines first, before trying to set it up for a
 * cluster.

 * To set YARP up on a single machine:
 * \li @ref install_ace
 * \li Install YARP :-)
 *
 * Then test your installation before moving on to
 * other YARP components.
 *
 *
 */


/**
 * @page install_ace Install the ACE library
 *
 * @section intro_install_ace The general approach
 *
 * Obtain ACE at: http://deuce.doc.wustl.edu/Download.html
 *
 * (you only need ACE, not TAO or any other mysterious acronym that
 * might be offered to you).
 *
 * Find build instructions for your OS at:
 * http://www.cs.wustl.edu/~schmidt/ACE_wrappers/ACE-INSTALL.html
 *
 * We require ACE version 5.3.3 or greater (but we recommend
 * 5.4.2). ACE should compile fine just by following the installation
 * instructions that can be found on the ACE website
 * (http://www.cs.wustl.edu/~schmidt/ACE.html).

 * @section debian_install_ace On Debian

 * On Debian-based linux machines, ACE is available as a binary
 * package; simply type ``apt-get install libace-dev'' as
 * super-user. Please check (e.g. by searching at
 * http://packages.debian.org) that the version available is 5.3.3 or
 * greater. At the time of writing, this is true for Debian unstable
 * but not true for Debian stable.

 * @section windows_install_ace With Visual Studio

 * On Windows you can unzip the ACE distribution and simply open the
 * workspace file contained within it from within Visual Studio and
 * then start the compilation.  We recommend you compile twice,
 * once in debug mode and release mode, so you have both versions of
 * the library available later.
 * 
 */

/**
 * @page yarp_os The YARP OS library
 *
 * @section intro_sec Introduction
 *
 * Welcome to the YARP OS library.  This library interfaces
 * with your operating system and provides some basic services.
 * We give you
 * \link yarp::os::Thread Thread\endlink, 
 * \link yarp::os::Semaphore Semaphore\endlink, and 
 * \link yarp::os::Time Time\endlink
 * services just in case
 * you don't have them, but our main goal is to give you easy network
 * communication using the YARP 
 * \link yarp::os::Port Port\endlink
 * Network.
 * 
 * The links at the top of this page let you explore the classes
 * provided in the YARP OS library.  To install and test yarp,
 * see our website, http://yarp0.sourceforge.net
 *
 * @section install_sec What is a Port?
 *
 * A \link yarp::os::Port Port\endlink is an object that can read and
 * write values to peer objects spread throughout a network of
 * computers.  You can create them in your program, and then add and
 * remove connections either from that program, from the command line,
 * or from another program.
 * Ports are specialized for streaming communication, such as camera
 * images or motor commands.  You can switch network protocols for any
 * or all your connections without changing a line of code.
 *
 * @section example_sec A simple code example
 *
 * Here we work through an example of using YARP communication.
 * First, here are two programs, a sender and receiver, let's call
 * them sender.cpp and receiver.cpp
 *
 * @subsection example_sender A program for sending a message
 * \code
 // source for sender.cpp
 #include <yarp/os/Network.h>
 #include <yarp/os/Port.h>
 #include <yarp/os/Bottle.h>
 #include <yarp/os/Time.h>
 #include <stdio.h>
 using namespace yarp::os;
 int main() {
 Network::init();
 Bottle bot1; 
 bot1.addString("testing"); // a simple message
 Port output;
 output.open("/out");
 for (int i=0; i<100; i++) {
 output.write(bot1);
 printf("Sent message: %s\n", bot1.toString().c_str());
 bot1.addInt(i); // change the message for next time
 Time::delay(1);
 }
 output.close();
 Network::fini();
 return 0;
 }
 * \endcode
 *
 * @subsection example_receiver A program for receiving a message
 *
 * \code
 // source for receiver.cpp
 #include <yarp/os/Network.h>
 #include <yarp/os/Port.h>
 #include <yarp/os/Bottle.h>
 #include <stdio.h>
 using namespace yarp::os;
 int main() {
 Network::init();
 Bottle bot2;
 Port input;
 input.open("/in");
 input.read(bot2);
 printf("Got message: %s\n", bot2.toString().c_str());
 input.close();
 Network::fini();
 return 0;
 }
 * \endcode
 *
 * @subsection example_compiling Compiling the examples
 *
 * To compile these programs you need two libraries: yarp and ace.
 * These instructions assume you have installed them - if not, see
 * their websites for instructions.  The source of these programs
 * and a makefile are in the directory src/libYARP_OS2/examples
 *
 * If you're on a UNIX machine, you can compile with:
 * \code
 g++ receiver.cpp -o receiver -lACE -lYARP_OS
 g++ sender.cpp -o sender -lACE -lYARP_OS
 * \endcode
 *
 * On windows, you'll need to set up projects and set up include
 * paths and libraries appropriately.
 *
 * @subsection example_running Running the examples
 *
 * On UNIX, on four separate consoles, do:
 * \code
 yarp server
 ./sender
 ./receiver
 yarp connect /out /in
 * \endcode
 *
 * (You may have to give a path to wherever yarp is compiled if it is
 * not in your path). The process on windows in similar.
 *
 */


#endif
