
sound_marker_portmonitor plugin
======================================================================
This portmonitor adds a marker (i.e. extra audio samples with a specific pattern) at the end of each processed yarp::sig::Sound

Usage:
-----
yarpdev --device deviceBundler --wrapper_device AudioRecorderWrapper --attached_device portaudioRecorder --start --min_samples_over_network 3200 --max_samples_over_network 3200 --AUDIO_BASE::rate 16000 --AUDIO_BASE::samples 6400 --AUDIO_BASE::channels 1
yarpdev --device deviceBundler --wrapper_device AudioPlayerWrapper --attached_device audioToFileDevice --start
yarp connect /audioRecorder_nws/audio:o /audioPlayerWrapper/audio:i tcp+recv.portmonitor+file.sound_marker+type.dll
