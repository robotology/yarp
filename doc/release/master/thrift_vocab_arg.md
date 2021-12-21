thrift_vocab_arg {#master}
----------------

Important Changes
-----------------

### `yarpidl_thrift`

* It's no longer possible to have service functions that starts with the same
  name as a different one followed by `_` (for example `get` and `get_all`).
