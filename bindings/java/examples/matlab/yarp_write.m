% SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
% SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
% SPDX-License-Identifier: BSD-3-Clause

% show how to call YARP from Matlab.
% Write a bottle (quivalent to yarp write)

yarp.matlab.LoadYarp;
import yarp.Port;
done=0;

port=Port;
%first close the port just in case
port.close;

disp('Going to open port /matlab/write');
port.open('/matlab/write');

disp('Please connect to a bottle sink (e.g. yarp read)');

b=Bottle;
while(~done)
  reply = input('Write a string (''quit'' to quit):', 's');

  b.fromString(reply);

  port.write(b);

  if (strcmp(reply, 'quit'))
    done=1;
  end
end

port.close;
