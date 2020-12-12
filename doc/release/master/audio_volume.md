audio_volume {#master}
-------------------------

## Important Changes

### Libraries

#### sig
* `yarp::sig::sound` added the following methods:  amplifyChannel(), amplify(), findPeakInChannel(), findPeak(), normalizeChannel(), normalize() 
* `harness_sig`: added tests to check the new methods

### Devices
* `AudioPlayerWrapper` and `AudioRecorderWrapper` added option `--wrapper_volume` to adjust the gain of the played/recorded sound. 
* The same option `wrapper_volume` is also available via rpc command, to interactively change the volume.
