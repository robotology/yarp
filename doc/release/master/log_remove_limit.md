log_remove_limit {#master}
----------------

### Libraries

#### `os`

##### `Log`

* The limit of 1024 characters for the c-style yDebug() macro family was
  removed. A dynamic allocation is now used, but only when the size of the
  output exceeds this size.
