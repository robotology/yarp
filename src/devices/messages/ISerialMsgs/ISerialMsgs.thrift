/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

struct yReturnValue {
} (
  yarp.name = "yarp::dev::ReturnValue"
  yarp.includefile = "yarp/dev/ReturnValue.h"
)

struct return_flush {
  1: yReturnValue retval;
  2: i32 flushed_bytes;
}

struct return_receiveString {
  1: yReturnValue retval;
  2: string message;
}

struct return_receiveBytes {
  1: yReturnValue retval;
  2: list<byte> message;
}

struct return_receiveLine {
  1: yReturnValue retval;
  2: list<byte> message;
}

struct return_receiveByte {
  1: yReturnValue retval;
  2: byte message;
}

service ISerialMsgs
{
    yReturnValue  sendString (1:string message);
    yReturnValue  sendBytes  (1:list<byte> message);
    yReturnValue  sendByte   (1:byte message);

    return_receiveString  receiveString ();
    return_receiveBytes   receiveBytes  (1:i32 maxNumberOfByes);
    return_receiveByte    receiveByte   ();
    return_receiveLine    receiveLine   (1:i32 maxNumberOfByes);

    yReturnValue  setDTR (1:bool enable);
    yReturnValue  flush ();
    return_flush  flushWithRet ();
}
