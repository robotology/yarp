# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2008 Giacomo Spigler
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

SerialServoBoard:
  Driver to interface with the following Servo Control Boards, for home-based (cheap) Robotics:
  -SSC32 = ssc32
  -MiniSSC (II) = minissc
  -Pontech SV203X = pontech_sv203x
  -Mondotronic SMI = mondotronic_smi
  -Pololu USB 16-Servo Controller = pololu_usb_16servo
  -PicoPic = picopic

Requirements:
  The "serialport" module must be compiled in order to use this driver.
