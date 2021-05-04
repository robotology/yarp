fakeFrameGrabber_enh  {#master}
--------------------

## Devices

### `fakeFrameGrabber`

* Added `--timestamp` option to write the timestamp in the first bytes of the
  image. This was previously automatically enabled in `line` mode, but it can
  now be used in all modes.
* Add `nois` (noise) mode. This can be used alone to generate a white noise,
  image, or while passing `--src` to add some noise to the image.
  The `--snr` can be pass to specify the signal noise ratio (default 0.5).
* Double buffering is now used to produce the image on a separate thread. This
  should improve performances when generating large images.
