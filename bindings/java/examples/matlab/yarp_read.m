% SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
% SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
% SPDX-License-Identifier: BSD-3-Clause

% show how to call YARP from Matlab.
% Write a bottle (quivalent to yarp write)

yarp.matlab.LoadYarp;

done=0;

port=Port;
%first close the port just in case
port.close;

disp('Going to open port /matlab/read');
port.open('/matlab/read');

disp('Please connect to a bottle sink (e.g. yarp write)');
disp('The program closes when ''quit'' is received');

b=Bottle;
while(~done)
  port.read(b);
  disp(b);

  if (strcmp(b.toString, 'quit'))
    done=1;
  end
end

port.close;
