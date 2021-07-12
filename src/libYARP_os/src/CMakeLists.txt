# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

add_library(YARP_os)
add_library(YARP::YARP_os ALIAS YARP_os)

if(NOT YARP_NO_DEPRECATED) # since YARP 3.3
  add_library(YARP_OS ALIAS YARP_os)
  add_library(YARP::YARP_OS ALIAS YARP_os)
endif()

set(YARP_os_HDRS yarp/os/AbstractCarrier.h
                 yarp/os/AbstractContactable.h
                 yarp/os/all.h
                 yarp/os/api.h
                 yarp/os/BinPortable.h
                 yarp/os/BinPortable-inl.h
                 yarp/os/Bottle.h
                 yarp/os/BufferedPort.h
                 yarp/os/BufferedPort-inl.h
                 yarp/os/Bytes.h
                 yarp/os/Carrier.h
                 yarp/os/Carriers.h
                 yarp/os/Clock.h
                 yarp/os/Connection.h
                 yarp/os/ConnectionReader.h
                 yarp/os/ConnectionState.h
                 yarp/os/ConnectionWriter.h
                 yarp/os/ConstString.h
                 yarp/os/Contactables.h
                 yarp/os/Contactable.h
                 yarp/os/Contact.h
                 yarp/os/ContactStyle.h
                 yarp/os/DummyConnector.h
                 yarp/os/Election.h
                 yarp/os/Event.h
                 yarp/os/Face.h
                 yarp/os/Header.h
                 yarp/os/IConfig.h
                 yarp/os/InputProtocol.h
                 yarp/os/InputStream.h
                 yarp/os/LocalReader.h
                 yarp/os/Log.h
                 yarp/os/LogComponent.h
                 yarp/os/LogStream.h
                 yarp/os/ManagedBytes.h
                 yarp/os/MessageStack.h
                 yarp/os/ModifyingCarrier.h
                 yarp/os/MonitorObject.h
                 yarp/os/MultiNameSpace.h
                 yarp/os/Name.h
                 yarp/os/NameSpace.h
                 yarp/os/NameStore.h
                 yarp/os/NestedContact.h
                 yarp/os/NetFloat32.h
                 yarp/os/NetFloat64.h
                 yarp/os/NetInt8.h
                 yarp/os/NetInt16.h
                 yarp/os/NetInt32.h
                 yarp/os/NetInt64.h
                 yarp/os/NetType.h
                 yarp/os/NetUint8.h
                 yarp/os/NetUint16.h
                 yarp/os/NetUint32.h
                 yarp/os/NetUint64.h
                 yarp/os/Network.h
                 yarp/os/NetworkClock.h
                 yarp/os/Nodes.h
                 yarp/os/Node.h
                 yarp/os/NullConnection.h
                 yarp/os/NullConnectionReader.h
                 yarp/os/NullConnectionWriter.h
                 yarp/os/Os.h
                 yarp/os/OutputProtocol.h
                 yarp/os/OutputStream.h
                 yarp/os/PeriodicThread.h
                 yarp/os/Ping.h
                 yarp/os/Portable.h
                 yarp/os/PortablePair.h
                 yarp/os/Port.h
                 yarp/os/PortInfo.h
                 yarp/os/PortReaderBuffer.h
                 yarp/os/PortReaderBuffer-inl.h
                 yarp/os/PortReaderBufferBase.h
                 yarp/os/PortReaderBufferBaseCreator.h
                 yarp/os/PortReaderCreator.h
                 yarp/os/PortReader.h
                 yarp/os/PortReport.h
                 yarp/os/PortWriterBuffer.h
                 yarp/os/PortWriterBufferBase.h
                 yarp/os/PortWriter.h
                 yarp/os/Property.h
                 yarp/os/Publisher.h
                 yarp/os/Random.h
                 yarp/os/ResourceFinder.h
                 yarp/os/ResourceFinderOptions.h
                 yarp/os/RFModule.h
                 yarp/os/RFPlugin.h
                 yarp/os/RosNameSpace.h
                 yarp/os/Route.h
                 yarp/os/RpcClient.h
                 yarp/os/RpcServer.h
                 yarp/os/Searchable.h
                 yarp/os/Semaphore.h
                 yarp/os/SharedLibraryClassApi.h
                 yarp/os/SharedLibraryClassFactory.h
                 yarp/os/SharedLibraryClass.h
                 yarp/os/SharedLibraryFactory.h
                 yarp/os/SharedLibrary.h
                 yarp/os/ShiftStream.h
                 yarp/os/SizedWriter.h
                 yarp/os/Stamp.h
                 yarp/os/StringInputStream.h
                 yarp/os/StringOutputStream.h
                 yarp/os/Subscriber.h
                 yarp/os/SystemClock.h
                 yarp/os/Terminator.h
                 yarp/os/Things.h
                 yarp/os/Thread.h
                 yarp/os/Time.h
                 yarp/os/Timer.h
                 yarp/os/TwoWayStream.h
                 yarp/os/Type.h
                 yarp/os/TypedReader.h
                 yarp/os/TypedReaderCallback.h
                 yarp/os/TypedReaderCallback-inl.h
                 yarp/os/TypedReaderThread.h
                 yarp/os/TypedReaderThread-inl.h
                 yarp/os/UnbufferedContactable.h
                 yarp/os/Value.h
                 yarp/os/Vocab.h
                 yarp/os/Wire.h
                 yarp/os/WireLink.h
                 yarp/os/YarpNameSpace.h
                 yarp/os/YarpPlugin.h
                 yarp/os/YarpPluginSelector.h
                 yarp/os/YarpPluginSettings.h
                 yarp/os/SystemInfo.h
                 yarp/os/SystemInfoSerializer.h
                 yarp/os/QosStyle.h)

set(YARP_os_SRCS yarp/os/AbstractCarrier.cpp
                 yarp/os/AbstractContactable.cpp
                 yarp/os/Bottle.cpp
                 yarp/os/Bytes.cpp
                 yarp/os/Carrier.cpp
                 yarp/os/Carriers.cpp
                 yarp/os/Clock.cpp
                 yarp/os/Connection.cpp
                 yarp/os/ConnectionReader.cpp
                 yarp/os/ConnectionState.cpp
                 yarp/os/ConnectionWriter.cpp
                 yarp/os/Contactable.cpp
                 yarp/os/Contactables.cpp
                 yarp/os/Contact.cpp
                 yarp/os/ContactStyle.cpp
                 yarp/os/DummyConnector.cpp
                 yarp/os/Event.cpp
                 yarp/os/Header.cpp
                 yarp/os/IConfig.cpp
                 yarp/os/InputStream.cpp
                 yarp/os/Log.cpp
                 yarp/os/LogComponent.cpp
                 yarp/os/ManagedBytes.cpp
                 yarp/os/MessageStack.cpp
                 yarp/os/ModifyingCarrier.cpp
                 yarp/os/MultiNameSpace.cpp
                 yarp/os/Name.cpp
                 yarp/os/NameSpace.cpp
                 yarp/os/NameStore.cpp
                 yarp/os/MonitorObject.cpp
                 yarp/os/NestedContact.cpp
                 yarp/os/NetFloat32.cpp
                 yarp/os/NetFloat64.cpp
                 yarp/os/NetInt16.cpp
                 yarp/os/NetInt32.cpp
                 yarp/os/NetInt64.cpp
                 yarp/os/NetType.cpp
                 yarp/os/NetUint16.cpp
                 yarp/os/NetUint32.cpp
                 yarp/os/NetUint64.cpp
                 yarp/os/Network.cpp
                 yarp/os/NetworkClock.cpp
                 yarp/os/Nodes.cpp
                 yarp/os/Node.cpp
                 yarp/os/NullConnection.cpp
                 yarp/os/NullConnectionReader.cpp
                 yarp/os/NullConnectionWriter.cpp
                 yarp/os/Os.cpp
                 yarp/os/OutputStream.cpp
                 yarp/os/PeriodicThread.cpp
                 yarp/os/Ping.cpp
                 yarp/os/Portable.cpp
                 yarp/os/PortablePair.cpp
                 yarp/os/Port.cpp
                 yarp/os/PortInfo.cpp
                 yarp/os/PortReaderBufferBase.cpp
                 yarp/os/PortReaderBufferBaseCreator.cpp
                 yarp/os/PortReader.cpp
                 yarp/os/PortReaderCreator.cpp
                 yarp/os/PortReport.cpp
                 yarp/os/PortWriterBufferBase.cpp
                 yarp/os/PortWriter.cpp
                 yarp/os/Property.cpp
                 yarp/os/Random.cpp
                 yarp/os/ResourceFinder.cpp
                 yarp/os/ResourceFinderOptions.cpp
                 yarp/os/RFModule.cpp
                 yarp/os/RFPlugin.cpp
                 yarp/os/RosNameSpace.cpp
                 yarp/os/Route.cpp
                 yarp/os/RpcClient.cpp
                 yarp/os/RpcServer.cpp
                 yarp/os/Searchable.cpp
                 yarp/os/Semaphore.cpp
                 yarp/os/SharedLibrary.cpp
                 yarp/os/SharedLibraryFactory.cpp
                 yarp/os/ShiftStream.cpp
                 yarp/os/SizedWriter.cpp
                 yarp/os/Stamp.cpp
                 yarp/os/SystemClock.cpp
                 yarp/os/SystemInfo.cpp
                 yarp/os/SystemInfoSerializer.cpp
                 yarp/os/Terminator.cpp
                 yarp/os/Things.cpp
                 yarp/os/Thread.cpp
                 yarp/os/Time.cpp
                 yarp/os/Timer.cpp
                 yarp/os/TwoWayStream.cpp
                 yarp/os/Type.cpp
                 yarp/os/Value.cpp
                 yarp/os/Vocab.cpp
                 yarp/os/YarpNameSpace.cpp
                 yarp/os/YarpPlugin.cpp
                 yarp/os/WireLink.cpp
                 yarp/os/QosStyle.cpp)

if(NOT YARP_NO_DEPRECATED)
  list(APPEND YARP_os_HDRS yarp/os/ConstString.h      # DEPRECATED since YARP 3.0.0
                           yarp/os/LockGuard.h        # DEPRECATED since YARP 3.3.0
                           yarp/os/Mutex.h            # DEPRECATED since YARP 3.3.0
                           yarp/os/RateThread.h       # DEPRECATED since YARP 3.3.0
                           yarp/os/RecursiveMutex.h   # DEPRECATED since YARP 3.3.0
                           yarp/os/Runnable.h)        # DEPRECATED since YARP 3.3.0
  list(APPEND YARP_os_SRCS yarp/os/Mutex.cpp          # DEPRECATED since YARP 3.3.0
                           yarp/os/RateThread.cpp     # DEPRECATED since YARP 3.3.0
                           yarp/os/RecursiveMutex.cpp # DEPRECATED since YARP 3.3.0
                           yarp/os/Runnable.cpp)      # DEPRECATED since YARP 3.3.0
endif()

set(YARP_os_IDL_HDRS yarp/os/idl/BareStyle.h
                     yarp/os/idl/BottleStyle.h
                     yarp/os/idl/Unwrapped.h
                     yarp/os/idl/WirePortable.h
                     yarp/os/idl/WireReader.h
                     yarp/os/idl/WireState.h
                     yarp/os/idl/WireTypes.h
                     yarp/os/idl/WireVocab.h
                     yarp/os/idl/WireWriter.h)

set(YARP_os_IDL_SRCS yarp/os/idl/WirePortable.cpp
                     yarp/os/idl/WireReader.cpp
                     yarp/os/idl/WireState.cpp
                     yarp/os/idl/WireWriter.cpp)

set(YARP_os_IMPL_HDRS yarp/os/impl/AuthHMAC.h
                      yarp/os/impl/BottleImpl.h
                      yarp/os/impl/BufferedConnectionWriter.h
                      yarp/os/impl/ConnectionRecorder.h
                      yarp/os/impl/DgramTwoWayStream.h
                      yarp/os/impl/Dispatcher.h
                      yarp/os/impl/FakeFace.h
                      yarp/os/impl/FakeTwoWayStream.h
                      yarp/os/impl/FallbackNameClient.h
                      yarp/os/impl/FallbackNameServer.h
                      yarp/os/impl/HttpCarrier.h
                      yarp/os/impl/LocalCarrier.h
                      yarp/os/impl/LogComponent.h
                      yarp/os/impl/LogForwarder.h
                      yarp/os/impl/McastCarrier.h
                      yarp/os/impl/MemoryOutputStream.h
                      yarp/os/impl/NameClient.h
                      yarp/os/impl/NameConfig.h
                      yarp/os/impl/NameserCarrier.h
                      yarp/os/impl/NameServer.h
                      yarp/os/impl/PlatformDirent.h
                      yarp/os/impl/PlatformDlfcn.h
                      yarp/os/impl/PlatformIfaddrs.h
                      yarp/os/impl/PlatformLimits.h
                      yarp/os/impl/PlatformNetdb.h
                      yarp/os/impl/PlatformSignal.h
                      yarp/os/impl/PlatformStdio.h
                      yarp/os/impl/PlatformSysStat.h
                      yarp/os/impl/PlatformSysWait.h
                      yarp/os/impl/PlatformTime.h
                      yarp/os/impl/PlatformUnistd.h
                      yarp/os/impl/PortCommand.h
                      yarp/os/impl/PortCore.h
                      yarp/os/impl/PortCoreAdapter.h
                      yarp/os/impl/PortCoreInputUnit.h
                      yarp/os/impl/PortCoreOutputUnit.h
                      yarp/os/impl/PortCorePacket.h
                      yarp/os/impl/PortCorePackets.h
                      yarp/os/impl/PortCoreUnit.h
                      yarp/os/impl/Protocol.h
                      yarp/os/impl/RFModuleFactory.h
                      yarp/os/impl/SocketTwoWayStream.h
                      yarp/os/impl/Storable.h
                      yarp/os/impl/StreamConnectionReader.h
                      yarp/os/impl/TcpAcceptor.h
                      yarp/os/impl/TcpCarrier.h
                      yarp/os/impl/TcpConnector.h
                      yarp/os/impl/TcpFace.h
                      yarp/os/impl/TcpStream.h
                      yarp/os/impl/Terminal.h
                      yarp/os/impl/TextCarrier.h
                      yarp/os/impl/ThreadImpl.h
                      yarp/os/impl/UdpCarrier.h)

set(YARP_os_IMPL_SRCS yarp/os/impl/AuthHMAC.cpp
                      yarp/os/impl/BottleImpl.cpp
                      yarp/os/impl/BufferedConnectionWriter.cpp
                      yarp/os/impl/ConnectionRecorder.cpp
                      yarp/os/impl/DgramTwoWayStream.cpp
                      yarp/os/impl/Dispatcher.cpp
                      yarp/os/impl/FakeFace.cpp
                      yarp/os/impl/FallbackNameClient.cpp
                      yarp/os/impl/FallbackNameServer.cpp
                      yarp/os/impl/HttpCarrier.cpp
                      yarp/os/impl/LocalCarrier.cpp
                      yarp/os/impl/LogComponent.cpp
                      yarp/os/impl/LogForwarder.cpp
                      yarp/os/impl/McastCarrier.cpp
                      yarp/os/impl/NameClient.cpp
                      yarp/os/impl/NameConfig.cpp
                      yarp/os/impl/NameserCarrier.cpp
                      yarp/os/impl/NameServer.cpp
                      yarp/os/impl/PlatformTime.cpp
                      yarp/os/impl/PortCommand.cpp
                      yarp/os/impl/PortCore.cpp
                      yarp/os/impl/PortCoreAdapter.cpp
                      yarp/os/impl/PortCoreInputUnit.cpp
                      yarp/os/impl/PortCoreOutputUnit.cpp
                      yarp/os/impl/PortCorePackets.cpp
                      yarp/os/impl/Protocol.cpp
                      yarp/os/impl/RFModuleFactory.cpp
                      yarp/os/impl/SocketTwoWayStream.cpp
                      yarp/os/impl/Storable.cpp
                      yarp/os/impl/StreamConnectionReader.cpp
                      yarp/os/impl/TcpCarrier.cpp
                      yarp/os/impl/TcpFace.cpp
                      yarp/os/impl/Terminal.cpp
                      yarp/os/impl/TextCarrier.cpp
                      yarp/os/impl/ThreadImpl.cpp
                      yarp/os/impl/UdpCarrier.cpp)

set(YARP_os_IMPL_POSIX_HDRS yarp/os/impl/posix/TcpAcceptor.h
                            yarp/os/impl/posix/TcpConnector.h
                            yarp/os/impl/posix/TcpStream.h)
set(YARP_os_IMPL_POSIX_SRCS yarp/os/impl/posix/TcpAcceptor.cpp
                            yarp/os/impl/posix/TcpConnector.cpp
                            yarp/os/impl/posix/TcpStream.cpp)

set(YARP_os_IMPL_MACOS_HDRS yarp/os/impl/macos/MacOSAPI.h)
set(YARP_os_IMPL_MACOS_SRCS yarp/os/impl/macos/MacOSAPI.mm)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Source Files"
             FILES ${YARP_os_SRCS}
                   ${YARP_os_IDL_SRCS}
                   ${YARP_os_IMPL_SRCS}
                   ${YARP_os_IMPL_POSIX_SRCS}
                   ${YARP_os_IMPL_MACOS_SRCS}
                   ${YARP_os_IMPL_WIN32_SRCS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Header Files"
             FILES ${YARP_os_HDRS}
                   ${YARP_os_IDL_HDRS}
                   ${YARP_os_IMPL_HDRS}
                   ${YARP_os_IMPL_POSIX_HDRS}
                   ${YARP_os_IMPL_MACOS_HDRS})

target_sources(YARP_os PRIVATE ${YARP_os_SRCS}
                               ${YARP_os_HDRS}
                               ${YARP_os_IDL_SRCS}
                               ${YARP_os_IDL_HDRS}
                               ${YARP_os_IMPL_SRCS}
                               ${YARP_os_IMPL_HDRS})

if(SKIP_ACE)
  target_sources(YARP_os PRIVATE ${YARP_os_IMPL_POSIX_HDRS}
                                 ${YARP_os_IMPL_POSIX_SRCS})
endif()

if(APPLE)
  target_sources(YARP_os PRIVATE ${YARP_os_IMPL_MACOS_HDRS}
                                 ${YARP_os_IMPL_MACOS_SRCS})
  # Required by MacOSAPI.mm
  target_link_libraries(YARP_os PRIVATE "-framework Foundation")
endif()

if(MSVC AND YARP_HAS_WIN_VT_SUPPORT)
  target_compile_definitions(YARP_os PRIVATE YARP_HAS_WIN_VT_SUPPORT
                                             NOMINMAX)
endif()

target_include_directories(YARP_os PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                                          $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_features(YARP_os PUBLIC cxx_std_14)

target_link_libraries(YARP_os PUBLIC YARP::YARP_conf)
list(APPEND YARP_os_PUBLIC_DEPS YARP_conf)

target_include_directories(YARP_os SYSTEM PRIVATE ${hmac_INCLUDE_DIRS})
target_link_libraries(YARP_os PRIVATE ${hmac_LIBRARIES})
# list(APPEND YARP_os_PUBLIC_DEPS hmac)

if(YARP_HAS_ACE)
  target_compile_definitions(YARP_os PRIVATE YARP_HAS_ACE)
  target_link_libraries(YARP_os PRIVATE ACE::ACE)
  list(APPEND YARP_os_PRIVATE_DEPS ACE)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  # Required for using std::thread on linux
  target_link_libraries(YARP_os PRIVATE pthread)

  # Required by SharedLibrary.cpp (dlopen, dlsym, dlclose, dlerror)
  target_link_libraries(YARP_os PRIVATE ${CMAKE_DL_LIBS})
endif()

if(MSVC)
  # Required by Time.cpp (timeBeginPeriod, timeEndPeriod functions)
  target_link_libraries(YARP_os PRIVATE "Winmm")
endif()

if(YARP_HAS_Libedit)
  target_compile_definitions(YARP_os PRIVATE YARP_HAS_Libedit)
  target_link_libraries(YARP_os PRIVATE Libedit::libedit)
  list(APPEND YARP_os_PRIVATE_DEPS Libedit)
endif()

set_property(TARGET YARP_os PROPERTY PUBLIC_HEADER ${YARP_os_HDRS})
set_property(TARGET YARP_os PROPERTY PRIVATE_HEADER ${YARP_os_IMPL_HDRS})
set_property(TARGET YARP_os PROPERTY VERSION ${YARP_VERSION_SHORT})
set_property(TARGET YARP_os PROPERTY SOVERSION ${YARP_SOVERSION})
set_property(TARGET YARP_os PROPERTY FOLDER "Libraries")


install(TARGETS YARP_os
        EXPORT YARP_os
        RUNTIME
          DESTINATION "${CMAKE_INSTALL_BINDIR}"
          COMPONENT YARP_os
        LIBRARY
          DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          COMPONENT YARP_os
          NAMELINK_COMPONENT YARP_os-dev
        ARCHIVE
          DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          COMPONENT YARP_os-dev
        PUBLIC_HEADER
          DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/os"
          COMPONENT YARP_os-dev
        PRIVATE_HEADER
          DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/os/impl"
          COMPONENT YARP_os-priv-dev)
# PUBLIC_HEADER and PRIVATE_HEADER do not support sub-directories
# See https://gitlab.kitware.com/cmake/cmake/issues/7941
install(FILES ${YARP_os_IDL_HDRS}
        COMPONENT YARP_os-dev
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp/os/idl)
install(FILES ${YARP_os_IMPL_POSIX_HDRS}
        COMPONENT YARP_os-priv-dev
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp/os/impl/posix)
install(FILES ${YARP_os_IMPL_MACOS_HDRS}
        COMPONENT YARP_os-priv-dev
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp/os/impl/macos)


set(YARP_os_PUBLIC_DEPS ${YARP_os_PUBLIC_DEPS} PARENT_SCOPE)
set(YARP_os_PRIVATE_DEPS ${YARP_os_PRIVATE_DEPS} PARENT_SCOPE)
