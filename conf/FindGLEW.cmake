include(MacroStandardFindModule)
macro_standard_find_module(GLEW glew)

if(NOT GLEW_FOUND)
  set(GLEW_INCLUDE_DIRS "D:\\ROBOT\\glew-1.12.0\\include")
  set(GLEW_LIBRARIES "D:\\ROBOT\\glew-1.12.0\\lib\\Release\\Win32\\glew32.lib")
endif()
