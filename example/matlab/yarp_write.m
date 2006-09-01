% show how to call YARP from Matlab.
% Write a bottle (quivalent to yarp write)
% -nat

LoadYarp;

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
  
  
  