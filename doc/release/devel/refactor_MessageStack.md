refactor_MessageStack {#devel}

### os

#### `MessageStack`

* Constructor now accepts the max number of thread as `size_t` instead of `int`.
* Constructor is now explicit.
* Copy and move constructors and operators are now explicitly deleted.
