  * |      |  source             | string  | -    | default.avi         | No    | media file to read from                       |  |
  * |      |  loop               | bool    | -    | true                | No    | media should loop (default)                       |  |
  * |      |  sync               | string  | -    | image               | No    | sync on image or audio (if have to choose)?        |  |
  * |      |  nodelay            | bool    | -    | false               | No    | media will play in simulated realtime unless this is present        |  |
  * |      |  pace               | bool    | -    | 1.0                 | No    | simulated realtime multiplier factor (must be <1 right now)        |  |
  * |      |  v4l                | bool    | -    | false               | No    | if present, read from video4linux        |  |
  * |      |  v4l1               | bool    | -    | false               | No    | if present, read from video4linux        |  |
  * |      |  v4l2               | bool    | -    | false               | No    | if present, read from video4linux        |  |
  * |      |  ieee1394           | bool    | -    | false               | No    | if present, read from firewire           |  |
  * |      |  v4ldevice          | string  | -    |  /dev/video0        | No    | device name                      |  |
  * |      |  audio              | string  | -    |  /dev/dsp           | No    | optional audio device name                            |  |
  * |      |  audio_rate         | int     | -    |  44100              | No    | audio sample rate                              |  |
  * |      |  channels           | int     | -    |  1                  | No    | number of channels                       |  |
  * |      |  time_base_num      | int     | -    |  1                  | No    | numerator of basic time unit                       |  |
  * |      |  time_base_den      | int     | -    |  29                 | No    | denominator of basic time unit    |  |
  * |      |  channel            | int     | -    |  0                  | No    | channel identifier                       |  |
  * |      |  standard           | string  | -    |  -                  | No    | pal versus ntsc    |  |
  * |      |  width              | int     | -    |  640                | No    | width of image                       |  |
  * |      |  height             | int     | -    |  480                | No    | height of image    |  |
  * |      |  devname            | string  | -    |  /dev/dv1394        | No    | firewire device name                      |  |
