# Copyright: (C) 2010 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

How to make a YARP release:
--------------------------

* Use a linux machine

* Look at:
    http://dashboard.icub.org/index.php?project=YARP
  If there are any issues, resolve them and delay release a few days.

* Check that copyright holder and licensing information is up to date.
  Run:
    ./scripts/admin/update-license
    # check license-statement.txt for sanity

* If license-statement.txt is correct, do:
    cp license-statement.txt COPYING

* Update version number in conf/YarpVersion.cmake and add a ChangeLog
  entry saying "bump" on that file

* Commit.

* Do something like this:

svn copy https://yarp0.svn.sourceforge.net/svnroot/yarp0/trunk/yarp2  https://yarp0.svn.sourceforge.net/svnroot/yarp0/tags/yarp-2.3.1 -m "yarp release 2.3.1" and note revision number.

./scripts/admin/make-source-package-svn yarp-2.3.1


* Look at this page to get the current YARP version, say 2.1.6:

  http://eris.liralab.it/yarp/specs/dox/download.html

* For practice, run (replacing 2.1.6 with current version):

  ./scripts/check-public-version.sh 2.1.6

* That checks that everything in the public version compiles ok.

* Make sure that conf/YarpVersion.cmake is different to the latest
  public version, bumping up the version number if needed.

* To update shadow autoconf/automake file, from $YARP_ROOT run:

  ./scripts/update-automake 

* New instructions:

  rm -rf license_check
  ./scripts/update-license
  # check license-statement.txt for sanity
  cp license-statement.txt COPYING
  svn copy https://yarp0.svn.sourceforge.net/svnroot/yarp0/trunk/yarp2  https://yarp0.svn.sourceforge.net/svnroot/yarp0/tags/yarp-2.X.X -m "yarp release 2.X.X"
  ./scripts/make-source-package-svn 2.2.6
