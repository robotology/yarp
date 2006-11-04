
SET(CREATE_DEVICE_LIBRARY FALSE CACHE BOOL "Do you want to compile the device library")
SET(CREATE_GUIS FALSE CACHE BOOL "Do you want to compile GUIs")

# Flag for device testing and documentation - not really for end-user,
# but instead the library developers
SET(CREATE_DEVICE_TESTS FALSE CACHE BOOL "Do you want to create device tests")
MARK_AS_ADVANCED(CREATE_DEVICE_DOC)
