Legal
-----
The files "sfun_time.c" "utility.mdl" are originally part of Simulink® Real Time Execution
by Guy Rouleau (http://www.mathworks.com/matlabcentral/fileexchange/authors/31651)
27 Oct 2008 (Updated 22 Jul 2010), downloaded 22 Oct 2012 from:
http://www.mathworks.com/matlabcentral/fileexchange/21908-simulink%C2%AE-real-time-execution
The licence of Simulink® Real Time Execution is attached as "license.txt" in this directory.

These three YARP/Simulink examples have been developed by Juan G. Victores
(http://roboticslab.uc3m.es/roboticslab/persona.php?id_pers=72), licenced as the rest of
the YARP Repository (http://www.yarp.it/index.html) from where this
source can be downloaded.

Install
-------
Run the following command in MATLAB for the examples to work:
mex sfun_time.c

You also need to have YARP integrated into MATLAB. Make sure you can execute the following from MATLAB:
LoadYarp

Running
-------
Have a yarp server running. Additional dependencies can be solved with the following terminal commands:
* yarpWriteDoubles: yarp read /read
* yarpReadDoubles: yarp write /write
* yarpRemotePosEnc: yarpdev --device fakeMotionControl
* yarpRemoteVelEnc: yarpdev --device fakeMotionControl
