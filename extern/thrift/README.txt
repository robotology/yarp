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
 Licensed to the Apache Software Foundation (ASF) under one
 or more contributor license agreements. See the NOTICE file
 distributed with this work for additional information
 regarding copyright ownership. The ASF licenses this file
 to you under the Apache License, Version 2.0 (the
 "License"); you may not use this file except in compliance
 with the License. You may obtain a copy of the License at
 
   http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing,
 software distributed under the License is distributed on an
 "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 KIND, either express or implied. See the License for the
 specific language governing permissions and limitations
 under the License.

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
