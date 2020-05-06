log_remove_trace {#master}
----------------

### Libraries

#### `os`

##### `Log`

* Trace should no longer generate code when building in release mode.
  When building with `-DYARP_NO_DEBUG_OUTPUT` debug should not generate any code
  as well. See https://godbolt.org/z/hSAC56
