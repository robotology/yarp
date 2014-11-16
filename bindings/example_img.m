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
p = yarp.BufferedPortImageRgb();
ret = p.open('/img/in');
if ret == 0 
    return
endif


% read a single-frame image data from 
% the port and show it using imshow
img = p.read();
img.width()
img.height()
I = [];

%TODO: we need to avoid this dirty image copy
for x=1:img.width()
    for y=1:img.height()
        I(y, x, 1) = img.pixel(x-1, y-1).r/255;
        I(y, x, 2) = img.pixel(x-1, y-1).g/255;
        I(y, x, 3) = img.pixel(x-1, y-1).b/255;
    endfor
endfor

bw = edge(rgb2gray(I));

figure;
subplot(1,2,1);
imshow(I);
subplot(1,2,2);
imshow(bw);

% Close the port
p.close();

% Deinitialize yarp network
yarp.Network.fini();


