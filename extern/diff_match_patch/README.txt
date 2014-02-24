diff-match-patch-cpp-stl

C++ STL variant of https://code.google.com/p/google-diff-match-patch.

STL Port was done by Sergey Nozhenko (snhere@gmail.com) and posted on
https://code.google.com/p/google-diff-match-patch/issues/detail?id=25

The STL Port is header only and works with std::wstring andi
std::string.

Homepage: https://github.com/leutloff/diff-match-patch-cpp-stl

Copyright: 2008 Google Inc. All Rights Reserved.

Authors: Neil Fraser <fraser@google.com>
         Mike Slemmer <mikeslemmer@gmail.com>
         Sergey Nozhenko <snhere@gmail.com>

License:
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

Version: 035543a22465c5d7f8e2073f60f407cb35287f4e (Aug 20th 2013)

Patches:
* algorithm.patch: Include <algorithm> from standard c++ library for
  "std::max()" that is not found on some configurations.
