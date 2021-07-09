# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2007 Alexandre Bernardino
# SPDX-FileCopyrightText: 2007 Carlos Beltran-Gonzalez
# SPDX-FileCopyrightText: 2007 Francesco Giovannini
# SPDX-License-Identifier: BSD-3-Clause

Driver for Serial communications.

This driver creates an OS independent serial communications driver for YARP.
We exploit the capacity of ACE (Adaptive Communications Environment) to provide
serial communications.

The companion network wrapper provides a "yarpdev" server for the driver.

Invocation example:

>> yarpdev --device serialport --comport COM4 --file serial.ini

The server opens two ports (defaults /serial/in and /serial/out) that receive
and send Bottles to the clients, containing the characters to send/receive
to/from the serial link.

To test the driver, you may use a rs232 9-pin plun with pins 2 and 3 shortcutted
(rx/tx) then send strings to the input port using yarp write and attach a yarp
read to the output port.
The sent strings should appear unchanged at the yarp read terminal (except for
being broken or having extra control characters from the bottle format).
