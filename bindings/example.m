% Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
% Author: Ali Paikan <ali.paikan@iit.it>
% Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


% Setting yarp binding library path
% you can avoid this if the 'yarp.oct' is already 
% the octave load path
addpath([getenv('YARP_ROOT') '/build/lib/octave']);

% Import Yarp library
yarp;

% Initialize Yarp network
yarp.Network.init();


% Create and open a port
p = yarp.BufferedPortBottle();
p.open('/octave');
ret = p.open('/octave');
if ret == 0 
    return
endif


% write some data to the port
for i=1:100
    wb = p.prepare();
    wb.clear();
    wb.addString('count');
    wb.addInt(i);
    wb.addString('of');
    wb.addInt(100);
    p.write();
    wb.toString()
    yarp.Time.delay(0.5)
endfor

% Close the port
p.close();

% Deinitialize yarp network
yarp.Network.fini();


