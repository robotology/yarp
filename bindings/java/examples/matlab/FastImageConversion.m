% SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
% SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
% SPDX-License-Identifier: BSD-3-Clause

function FastImageConversion()

    % Example code to show how to read an image from a YARP source, convert it
    % to matlab matix image using a fast method convertion (Leo Pape) and back
    % to a yarp image in order to send it to some ports
    %
    % run the example
    % connect images to input - e.g. yarp connect /icub/camcalib/left/out /matlab/img:i
    % connect images to output to yarpview - e.g. yarp connect /matlab/img:o /viewOut
    % connect write port to start and quit - e.g. yarp write ... /matlab/read
    %
    % use any type of message to start eg. "go"
    % when convertion is finished type "quit" to close ports nicely



    % initialize YARP
    yarp.matlab.LoadYarp;
    import yarp.BufferedPortImageRgb
    import yarp.BufferedPortBottle
    import yarp.Port
    import yarp.Bottle
    import yarp.Time
    import yarp.ImageRgb
    import yarp.Image
    import yarp.PixelRgb
    done=0;
    b=Bottle;

    %creating ports
    port=BufferedPortBottle;        %port for reading "quit" signal
    portImage=BufferedPortImageRgb; %Buffered Port for reading image
    portFilters=Port;               %port for sending image
    %first close the port just in case
    %(this is to try to prevent matlab from beuing unresponsive)
    port.close;
    portImage.close;
    portFilters.close;
    %open the ports
    disp('opening ports...');
    port.open('/matlab/read');
    disp('opened port /matlab/read');
    pause(0.5);
    portImage.open('/matlab/img:i');
    disp('opened port /matlab/img:i');
    pause(0.5);
    portFilters.open('/matlab/img:o');
    disp('opened port /matlab/img:o');
    pause(0.5);
    disp('done.');


    finishup = onCleanup(@() closePorts(portImage,port,portFilters));


    while(~done)%run until you get the quit signal

         b = port.read( false );%use false to have a non blocking port
         if (sum(size(b)) ~= 0) %check size of bottle
             disp('received command: ');
             disp(b);
             %checking for quit signal
             if (strcmp(b.toString, 'quit'))
                 break;
             end
         end

         %disp('getting a yarp image..');
         yarpImage=portImage.read(false);%get the yarp image from port
         if (sum(size(yarpImage)) ~= 0) %check size of bottle
             %disp('got it..');
             h=yarpImage.height;
             w=yarpImage.width;
             pixSize=yarpImage.getPixelSize();
             tool=yarp.matlab.YarpImageHelper(h, w);
             tic %start time
             IN = tool.getRawImg(yarpImage); %use leo pape image patch
             TEST = reshape(IN, [h w pixSize]); %need to reshape the matrix from 1D to h w pixelSize
             COLOR=uint8(zeros(h, w, pixSize)); %create an empty image with the correct dimentions
             r = cast(TEST(:,:,1),'uint8');  % need to cast the image from int16 to uint8
             g = cast(TEST(:,:,2),'uint8');
             b = cast(TEST(:,:,3),'uint8');
             COLOR(:,:,1)= r; % copy the image to the previoulsy create matrix
             COLOR(:,:,2)= g;
             COLOR(:,:,3)= b;


             time = toc;
             fprintf('receiving a yarp image took %f seconds \n', time);
             %
             % Do any type of processing
             %
             tic
             %send it back to yarp
             img = yarp.ImageRgb(); %create a new yarp image to send results to ports
             img.resize(w,h);   %resize it to the desired size
             img.zero();        %set all pixels to black
             COLOR = reshape(COLOR, [h*w*pixSize 1]); %reshape the matlab image to 1D
             tempImg = cast(COLOR ,'int16');   %cast it to int16
             img = tool.setRawImg(tempImg, h, w, pixSize); % pass it to the setRawImg function (returns the full image)
             portFilters.write(img); %send it off
             time = toc;
             fprintf('converting back to yarp took %f seconds \n', time);
         end

        pause(0.01);
    end

end

function closePorts(portImage,port,portFilters)

    disp('Going to close the port');
    portImage.close;
    port.close;
    portFilters.close;

end
