XmlRpc++
--------

XmlRpc++ is an implementation of the XmlRpc protocol written in C++,
based upon Shilad Sen's excellent py-xmlrpc library. It is designed to
make it easy to incorporate XmlRpc client+server support into C++
applications and requires no other libraries.

This version contains several modification (including a change of
namespace from XmlRpc to YarpXmlRpc and the removal of a few classes and
the replacement of the original base64.h which license is unknown with a
different implementation by René Nyffenegger released as BSD) and a few
bug fixes for YARP.

Homepage: http://xmlrpcpp.sourceforge.net/
          https://sourceforge.net/projects/xmlrpcpp/

Copyright: Copyright (c) 2002-2003 Chris Morley

License: LGPL2.1 or later

Version: 0.7



base64.cpp and base64.h

Encoding and decoding base64 with C++

Homepage: http://www.adp-gmbh.ch/cpp/common/base64.html

Copyright (C) 2004-2008 René Nyffenegger

License: zlib
 This source code is provided 'as-is', without any express or implied
 warranty. In no event will the author be held liable for any damages
 arising from the use of this software.

 Permission is granted to anyone to use this software for any purpose,
 including commercial applications, and to alter it and redistribute it
 freely, subject to the following restrictions:

 1. The origin of this source code must not be misrepresented; you must not
    claim that you wrote the original source code. If you use this source code
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

 2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original source code.

 3. This notice may not be removed or altered from any source distribution.



Patches:
 * 0001-Fix-EOL-and-permissions.patch: Fix EOL and permissions
   (from commit 19a3ac4fbd725719a6dae0ee87c9536d388d427a)

 * 0002-first-pass-at-an-xml-rpc-carrier.patch: first pass at an xml/rpc
   carrier
   (from commit 19a3ac4fbd725719a6dae0ee87c9536d388d427a)

 * 0003-more-LGPL-tagging.patch: more LGPL tagging
   (from commit b3e24151e815a3cb634d0a1c6a8815449ff9db7a)

 * 0004-small-windows-fixes.patch: small windows fixes
   (from commit 9ea098942e14e08150c5736c71ebe6c58e47fdda)

 * 0005-fix-a-few-trivial-warnings.patch: fix a few trivial warnings
   (from commit 2a78f5e6766785c435bf03053761760e4d944f3f)

 * 0006-merge-improved-tcpros-.msg-support-from-yarpros-bran.patch:
   merge improved tcpros + .msg support from yarpros branch on launchpad
   (from commit 48e3b01d0a66e135f239f824033ff6c857827364)

 * 0007-tidy-up-windows-build-of-tcpros-carrier.patch: tidy up windows
   build of tcpros carrier
   (from commit 447f0b0d141f162b8481d05bfbbb317878761388)

 * 0008-fix-integer-type.patch: fix integer type
   (from commit ae3580f308d7b30b6e04121fdcf0fd64ab52126c)

 * 0009-add-library-needed-with-msvc-dll-build.patch: add library needed
   with msvc dll build
   (from commit dfa12005a8b39bc8abe76cdbdea1f19d086cc836)

 * 0010-drop-unneeded-xmlrpc-code-that-is-choking-msvc-dll-b.patch: drop
   unneeded xmlrpc code that is choking msvc dll build
   (from commit 57c407b67df1110c74881f32e4bf8b05e1b1cd91)

 * 0011-Cleanup.patch: [Cleanup]
   (from commit c44cf5c842b7bd679155d751ae7b4d46907a58a3)

 * 0012-Documentation-tweaks.patch: Documentation tweaks
   (from commit 85ca0d444c8462cd0d9da9130f82d6cfbf02f7fa)

 * 0013-ros-change-XmlRpc-namespace-to-avoid-conflict.patch: YARP is
   compiled as a shared library, where the XmlRpc-related symbols are
   not exposed).  To support static linking, the XmlRpc namespace in
   YARP is renamed to YarpXmlRpc.
   See https://github.com/robotology/yarp/issues/167
   (from commit 8ecaff0464bec2b6deb1538b3cb6b6d322cc0ac4)

 * 0014-Cleanup.patch: [Cleanup]
   (from commit 53ea746a7fa2d2603df46763b8cb380dcc2407ab)

 * 0015-Cleanup-and-Fix-issues-with-latex-pdf-generation.patch: Cleanup
   and Fix issues with latex/pdf generation
   (from commit 9ddf972da49e7e258e8f0df7ef8c1a643d02f5b8)

 * 0016-Renamed-macros-to-avoid-leading-underscore.patch: Renamed macros
   to avoid leading underscore
   (from commit df486e58f8ec80e140e0f55e1c3833ac8ca50603)

 * 0017-xmlrpc-Fix-tmEq-method.patch: xmlrpc++: Fix tmEq() method
   (from commit 95238ffbcd164594ace75baff278ac936a14484b)

 * rm -r extern/xmlrpcpp/xmlrpcpp/test
