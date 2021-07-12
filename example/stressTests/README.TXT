# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

Lorenzo Natale's bug hunting:

====================================================================


In $YARP_ROOT/example/stressTests you find some code + a couple of
scripts that produce some undesired behavior. It is basically a
remote_controlboard accessed concurrently from a number of controlboard
(stressrpc.cpp). Code and scripts are easy, so I don't go too much in
the details, I just summarize here the behavior I see:

./stress.sh works fine
./stress2.sh works fine, but at the end files to quit yarpdev, just have
a look at the output of ps (this is the undefined behavior I see)
./stress3.sh works fine

====================================================================

Lorenzo:

I have uploaded a stressrpcMD.cpp (MD is for motion done). If you run
the fakeMotionControl device like this:

yarpdev --device controlboard --subdevice fakeMotor --delay 80

and then two instances of stressrpcMD.cpp as:

console1:
stressrpcMD --id 0

console 2
stressrpcMD --id 1

you *should* hopefully see that sometimes one of the two (the second?)
hangs within the "checkMotionDone" call (but sometimes even in the port
"open" call). The problem is more frequent if you kill and restart one
of the two clients. When this happens the fakeMotor device does not
close properly unless the clients are killed (similarly to the previous
bugs I have recently reported)

I believe the problem has to do with timing. In fact it took me a while
to replicate it in the fakeMotor device. The problem on the robot was
triggered very reliably in the getPid function which took 70-90ms to
complete. For this reason I have added a Time::delay() in the
checkMotionDone function of the fakeMotor device (this delay can be
changed with the --delay parameter).  Caveats: the problem happens often
but with variable likelihood, the --delay 80 appeared to trigger it more
often, but I'm no longer sure. Maybe adding the sleep was enough to make
the problem more probable and I got fooled into thinking the "80" number
was more important than it is... I don't know.

Anyway let's first see if you can reproduce the problem... it might be
machine dependent.

====================================================================

Paul:

I've added a stress test that doesn't use the controlboard: "smallrpc"
  smallrpc --server
  smallrpc --client --name /client0
  smallrpc --client --name /client1
  smallrpc --client --name /client2
