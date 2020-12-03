
# libffmpeg_portmonitor
## Description

The portmonitor allows you to transmit compressed videos from a source to a destination, so as to occupy less bandwidth and also be able to manipulate the output video. <br>
The portmonitor relies on the open source library of ffmpeg to be able to encode and decode video streams.
<br>
In addition to the codecs, various other parameters can be chosen from the command line (see [Parameters](#Parameters)), if not specified, default values ​​are set.

## Installation

To install/activate the portmonitor you need to enter the /pathtoyarp/yarp/build folder (if the "build" folder does not exist, create it) and run the following command:
```
cmake .. -DENABLE_yarpcar_libffmpeg=on
```
Once the command is run, make sure there is an "x" in the brackets on the libffmpeg_portmonitor line, as you can see below
```
--  [x]   Plugin: yarpcar_libffmpeg (ENABLE_yarpcar_libffmpeg)
```
Then, run the following commands to make the changes effective:
```
make
sudo make install
```
To speed up the execution of the "make" command, you can specify the number of threads to use after the "-j" parameter, such as
```
make -j4
```
to use 4 threads.

## Usage

The software allows the entry of a wide range of parameters via the command line.<br>
The string:
```
yarp connect /grabber /view <protocol>+send.portmonitor.+file.libffmpeg+recv.portmonitor+file.libffmpeg+type.dll
```
it is the basic command to connect source and receiver through the monitor port; we will refer to this string in the following as “Connection base string”. <br>
The "protocol" to be specified in the basic connection string can be for example:
-   tcp
-   fast_tcp
-   udp

It is also possible to enrich the base string with parameters following the next syntax:
```
connection_base_string+parameter_name.parameter_value
```
Concatenating to the basic connection string a "+" followed by the name of the parameter to be set and a "." followed by the value to be assigned to the parameter.
All the parameters entered in this way will be set within the compression/decompression context of the Ffmpeg library.

## Parameters

The parameters that can be set on the command line can be of different types.<br>
The encoding formats implemented are:
-   h264
-   h265
-   mpeg2video

Among the implemented codecs you can choose which one to use through the connection string, if no codec is chosen then the mpeg2video codec is used as default. <br>
Parameters can be used to manipulate image quality. Specifically, the "qmin" and "qmax" parameters are used in pairs to obtain the best quality, at the expense of the bandwidth.
The best parameters for each codec are defined in [this page](https://slhck.info/video/2017/02/24/vbr-settings.html) and reported in the following table.
| Codec       | qmin        | qmax        |
| ----------- | ----------- | ----------- |
| h264        | 18          | 28          |
| h265        | 24          | 34          |
| mpeg2video  | 3           | 5           |

Alternatively, the "crf" parameter can be used. We always find the best values ​​on the same page.

| Codec       | crf         |
| ----------- | ----------- |
| h264        | 23          |
| h265        | 28          |
| mpeg2video  | 3           |

The values ​​of the crf allow to improve the quality of the image. The lower the crf value, the better the image quality and vice versa. <br>
Furthermore, changing the value of the "crf" parameter as well as of "qmin" and "qmax" indirectly affects the occupied bandwidth and fps, making the encoding VBR (variable bit rate).

## Example

A very simple example is to use, for the portmonitor, as input the test_grabber device and as output the view. <br>
Using different terminals the next procedure can be followed: <br>
### Terminal 1
```
yarp server
```

### Terminal 2
```
yarpdev --device test_grabber --mode grid
```

### Terminal 3
```
yarpview --name /view
```

### Terminal 4
```
yarp connect /grabber /view fast_tcp+send.portmonitor+file.libffmpeg+recv.portmonitor+file.libffmpeg+type.dll
```
<br>
The result is a line moving from top to bottom in a colored space, as seen in the following image.
<br><br>

![testgrabber](Img/testgrabber.png)
<br>
The terminals can be on different machines, as long as the IP address and port is set in the yarp configuration of each machine where the command
```
yarp server
```
is executed.<br>
To check the configuration you can use the command:
```
yarp conf
```
which returns the file location to change the IP address and port.
<br><br>
Installing the necessary plugins and following the same procedure, you can for example play videos saved on the machine by running the following command (instead of the terminal 2 command)
```
yarpdev --device ffmpeg_grabber --source path/to/file
```
or even see live the video stream of a video camera, such as that of the gazebo simulator.
