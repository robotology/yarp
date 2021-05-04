remove_stream_operator_std_vector {#master}
---------------------------------

## Libraries

### `os`

#### `LogStream`

* The `operator<<` for `std::ostream` and `std::vector` which caused conflicts
  with Casadi was removed (#2067).
