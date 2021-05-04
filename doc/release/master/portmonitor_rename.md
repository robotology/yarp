portmonitor_rename {#master}
------------------

### PortMonitors

* Portmonitors were reorganized and renamed without keeping the back
  compatibility with the old name.
  The new names are:
  * `depthimage_compression_zfp` (`zfp`)
  * `depthimage_to_mono` (`depthimage`)
  * `depthimage_to_rgb` (`depthimage2`)
  * `segmentationimage_to_rgb` (`segmentationimage`)
  The new names for portmonitors not released yet are:
  * `image_compression_ffmpeg` (`libffmpeg`)
  * `sound_compression_mp3` (`mp3Sound`)
* There is now a new `yarppm` library (similar to `yarpcar` and `yarpmod`) that
  links all the portmonitors in static builds.
