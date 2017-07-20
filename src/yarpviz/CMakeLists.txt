if(CREATE_YARPVIZ)

  project(yarpviz)

  include(YarpUseQt5)
  include(YarpOSXUtilities)

  add_subdirectory(src)
  add_subdirectory(app)

endif()
