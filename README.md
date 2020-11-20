![YARP logo](doc/images/yarp-robot-24.png?raw=true "YARP")
YARP
====

[![YARP homepage](https://img.shields.io/badge/YARP-Yet_Another_Robot_Platform-19c2d8.svg?logo=data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABYAAAAWCAYAAADEtGw7AAAABmJLR0QA/wD/AP+gvaeTAAAACXBIWXMAAA7EAAAOxAGVKw4bAAAAB3RJTUUH4QEDEQMztwAfSwAAAhRJREFUOMvVlD9ME2EYxn931x4crVDbajTRwbQSg8ZuDm6NuombBgcTZxMXY4gLySV2YDFMupKwAtESRhIxEQdcIFEjqK1/SsBaWkr/3ZXenUPTUpprA7Ed/Lbvfb7n+d7vyfN+AgCvV89gSMt0YknGFcKhhEiXlsOuOHbuhDvkVurYk29bua/FsgEQ7JOl8cCpYzVsNV+qPI3/yTdr2HYc9rrkSHROGZ2eUbTkhuJzSvVzPqckaskNZXR6RolE55Sw1yUfumOA+M4uWV0nq+nAQW5W04llsgz09LS0QiQyMcnS4nzHzF1anCcyMenAwotg+Zvxl7dvsmeaDPl9TK0nD2C3BgMEvR6cop2Tlh8Lr60Vwys/M7IoVDeJBFnDsGrY+1xp7/JKYqu2L3/PHz4VBcO0Cob9S00TMub+Ra09toghimsd81gU17CICa0m78WF0/1XB/rkdhrvssXyg8+bu3aT1zJuwV7ZEXL3OtsJJ/WKeaTJA4hu57QvWrnSTvhTQa8cWfj5r3Txn6zu7idkaCVwvUEQ+huCrmGRassWOA6CqyGM6aoWCPXawsdHYD1uYL3l+sU7bYUXPjwD7u73wkNuXJrtqhX/n3DVY1UVOTn4ClG6Vkcqxiap9SFUtWzLVFUZX2AZp3y+Xitrs6Tj91FVs5oKh/ss27+Hm6gBRM8IMGX/Vs8IO6lQU/UeDvcY8OMv7HG7CnjlFeQAAAAASUVORK5CYII=)](http://www.yarp.it/)

     __  __ ___  ____   ____
     \ \/ //   ||  _ \ |  _ \
      \  // /| || |/ / | |/ /
      / // ___ ||  _ \ |  _/
     /_//_/  |_||_| \_\|_|
     ========================
    Yet Another Robot Platform

[![Latest release](https://img.shields.io/github/release/robotology/yarp.svg)](https://github.com/robotology/yarp/releases)
[![Release date](https://img.shields.io/github/release-date/robotology/yarp.svg)](https://github.com/robotology/yarp/releases)

YARP is a library and toolkit for communication and device interfaces,
used on everything from humanoids to embedded devices.


Documentation
-------------

[![YARP documentation](https://img.shields.io/badge/Documentation-yarp.it-19c2d8.svg)](http://www.yarp.it/)


Installation
------------

See full instructions at http://www.yarp.it/install.html

* On Linux:

```bash
sudo apt-get install cmake libace-dev
git clone https://github.com/robotology/yarp
cd yarp && mkdir build && cd build && cmake .. && make
sudo make install  # Optional
```

* On Windows:

  Binaries at https://github.com/robotology/yarp/releases

* On macOS:

```bash
brew install robotology/formulae/yarp
```

Regular YARP builds use the [ACE](https://github.com/DOCGroup/ACE_TAO) library.
On Linux and macOS, YARP can be compiled without ACE by adding `-DSKIP_ACE=TRUE`
when running cmake.


Tutorials
---------

There's a comprehensive list of tutorials here:

 * http://www.yarp.it/tutorials.html


License
-------

[![License](https://img.shields.io/badge/license-BSD--3--Clause%20%2B%20others-19c2d8.svg)](https://github.com/robotology/yarp/blob/master/LICENSE)

Material included in YARP is Copyright of Istituto Italiano di Tecnologia (IIT),
RobotCub Consortium and other contributors.

Most YARP components are released under the terms of the BSD-3-Clause.
Some optional components are released under the terms of the LGPL-2.1 or later,
GPL-2.0 or later, GPL-3.0 or later, or Apache-2.0 License.
See the file COPYING and LICENSE files for details.


CI Status
---------

[![Build Status](https://github.com/robotology/yarp/workflows/CI%20Workflow/badge.svg)](https://github.com/robotology/yarp/actions?query=workflow%3A%22CI+Workflow%22)

Stats
-----

[![Github commits (since latest release)](https://img.shields.io/github/commits-since/robotology/yarp/latest.svg)](https://github.com/robotology/yarp)
[![GitHub last commit](https://img.shields.io/github/last-commit/robotology/yarp/master.svg?label=last%20commit)](https://github.com/robotology/yarp)

[![GitHub commit activity the past week, 4 weeks, year](https://img.shields.io/github/commit-activity/y/robotology/yarp.svg)](https://github.com/robotology/yarp)
[![GitHub contributors](https://img.shields.io/github/contributors/robotology/yarp.svg)](https://github.com/robotology/yarp/graphs/contributors)

[![GitHub closed issues](https://img.shields.io/github/issues-closed-raw/robotology/yarp.svg)](https://github.com/robotology/yarp/issues)
[![GitHub issues](https://img.shields.io/github/issues-raw/robotology/yarp.svg)](https://github.com/robotology/yarp/issues)

[![GitHub pull requests](https://img.shields.io/github/issues-pr-raw/robotology/yarp.svg)](https://github.com/robotology/yarp/pulls)
[![GitHub closed pull requests](https://img.shields.io/github/issues-pr-closed-raw/robotology/yarp.svg)](https://github.com/robotology/yarp/pulls)

[![GitHub top language](https://img.shields.io/github/languages/count/robotology/yarp.svg)](https://github.com/robotology/yarp)
[![GitHub language count](https://img.shields.io/github/languages/top/robotology/yarp.svg)](https://github.com/robotology/yarp)


Development Powered by
----------------------

[![GitHub](https://img.shields.io/badge/Hosted_on-GitHub-24292e.svg?logo=github)](https://github.com)

[![Slack](https://img.shields.io/badge/-Slack-4d394b.svg?logo=slack)](https://icub-facility.slack.com)

[![ZenHub](https://img.shields.io/badge/Shipping_faster_with-ZenHub-435198.svg)](https://zenhub.com)

[![Shields.io](https://img.shields.io/badge/Badges_by-Shields.io-green.svg)](https://shields.io/)

[![YARPino](https://img.shields.io/badge/YARP-ino-19c2d8.svg?logo=data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABYAAAAWCAYAAADEtGw7AAAABmJLR0QA/wD/AP+gvaeTAAAACXBIWXMAAA7EAAAOxAGVKw4bAAAAB3RJTUUH4QEDEQMztwAfSwAAAhRJREFUOMvVlD9ME2EYxn931x4crVDbajTRwbQSg8ZuDm6NuombBgcTZxMXY4gLySV2YDFMupKwAtESRhIxEQdcIFEjqK1/SsBaWkr/3ZXenUPTUpprA7Ed/Lbvfb7n+d7vyfN+AgCvV89gSMt0YknGFcKhhEiXlsOuOHbuhDvkVurYk29bua/FsgEQ7JOl8cCpYzVsNV+qPI3/yTdr2HYc9rrkSHROGZ2eUbTkhuJzSvVzPqckaskNZXR6RolE55Sw1yUfumOA+M4uWV0nq+nAQW5W04llsgz09LS0QiQyMcnS4nzHzF1anCcyMenAwotg+Zvxl7dvsmeaDPl9TK0nD2C3BgMEvR6cop2Tlh8Lr60Vwys/M7IoVDeJBFnDsGrY+1xp7/JKYqu2L3/PHz4VBcO0Cob9S00TMub+Ra09toghimsd81gU17CICa0m78WF0/1XB/rkdhrvssXyg8+bu3aT1zJuwV7ZEXL3OtsJJ/WKeaTJA4hu57QvWrnSTvhTQa8cWfj5r3Txn6zu7idkaCVwvUEQ+huCrmGRassWOA6CqyGM6aoWCPXawsdHYD1uYL3l+sU7bYUXPjwD7u73wkNuXJrtqhX/n3DVY1UVOTn4ClG6Vkcqxiap9SFUtWzLVFUZX2AZp3y+Xitrs6Tj91FVs5oKh/ss27+Hm6gBRM8IMGX/Vs8IO6lQU/UeDvcY8OMv7HG7CnjlFeQAAAAASUVORK5CYII=)](doc/images/yarp-robot-256.png?raw=true)
