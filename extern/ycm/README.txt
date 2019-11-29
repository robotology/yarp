YCM
---

Extra CMake Modules for YARP and friends

Homepage: https://github.com/robotology/ycm/

Copyright: 2013-2019 Istituto Italiano di Tecnologia (IIT)

License: 3-Clause-BSD
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

 * Neither the names of Istituto Italiano di Tecnologia (IIT),
   nor the names of their contributors may be used to endorse or promote
   products derived from this software without specific prior written
   permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Version: 0.11.0

Notes:
 YCM downloads several modules at build time. We don't want to add a network
 dependency on YARP builds, therefore this is a copy of the share/YCM folder
 in the build directory of a build with YCM_MAINTAINER_MODE enabled.
 If you want to update the files in this folder, update the files in YCM, build
 it, and copy the share/YCM folder here, rename the folder with the right name,
 update the version and add a line to remove the old version in the
 CMakeLists.txt in this folder.
