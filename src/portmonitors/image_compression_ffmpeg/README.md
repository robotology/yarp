
# image_compression_ffmpeg portmonitor
## Description

This port monitor allows you to compress a video stream while transmitting it from a source YARP port to a destination YARP port. The port monitor relies on the open source library Ffmpeg to encode and decode video streams.
<br>
You can choose the desired codec using the command line, along with numerous other possible parameters (see [Parameters](#Parameters)).
If the "codec" parameter is not set, it will be set to a default value.

## Installation

To install/activate the port monitor you need to enter the <yarp_root>/build folder in a terminal (if the "build" folder does not exist, create it) and run the following command:
```
cmake .. -DENABLE_yarppm_image_compression_ffmpeg=on
```
Once the command is run, make sure there is an "x" in the brackets on the image_compression_ffmpeg portmonitor line in the command output in terminal, as you can see below:
```
--  [x]   Plugin: yarppm_image_compression_ffmpeg (ENABLE_yarppm_image_compression_ffmpeg)
```
Then, run the following commands to make the changes effective:
```
make
sudo make install
```
To speed up the execution of the "make" command, you can specify the number of recipes to execute at once after the "-j" parameter, such as
```
make -j4
```
to execute 4 recipes at a time.

## Usage

The software allows the insertion of a wide range of parameters via the command line.<br>
The string:
```
yarp connect /grabber /view <protocol>+send.portmonitor.+file.image_compression_ffmpeg+recv.portmonitor+file.image_compression_ffmpeg+type.dll
```
is the basic command to connect source and receiver through the port monitor; we will refer to this string in the following as “Connection base string”. <br>

The \<protocol\> section in the connection base string represents the protocol used for the transmission between sender and receiver; for example, it can be:
-   tcp
-   fast_tcp
-   udp

<br>
In order to insert other parameters into the connection string, you have to follow the syntax below:<br>

```
connection_base_string+parameter_name.parameter_value
```
concatenating a "+" to the connection base string, followed by the name of the parameter to set and a "." followed by the desired value for the parameter.
All the parameters will be passed to the compression/decompression context of the Ffmpeg library.

## Parameters

The parameters that can be set on the command line can have different types.<br>
The encoding formats implemented are:
-   H264
-   H265
-   MPEG2VIDEO

If no codec is chosen via the connection string, MPEG2VIDEO codec is used as default. <br>
Parameters can be used to manipulate image quality. For example, the right values for "qmin" and "qmax" parameters can lead you to the best quality (at the expense of the bandwidth).
The best values for these two parameters for each codec are defined in [this page](https://slhck.info/video/2017/02/24/vbr-settings.html) and reported in the following table. <br>
| Codec       | qmin        | qmax        |
| ----------- | ----------- | ----------- |
| h264        | 18          | 28          |
| h265        | 24          | 34          |
| mpeg2video  | 3           | 5           |

In order to control image quality, you can also use the "crf" parameter. The lower the crf value, the better the image quality and vice versa. <br>
Changing the value of the "crf" parameter (as well as of "qmin" and "qmax") indirectly affects the occupied bandwidth, making the encoding a VBR one (Variable Bit Rate).
The best values for the parameter "crf" can be still found in the page above.

| Codec       | crf         |
| ----------- | ----------- |
| h264        | 23          |
| h265        | 28          |

The crf parameter is not supported by mpeg2video, so the values are shown only for the other two codecs.



## Example

You can use the YARP device *test_grabber* to generate a test video stream, composed of frames with a rainbow background and a horizontal red line flowing from top to bottom. In order to visualize the video stream at the destination port, you can use the YARP device *yarpview*.<br>
The commands needed are listed below: <br>

### Terminal 1 (sender)
```
yarp server
```

### Terminal 2 (sender)
```
yarpdev --device test_grabber --mode grid
```

### Terminal 3 (receiver)
```
yarpview --name /view
```

### Terminal 4 (can be run on both sides)
```
yarp connect /grabber /view fast_tcp+send.portmonitor+file.image_compression_ffmpeg+recv.portmonitor+file.image_compression_ffmpeg+type.dll
```
<br>
The following image is the output video stream, which is compressed in sender side, transmitted, decompressed in receiver side and then visualized through the YARP viewer.
<br><br>

![testgrabber](Img/testgrabber.png)
<br>

#### Connection
You need to configure YARP properly to connect two different machines. To check the configuration you can use the command:
```
yarp conf
```
which returns the location of the file that contains the IP address and the port of the server to connect to.
<br>

#### Other Usages
After installing the necessary plugins and following the same procedure, you can also play videos saved on the machine by running the following command (instead of the Terminal 2 command)
```
yarpdev --device ffmpeg_grabber --source path/to/file
```
It is also possible to use this portmonitor with the USBCamera device in YARP.
