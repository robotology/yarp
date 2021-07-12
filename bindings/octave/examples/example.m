% SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
% SPDX-License-Identifier: BSD-3-Clause

% Setting yarp binding library path
% you can avoid this if the 'yarp.oct' is already
% the octave load path
addpath([getenv('YARP_ROOT') '/build/lib/octave']);

% Import YARP library
yarp;

% Initialize YARP network
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
    wb.addInt32(i);
    wb.addString('of');
    wb.addInt32(100);
    p.write();
    wb.toString()
    yarp.Time.delay(0.5)
endfor

% Close the port
p.close();

% Deinitialize yarp network
yarp.Network.fini();
