% SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
% SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
% SPDX-License-Identifier: BSD-3-Clause

% Example code to show how to read an image from a YARP source and
% handle it within matlab

%initialize YARP (it seems ok to call this more than once...)
yarp.matlab.LoadYarp;
import yarp.BufferedPortImageRgb
port=BufferedPortImageRgb;

disp('Registering port /matlab/sink');
port.close; %make sure the port is closed, calling open twice hangs
            %(we should fix this
port.open('/matlab/sink');

disp('Please connect the port /matlab/sink to an image source');
yarpImage=port.read;
h=yarpImage.height;
w=yarpImage.width;
%now we need to convert the yarpImage (a Java object) into a matlab matrix
tool=yarp.matlab.YarpImageHelper(h, w);

% convert to color image, for efficiency reasons this method returns
% a two-dimensional matrix that needs to be 'unpacked'
TMP=tool.get2DMatrix(yarpImage);
COLOR=uint8(zeros(h, w, 3));

COLOR(:,:,1)=TMP(:,1:w);
COLOR(:,:,2)=TMP(:,w+1:2*w);
COLOR(:,:,3)=TMP(:,2*w+1:3*w);

% convert to a grayscale image, for efficiency reasons this method
% is provided separately
MONO=uint8(zeros(h, w, 3));
TMP=tool.getMonoMatrix(yarpImage);

MONO(:,:,1)=TMP;
MONO(:,:,2)=TMP;
MONO(:,:,3)=TMP;

% extract R,G,B planes, for efficiency reasons these methods
% are provided separately
R=uint8(zeros(h,w));
G=R;
B=R;

R=tool.getR(yarpImage);
G=tool.getG(yarpImage);
B=tool.getB(yarpImage);

%display
figure(1), subplot(2,3,1),  image(COLOR), title('Color image');
figure(1), subplot(2,3,2),  image(MONO), title('Mono image');
figure(1), subplot(2,3,4), colormap(gray), image(R), title('R plane');
figure(1), subplot(2,3,6), colormap(gray), image(G), title('G plane');
figure(1), subplot(2,3,5), colormap(gray), image(B), title('B plane');

disp('Going to close the port');
port.close;
