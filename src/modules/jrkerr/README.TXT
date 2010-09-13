# Copyright: (C) 2009 RobotCub Consortium
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

YARP2 DRIVER FOR JRKERR CONTROL BOARDS
WIN32 VERSION - STILL INCOMPLETE

HOWTO TEST:

yarp server
yarpdev --device jrkerr --file y:/icub/conf/chico_head.ini
yarp rpc /controlboard/rpc:i

// In the yarp rpc interpreter do the following
 // initializations
 set aen <axis>                  //enable the axis amplifier
 set ena <axis>			 //initializes PIDs
 set vel <axis> <max_speed>      //sets maximum speed to <max_speed> [deg/sec]
 set acc <axis> <max_accel>      //sets maximum acceleration to <max_accel> [deg/sec^2]
 // commands
 set pos <axis> <set_point>      //moves axis to position <set_point> [deg]
 set vmo <axis> <set_point>      //moves axis with velocity <set_point> [deg/sec]

PROBLEMS:
 Specifying <axis> out of bound crashes.
 yarp rpc command 'off' crashes

-------------------------------------------------

Some data about the jrkerr boards:

- Current sense - 39 counts per ampere
- Servo rate - 1953.125 Hz
- Servo tick time - 0.512 ms
- Max baud rate - 230400
- Max Delay for acknowledge (status): 0.51 ms.
- Output limit [0 255] - 255 corresponds to the max voltage
- servo rate divisor (SRD) - number of samples to compute derivative term
   averages derivative error term and reduces digitation noise
- Quadrature encoders have 4*L counts (L=encoder lines).
- Position is specified in counts (signed 32 bit integer: -2,147,483,648 to    +2,147,483,647).
- Velocity is specified in counts/tick * 65536 (positive 32 bit integer: 0 to +83,886,080).
- Acceleration is specified in counts/tick/tick * 65536 (positive 32 bit integer: 0 to +2,147,483,647).
- A tick is 0.512 ms.
- Control gains KP, KD, KI, IL, EL - (positive 16 bit integer: 0 to 32767)
- Control settings OL, DB - (0 to 255)
- Control setting SM - (1 to 255)
- Control setting CL - (0 to 255). Odd values CUR_SENSE proportional to motor current, even values CUR_SENSE inversely proportional to motor current.
