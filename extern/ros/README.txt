ROS message definitions
-----------------------

Homepage: https://github.com/ros/std_msgs
          https://github.com/ros/common_msgs
          https://github.com/ros/geometry
          https://github.com/ros/geometry2

Copyright: See single packages

License: BSD-3-Clause

Version: std_msgs      0.5.11
         common_msgs   1.12.5
         geometry      1.11.9
         geometry2     0.5.17

Patches:
  find extern/ros/std_msgs -type f -not -name "*.msg" -not -name "*.srv" -not -name "package.xml" -delete
  find extern/ros/common_msgs -type f -not -name "*.msg" -not -name "*.srv" -not -name "package.xml" -delete
  find extern/ros/geometry -type f -not -name "*.msg" -not -name "*.srv" -not -name "package.xml" -delete
  find extern/ros/geometry2 -type f -not -name "*.msg" -not -name "*.srv" -not -name "package.xml" -delete
  find extern/ros/ -type d -exec bash -c "echo -ne '{} '; ls '{}' | wc -l" \; | awk '$NF==1{print $1}' | xargs -i find {} -type f -name package.xml -delete
  find extern/ros/ -type d -empty -delete
