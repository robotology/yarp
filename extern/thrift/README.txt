Apache Thrift
-------------

The Apache Thrift software framework, for scalable cross-language services
development, combines a software stack with a code generation engine to build
services that work efficiently and seamlessly between C++, Java, Python, PHP,
Ruby, Erlang, Perl, Haskell, C#, Cocoa, JavaScript, Node.js, Smalltalk, OCaml
and Delphi and other languages.

Homepage: https://thrift.apache.org/

Copyright: See thrift/LICENSE

License: Apache-2.0

Additional licenses:
 BSD-3-Clause
 GPL-3-or-later WITH Bison-exception-2.2
 GPL-2.0-or-later
 BSD-2-Clause

Version: 0.14.1

Patches:

* Modified version string in compiler/cpp/src/thrift/version.h

```diff
--- a/extern/thrift/thrift/compiler/cpp/src/thrift/version.h
+++ b/extern/thrift/thrift/compiler/cpp/src/thrift/version.h
@@ -24,6 +24,6 @@
 #pragma once
 #endif // _MSC_VER

-#define THRIFT_VERSION "0.14.1"
+#define THRIFT_VERSION "0.14.1-yarped"

 #endif // _THRIFT_VERSION_H_
```
