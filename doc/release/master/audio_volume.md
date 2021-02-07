audio_volume {#master}
-------------------------

## Important Changes

### Libraries

#### yarp::sig
* `yarp::sig::sound` added the following methods:  amplifyChannel(), amplify(), findPeakInChannel(), findPeak(), normalizeChannel(), normalize() 
* `harness_sig`: added tests to check the new methods

#### yarp::dev
* Added the new class `AudioPlayerDeviceBase`, all playback device drivers now derive from this class.
* Added the new methods to `setHWGain()`, `setHWGain()` to `IAudioRender` interface.
* Added the new methods to `setHWGain()`, `setHWGain()` to `IAudioGrabberSound` interface.

### Devices
* Added RPC commands to `AudioPlayerWrapper` and `AudioRecorderWrapper`, to control the volume via setHWGain()`, `setSWGain()`
* All playback device drivers, i.e. `audioToFileDevice`, `fakeSpeaker`, `portaudioPlayer` ,now derive from this class.
