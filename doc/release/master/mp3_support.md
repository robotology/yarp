mp3_support {#master}
-------------------------

## Important Changes

### libYARP_sig

* `yarp\src\libYARP_sig\src\yarp\sig\SoundFile.h/cpp` the code has been refactored, and the private methods have been move to new files: `SoundFileWav.h/cpp` and SoundFileMp3.h/cpp`
* The following methods were added (SoundFile.h): `read_bytestream()`
* The following methods were added (SoundFileMp3.h): `read_mp3_bytestream()`, `read_mp3_file()`, `write_mp3_file()`
* The following methods were added (SoundFileWav.h): `read_wav_bytestream()`, `read_wav_file()`, `write_wav_files()

### harness_sig

* `yarp\tests\libYARP_sig\SoundTest.cpp` added tests for the new methods.

