file(REMOVE_RECURSE
  "CMakeFiles/demo_thrift"
  "demo_thrift.cmake"
  "include/DemoEnum.h"
  "include/DemoStruct.h"
  "include/DemoStructList.h"
  "include/DemoStructExt.h"
  "include/TestSomeMoreTypes.h"
  "include/Demo.h"
  "src/DemoEnum.cpp"
  "src/DemoStruct.cpp"
  "src/DemoStructList.cpp"
  "src/DemoStructExt.cpp"
  "src/TestSomeMoreTypes.cpp"
  "src/Demo.cpp"
)

# Per-language clean rules from dependency scanning.
foreach(lang)
  include(CMakeFiles/demo_thrift.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
