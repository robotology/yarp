# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

This program simulates a robot head looking out to sea at a boat.

On Linux type:

   cmake .
   make
   # Edit fakebot.ini and change USERNAME to something you like
   ./run_fakebot --file fakebot.ini

On Windows with MSVC

   Use cmake and your compiler.
   Copy run_fakebot.exe from the Debug or Release subdirectory to the
   current directory (with fakebot.ini, back.ppm, and fore.ppm in it).
   Edit fakebot.ini and change USERNAME to something you like
   Run the program as "run_fakebot --file fakebot.ini"

Now you can run a copy of yarpview and connect it to /USERNAME/fakebot/camera

The simulated camera can be moved around by sending commands to
/USERNAME/fakebot/motor/rpc:i
