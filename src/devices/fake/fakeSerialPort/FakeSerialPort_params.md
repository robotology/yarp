  * |      |  comport           | string  |          |   COM3        | Yes     | name of the serial channel                              | optional, default 1.0s |
  * |      |  verbose           | int     | -        |   1           | No      | Specifies if the device is in verbose mode (0/1)                             | optional, default 1.0s |
  * |      |  baudrate          | int     | -        |   9600        | Yes     | Specifies the baudrate at which the communication port operates                              | optional, default 1.0s |
  * |      |  xonlim            | int     | -        |   0           | No      | Specifies the minimum number of bytes in input buffer before XON char is sent                              | Negative value indicates that default value should be used (Win32)|
  * |      |  xofflim           | int     | -        |   0           | No      | Specifies the maximum number of bytes in input buffer before XOFF char is sent                             | Negative value indicates that default value should be used (Win32)|
  * |      |  readmincharacters     | int  | ms        |   1         | No      | Specifies the minimum number of characters for non-canonical read (POSIX)                            | BEWARE: the exit condition for recv() function is readmincharacters && readtimeoutmsec.|
  * |      |  readtimeoutmsec       | int  | ms        |   100         | No    | Specifies the time to wait before returning from read. Negative value means infinite timeout         | BEWARE: the exit condition for recv() function is readmincharacters && readtimeoutmsec.|
  * |      |  paritymode      | string  | -        |   EVEN        | No      | Specifies the parity mode (EVEN, ODD, NONE   | POSIX supports even and odd parity. Additionally Win32 supports mark and space parity modes |
  * |      |  ctsenb          | int  | -        |    0         | No      | Enable & set CTS mode                              | RTS & CTS are enabled/disabled together on some systems  |
  * |      |  rtsenb          | int  | -        |    0         | No      | Enable & set RTS mode                              | RTS & CTS are enabled/disabled together on some systems. 0 = Disable RTS. 1 = Enable RTS. 2 = Enable RTS flow-control handshaking (Win32). 3 = Specifies that RTS line will be high if bytes are available for transmission. After transmission RTS will be low (Win32) |
  * |      |  xinenb            | int  | -        |   0         | No      | Enable/disable software flow control on input                |  |
  * |      |  xoutenb           | int  | -        |   0         | No      | Enable/disable software flow control on output.              |  |
  * |      |  modem             | int  | -        |   0         | No      | Specifies if device is a modem (POSIX). If not set modem status lines are ignored.         |  |
  * |      |  rcvenb            | int  | -        |   0         | No      | Enable/disable receiver (POSIX).                             |  |
  * |      |  dsrenb            | int  | -        |   0         | No      | Controls whether DSR is disabled or enabled (Win32).         |  |
  * |      |  dtrdisable        | int  | -        |   0         | No      | Controls whether DTR is disabled or enabled                  |  |
  * |      |  databits              | int     | s        |   7          | No      | Data bits. Valid values 5, 6, 7 and 8 data bits. Additionally Win32 supports 4 data bits.      |  |
  * |      |  stopbits              | int     | s        |   1          | No      | Stop bits. Valid values are 1 and 2.                    |  |
  * |      |  line_terminator_char1 | string  | -        |   \r         | No      | line terminator character for receiveLine()             |  |
  * |      |  line_terminator_char2 | string  | -        |   \n         | No      | line terminator character for receiveLine()             |  |
