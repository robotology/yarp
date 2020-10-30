fix_ffmpeg_getAudio {#yarp_3_4}
---------------

### device

#### `ffmpeg`

* Corrected a minor error in `FfmpegGrabber.cpp` that caused a segmentation fault when using a video with audio track as a `source` for the device

* **N.B.** The device can now be used and do not crash but the actual audio grabbing has not been tested yet.