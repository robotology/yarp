# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2008 Giacomo Spigler
# SPDX-License-Identifier: BSD-3-Clause

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
