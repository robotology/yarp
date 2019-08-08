# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

project(YARP_OS)

set(YARP_OS_HDRS include/yarp/os/AbstractCarrier.h
                 include/yarp/os/AbstractContactable.h
                 include/yarp/os/all.h
                 include/yarp/os/api.h
                 include/yarp/os/BinPortable.h
                 include/yarp/os/BinPortable-inl.h
                 include/yarp/os/Bottle.h
                 include/yarp/os/BufferedPort.h
                 include/yarp/os/BufferedPort-inl.h
                 include/yarp/os/Bytes.h
                 include/yarp/os/Carrier.h
                 include/yarp/os/Carriers.h
                 include/yarp/os/Clock.h
                 include/yarp/os/Connection.h
                 include/yarp/os/ConnectionReader.h
                 include/yarp/os/ConnectionState.h
                 include/yarp/os/ConnectionWriter.h
                 include/yarp/os/ConstString.h
                 include/yarp/os/Contactables.h
                 include/yarp/os/Contactable.h
                 include/yarp/os/Contact.h
                 include/yarp/os/ContactStyle.h
                 include/yarp/os/DummyConnector.h
                 include/yarp/os/Election.h
                 include/yarp/os/Event.h
                 include/yarp/os/Face.h
                 include/yarp/os/IConfig.h
                 include/yarp/os/InputProtocol.h
                 include/yarp/os/InputStream.h
                 include/yarp/os/LocalReader.h
                 include/yarp/os/LockGuard.h
                 include/yarp/os/Log.h
                 include/yarp/os/LogStream.h
                 include/yarp/os/ManagedBytes.h
                 include/yarp/os/MessageStack.h
                 include/yarp/os/ModifyingCarrier.h
                 include/yarp/os/MonitorObject.h
                 include/yarp/os/MultiNameSpace.h
                 include/yarp/os/Mutex.h
                 include/yarp/os/Name.h
                 include/yarp/os/NameSpace.h
                 include/yarp/os/NameStore.h
                 include/yarp/os/NestedContact.h
                 include/yarp/os/NetFloat32.h
                 include/yarp/os/NetFloat64.h
                 include/yarp/os/NetInt8.h
                 include/yarp/os/NetInt16.h
                 include/yarp/os/NetInt32.h
                 include/yarp/os/NetInt64.h
                 include/yarp/os/NetType.h
                 include/yarp/os/NetUint8.h
                 include/yarp/os/NetUint16.h
                 include/yarp/os/NetUint32.h
                 include/yarp/os/NetUint64.h
                 include/yarp/os/Network.h
                 include/yarp/os/NetworkClock.h
                 include/yarp/os/Nodes.h
                 include/yarp/os/Node.h
                 include/yarp/os/NullConnection.h
                 include/yarp/os/NullConnectionReader.h
                 include/yarp/os/NullConnectionWriter.h
                 include/yarp/os/Os.h
                 include/yarp/os/OutputProtocol.h
                 include/yarp/os/OutputStream.h
                 include/yarp/os/PeriodicThread.h
                 include/yarp/os/Ping.h
                 include/yarp/os/Portable.h
                 include/yarp/os/PortablePair.h
                 include/yarp/os/Port.h
                 include/yarp/os/PortInfo.h
                 include/yarp/os/PortReaderBuffer.h
                 include/yarp/os/PortReaderBuffer-inl.h
                 include/yarp/os/PortReaderBufferBase.h
                 include/yarp/os/PortReaderBufferBaseCreator.h
                 include/yarp/os/PortReaderCreator.h
                 include/yarp/os/PortReader.h
                 include/yarp/os/PortReport.h
                 include/yarp/os/PortWriterBuffer.h
                 include/yarp/os/PortWriterBufferBase.h
                 include/yarp/os/PortWriter.h
                 include/yarp/os/Property.h
                 include/yarp/os/Publisher.h
                 include/yarp/os/Random.h
                 include/yarp/os/RecursiveMutex.h
                 include/yarp/os/ResourceFinder.h
                 include/yarp/os/ResourceFinderOptions.h
                 include/yarp/os/RFModule.h
                 include/yarp/os/RFPlugin.h
                 include/yarp/os/RosNameSpace.h
                 include/yarp/os/Route.h
                 include/yarp/os/RpcClient.h
                 include/yarp/os/RpcServer.h
                 include/yarp/os/Searchable.h
                 include/yarp/os/Semaphore.h
                 include/yarp/os/SharedLibraryClassApi.h
                 include/yarp/os/SharedLibraryClassFactory.h
                 include/yarp/os/SharedLibraryClass.h
                 include/yarp/os/SharedLibraryFactory.h
                 include/yarp/os/SharedLibrary.h
                 include/yarp/os/ShiftStream.h
                 include/yarp/os/SizedWriter.h
                 include/yarp/os/Stamp.h
                 include/yarp/os/StringInputStream.h
                 include/yarp/os/StringOutputStream.h
                 include/yarp/os/Subscriber.h
                 include/yarp/os/SystemClock.h
                 include/yarp/os/Terminator.h
                 include/yarp/os/Things.h
                 include/yarp/os/Thread.h
                 include/yarp/os/Time.h
                 include/yarp/os/Timer.h
                 include/yarp/os/TwoWayStream.h
                 include/yarp/os/Type.h
                 include/yarp/os/TypedReader.h
                 include/yarp/os/TypedReaderCallback.h
                 include/yarp/os/TypedReaderCallback-inl.h
                 include/yarp/os/TypedReaderThread.h
                 include/yarp/os/TypedReaderThread-inl.h
                 include/yarp/os/UnbufferedContactable.h
                 include/yarp/os/Value.h
                 include/yarp/os/Vocab.h
                 include/yarp/os/Wire.h
                 include/yarp/os/WireLink.h
                 include/yarp/os/YarpNameSpace.h
                 include/yarp/os/YarpPlugin.h
                 include/yarp/os/YarpPluginSelector.h
                 include/yarp/os/YarpPluginSettings.h
                 include/yarp/os/SystemInfo.h
                 include/yarp/os/SystemInfoSerializer.h
                 include/yarp/os/QosStyle.h)

set(YARP_OS_IDL_HDRS include/yarp/os/idl/BareStyle.h
                     include/yarp/os/idl/BottleStyle.h
                     include/yarp/os/idl/Unwrapped.h
                     include/yarp/os/idl/WirePortable.h
                     include/yarp/os/idl/WireReader.h
                     include/yarp/os/idl/WireState.h
                     include/yarp/os/idl/WireTypes.h
                     include/yarp/os/idl/WireVocab.h
                     include/yarp/os/idl/WireWriter.h)

set(YARP_OS_IMPL_HDRS include/yarp/os/impl/AuthHMAC.h
                      include/yarp/os/impl/BottleImpl.h
                      include/yarp/os/impl/BufferedConnectionWriter.h
                      include/yarp/os/impl/ConnectionRecorder.h
                      include/yarp/os/impl/DgramTwoWayStream.h
                      include/yarp/os/impl/Dispatcher.h
                      include/yarp/os/impl/FakeFace.h
                      include/yarp/os/impl/FakeTwoWayStream.h
                      include/yarp/os/impl/FallbackNameClient.h
                      include/yarp/os/impl/FallbackNameServer.h
                      include/yarp/os/impl/HttpCarrier.h
                      include/yarp/os/impl/LocalCarrier.h
                      include/yarp/os/impl/Logger.h
                      include/yarp/os/impl/LogImpl.h
                      include/yarp/os/impl/LogForwarder.h
                      include/yarp/os/impl/McastCarrier.h
                      include/yarp/os/impl/MemoryOutputStream.h
                      include/yarp/os/impl/NameClient.h
                      include/yarp/os/impl/NameConfig.h
                      include/yarp/os/impl/NameserCarrier.h
                      include/yarp/os/impl/NameServer.h
                      include/yarp/os/impl/PlatformDirent.h
                      include/yarp/os/impl/PlatformDlfcn.h
                      include/yarp/os/impl/PlatformIfaddrs.h
                      include/yarp/os/impl/PlatformLimits.h
                      include/yarp/os/impl/PlatformNetdb.h
                      include/yarp/os/impl/PlatformSignal.h
                      include/yarp/os/impl/PlatformStdio.h
                      include/yarp/os/impl/PlatformStdlib.h
                      include/yarp/os/impl/PlatformSysStat.h
                      include/yarp/os/impl/PlatformSysWait.h
                      include/yarp/os/impl/PlatformTime.h
                      include/yarp/os/impl/PlatformUnistd.h
                      include/yarp/os/impl/PortCommand.h
                      include/yarp/os/impl/PortCore.h
                      include/yarp/os/impl/PortCoreAdapter.h
                      include/yarp/os/impl/PortCoreInputUnit.h
                      include/yarp/os/impl/PortCoreOutputUnit.h
                      include/yarp/os/impl/PortCorePacket.h
                      include/yarp/os/impl/PortCorePackets.h
                      include/yarp/os/impl/PortCoreUnit.h
                      include/yarp/os/impl/Protocol.h
                      include/yarp/os/impl/RFModuleFactory.h
                      include/yarp/os/impl/SocketTwoWayStream.h
                      include/yarp/os/impl/SplitString.h
                      include/yarp/os/impl/Storable.h
                      include/yarp/os/impl/StreamConnectionReader.h
                      include/yarp/os/impl/TcpAcceptor.h
                      include/yarp/os/impl/TcpCarrier.h
                      include/yarp/os/impl/TcpConnector.h
                      include/yarp/os/impl/TcpFace.h
                      include/yarp/os/impl/TcpStream.h
                      include/yarp/os/impl/Terminal.h
                      include/yarp/os/impl/TextCarrier.h
                      include/yarp/os/impl/ThreadImpl.h
                      include/yarp/os/impl/UdpCarrier.h)

set(YARP_OS_IMPL_POSIX_HDRS include/yarp/os/impl/posix/TcpAcceptor.h
                            include/yarp/os/impl/posix/TcpConnector.h
                            include/yarp/os/impl/posix/TcpStream.h)

set(YARP_OS_SRCS src/AbstractCarrier.cpp
                 src/AbstractContactable.cpp
                 src/AuthHMAC.cpp
                 src/Bottle.cpp
                 src/BottleImpl.cpp
                 src/BufferedConnectionWriter.cpp
                 src/Bytes.cpp
                 src/Carrier.cpp
                 src/Carriers.cpp
                 src/Clock.cpp
                 src/Connection.cpp
                 src/ConnectionReader.cpp
                 src/ConnectionRecorder.cpp
                 src/ConnectionState.cpp
                 src/ConnectionWriter.cpp
                 src/Contactable.cpp
                 src/Contactables.cpp
                 src/Contact.cpp
                 src/ContactStyle.cpp
                 src/DgramTwoWayStream.cpp
                 src/Dispatcher.cpp
                 src/DummyConnector.cpp
                 src/Event.cpp
                 src/FakeFace.cpp
                 src/FallbackNameClient.cpp
                 src/FallbackNameServer.cpp
                 src/HttpCarrier.cpp
                 src/IConfig.cpp
                 src/InputStream.cpp
                 src/LocalCarrier.cpp
                 src/Log.cpp
                 src/Logger.cpp
                 src/LogForwarder.cpp
                 src/ManagedBytes.cpp
                 src/McastCarrier.cpp
                 src/MessageStack.cpp
                 src/ModifyingCarrier.cpp
                 src/MultiNameSpace.cpp
                 src/NameClient.cpp
                 src/NameConfig.cpp
                 src/Name.cpp
                 src/NameserCarrier.cpp
                 src/NameServer.cpp
                 src/NameSpace.cpp
                 src/NameStore.cpp
                 src/MonitorObject.cpp
                 src/NestedContact.cpp
                 src/NetFloat32.cpp
                 src/NetFloat64.cpp
                 src/NetInt16.cpp
                 src/NetInt32.cpp
                 src/NetInt64.cpp
                 src/NetType.cpp
                 src/NetUint16.cpp
                 src/NetUint32.cpp
                 src/NetUint64.cpp
                 src/Network.cpp
                 src/NetworkClock.cpp
                 src/Nodes.cpp
                 src/Node.cpp
                 src/NullConnection.cpp
                 src/NullConnectionReader.cpp
                 src/NullConnectionWriter.cpp
                 src/Os.cpp
                 src/OutputStream.cpp
                 src/PeriodicThread.cpp
                 src/Ping.cpp
                 src/PlatformTime.cpp
                 src/Portable.cpp
                 src/PortablePair.cpp
                 src/PortCommand.cpp
                 src/PortCore.cpp
                 src/PortCoreAdapter.cpp
                 src/PortCoreInputUnit.cpp
                 src/PortCoreOutputUnit.cpp
                 src/Port.cpp
                 src/PortInfo.cpp
                 src/PortReaderBufferBase.cpp
                 src/PortReaderBufferBaseCreator.cpp
                 src/PortReader.cpp
                 src/PortReaderCreator.cpp
                 src/PortReport.cpp
                 src/PortWriterBufferBase.cpp
                 src/PortWriter.cpp
                 src/Property.cpp
                 src/Protocol.cpp
                 src/Random.cpp
                 src/ResourceFinder.cpp
                 src/ResourceFinderOptions.cpp
                 src/RFModule.cpp
                 src/RFModuleFactory.cpp
                 src/RFPlugin.cpp
                 src/RosNameSpace.cpp
                 src/Route.cpp
                 src/RpcClient.cpp
                 src/RpcServer.cpp
                 src/Searchable.cpp
                 src/Semaphore.cpp
                 src/SharedLibrary.cpp
                 src/SharedLibraryFactory.cpp
                 src/ShiftStream.cpp
                 src/SizedWriter.cpp
                 src/SocketTwoWayStream.cpp
                 src/SplitString.cpp
                 src/Stamp.cpp
                 src/Storable.cpp
                 src/StreamConnectionReader.cpp
                 src/SystemClock.cpp
                 src/SystemInfo.cpp
                 src/SystemInfoSerializer.cpp
                 src/TcpCarrier.cpp
                 src/TcpFace.cpp
                 src/Terminal.cpp
                 src/Terminator.cpp
                 src/TextCarrier.cpp
                 src/Things.cpp
                 src/Thread.cpp
                 src/ThreadImpl.cpp
                 src/Time.cpp
                 src/Timer.cpp
                 src/TwoWayStream.cpp
                 src/Type.cpp
                 src/UdpCarrier.cpp
                 src/Value.cpp
                 src/Vocab.cpp
                 src/YarpNameSpace.cpp
                 src/YarpPlugin.cpp
                 src/WireLink.cpp
                 src/WirePortable.cpp
                 src/WireReader.cpp
                 src/WireState.cpp
                 src/WireWriter.cpp
                 src/QosStyle.cpp)

if(NOT YARP_NO_DEPRECATED) # Since YARP 3.0.0
  list(APPEND YARP_OS_HDRS include/yarp/os/ConstString.h
                           include/yarp/os/RateThread.h
                           include/yarp/os/Runnable.h)

  list(APPEND YARP_OS_SRCS src/Mutex.cpp
                           src/RateThread.cpp
                           src/RecursiveMutex.cpp
                           src/Runnable.cpp)
endif()


if(SKIP_ACE)
  list(APPEND YARP_OS_SRCS src/posix/TcpAcceptor.cpp
                           src/posix/TcpConnector.cpp
                           src/posix/TcpStream.cpp)
endif()

if(APPLE)
  list(APPEND YARP_OS_IMPL_HDRS include/yarp/os/impl/MacOSAPI.h)
  list(APPEND YARP_OS_SRCS src/MacOSAPI.mm)
endif()

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Source Files"
             FILES ${YARP_OS_SRCS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Header Files"
             FILES ${YARP_OS_HDRS}
                   ${YARP_OS_IDL_HDRS}
                   ${YARP_OS_IMPL_HDRS}
                   ${YARP_OS_IMPL_POSIX_HDRS})

option(ENABLE_YARPRUN_LOG "Enable yarprun log file in temp dir" OFF)
if (ENABLE_YARPRUN_LOG)
    add_definitions(-DYARPRUN_LOG)
endif(ENABLE_YARPRUN_LOG)

add_library(YARP_OS ${YARP_OS_SRCS}
                    ${YARP_OS_HDRS}
                    ${YARP_OS_IDL_HDRS}
                    ${YARP_OS_IMPL_HDRS}
                    ${YARP_OS_IMPL_POSIX_HDRS})
add_library(YARP::YARP_OS ALIAS YARP_OS)

target_include_directories(YARP_OS PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                          $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(YARP_OS PUBLIC YARP::YARP_conf)
list(APPEND YARP_OS_PUBLIC_DEPS YARP_conf)

target_include_directories(YARP_OS SYSTEM PRIVATE ${hmac_INCLUDE_DIRS})
target_link_libraries(YARP_OS PRIVATE ${hmac_LIBRARIES})
# list(APPEND YARP_OS_PUBLIC_DEPS hmac)

if(YARP_HAS_ACE)
  target_compile_definitions(YARP_OS PRIVATE YARP_HAS_ACE)
  target_link_libraries(YARP_OS PRIVATE ACE::ACE)
  list(APPEND YARP_OS_PRIVATE_DEPS ACE)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  # Required for using std::thread on linux
  target_link_libraries(YARP_OS PRIVATE pthread)

  # Required by SharedLibrary.cpp (dlopen, dlsym, dlclose, dlerror)
  target_link_libraries(YARP_OS PRIVATE ${CMAKE_DL_LIBS})
endif()

if(APPLE)
  # Required by MacOSAPI.mm
  target_link_libraries(YARP_OS PRIVATE "-framework Foundation")
endif()

if(MSVC)
  # Required by Time.cpp (timeBeginPeriod, timeEndPeriod functions)
  target_link_libraries(YARP_OS PRIVATE "Winmm")
endif()

if(YARP_HAS_Libedit)
  target_compile_definitions(YARP_OS PRIVATE YARP_HAS_Libedit)
  target_link_libraries(YARP_OS PRIVATE Libedit::libedit)
  list(APPEND YARP_OS_PRIVATE_DEPS Libedit)
endif()

target_compile_features(YARP_OS PUBLIC cxx_nullptr
                                       cxx_override
                                       cxx_rvalue_references)

set_property(TARGET YARP_OS PROPERTY PUBLIC_HEADER ${YARP_OS_HDRS})
set_property(TARGET YARP_OS PROPERTY PRIVATE_HEADER ${YARP_OS_IMPL_HDRS})
set_property(TARGET YARP_OS PROPERTY VERSION ${YARP_VERSION_SHORT})
set_property(TARGET YARP_OS PROPERTY SOVERSION ${YARP_SOVERSION})


option(YARP_TEST_HEAP "Replace new/delete operators with test harness (may be incompatible with ACE)" FALSE)
mark_as_advanced(YARP_TEST_HEAP)
if (YARP_TEST_HEAP)
    add_definitions(-DYARP_TEST_HEAP)
endif()

install(TARGETS YARP_OS
        EXPORT YARP_OS
        COMPONENT YARP_OS
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp/os
        PRIVATE_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp/os/impl)
install(FILES ${YARP_OS_IDL_HDRS}
        COMPONENT YARP_OS
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp/os/idl)
install(FILES ${YARP_OS_IMPL_POSIX_HDRS}
        COMPONENT development
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp/os/impl/posix)

set(YARP_OS_PUBLIC_DEPS ${YARP_OS_PUBLIC_DEPS} PARENT_SCOPE)
set(YARP_OS_PRIVATE_DEPS ${YARP_OS_PRIVATE_DEPS} PARENT_SCOPE)

set_property(TARGET YARP_OS PROPERTY FOLDER "Libraries")
