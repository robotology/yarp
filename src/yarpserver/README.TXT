This is an experimental version of the yarp name server that uses a
real database (sqlite) to maintain the state of the yarp network.

It has the functionality of the standard name server, but is not well 
tested.

====================================================================
COMPILATION
===========

To compile:
  cmake .
  make

This builds three programs:
  yarpserver3
  server_peek
  server_test

The first, yarbserver3, is intended to be a drop in replacement
for the regular "yarp server" / "yarpserver" program.
(currently it will store a database in a file called "yarp.db"
in your working directory -- *don't* run on NFS!)

The second, server_peek, is a command line program for debugging
 to manipulate the database used by yarpserver3.

The third, server_test, runs some basic tests against a yarp name
server (the original, or this new one).

Examples of how to use and setup command line tests are available in
the ctrl.sh script.  The "sqlite3" program can be helpful for 
debugging.


====================================================================
DOCUMENTATION
=============

Run doxygen using the "doxygen.conf" file.  Then open the page:
  html/index.html
in a browser.


====================================================================
TESTING
=======

Kill all yarp programs. Now, on a single computer:

Run
  ./yarpserver3

Now do anything you would normally do with YARP.  For example,
try "yarp check", or:

Run
  yarp read /read

Run 
  yarp write /write

Run
  yarp connect /write /read


====================================================================
MISSING FEATURES
================

None known.

