# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

project(YARP_OS)

set(YARP_OS_HDRS include/yarp/os/AbstractCarrier.h
                 include/yarp/os/AbstractContactable.h
                 include/yarp/os/all.h
                 include/yarp/os/api.h
                 include/yarp/os/begin_pack_for_net.h
                 include/yarp/os/BinPortable.h
                 include/yarp/os/Bottle.h
                 include/yarp/os/BufferedPort.h
                 include/yarp/os/Bytes.h
                 include/yarp/os/Carrier.h
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
                 include/yarp/os/end_pack_for_net.h
                 include/yarp/os/Event.h
                 include/yarp/os/Face.h
                 include/yarp/os/IConfig.h
                 include/yarp/os/InputProtocol.h
                 include/yarp/os/InputStream.h
                 include/yarp/os/LocalReader.h
                 include/yarp/os/LockGuard.h
                 include/yarp/os/Log.h
                 include/yarp/os/ManagedBytes.h
                 include/yarp/os/ModifyingCarrier.h
                 include/yarp/os/Module.h
                 include/yarp/os/MultiNameSpace.h
                 include/yarp/os/Mutex.h
                 include/yarp/os/Name.h
                 include/yarp/os/NameSpace.h
                 include/yarp/os/NameStore.h
                 include/yarp/os/NestedContact.h
                 include/yarp/os/NetFloat32.h
                 include/yarp/os/NetFloat64.h
                 include/yarp/os/NetInt16.h
                 include/yarp/os/NetInt32.h
                 include/yarp/os/NetInt64.h
                 include/yarp/os/NetType.h
                 include/yarp/os/NetUint16.h
                 include/yarp/os/NetUint32.h
                 include/yarp/os/NetUint64.h
                 include/yarp/os/Network.h
                 include/yarp/os/NetworkClock.h
                 include/yarp/os/Nodes.h
                 include/yarp/os/Node.h
                 include/yarp/os/NullConnectionReader.h
                 include/yarp/os/NullConnectionWriter.h
                 include/yarp/os/Os.h
                 include/yarp/os/OutputProtocol.h
                 include/yarp/os/OutputStream.h
                 include/yarp/os/Ping.h
                 include/yarp/os/Portable.h
                 include/yarp/os/PortablePair.h
                 include/yarp/os/Port.h
                 include/yarp/os/PortInfo.h
                 include/yarp/os/PortReaderBuffer.h
                 include/yarp/os/PortReaderCreator.h
                 include/yarp/os/PortReader.h
                 include/yarp/os/PortReport.h
                 include/yarp/os/PortWriterBuffer.h
                 include/yarp/os/PortWriter.h
                 include/yarp/os/Property.h
                 include/yarp/os/Publisher.h
                 include/yarp/os/Random.h
                 include/yarp/os/RateThread.h
                 include/yarp/os/ResourceFinder.h
                 include/yarp/os/ResourceFinderOptions.h
                 include/yarp/os/RFModule.h
                 include/yarp/os/RosNameSpace.h
                 include/yarp/os/Route.h
                 include/yarp/os/RpcClient.h
                 include/yarp/os/RpcServer.h
                 include/yarp/os/Run.h
                 include/yarp/os/Runnable.h
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
                 include/yarp/os/TwoWayStream.h
                 include/yarp/os/Type.h
                 include/yarp/os/UnbufferedContactable.h
                 include/yarp/os/Value.h
                 include/yarp/os/Vocab.h
                 include/yarp/os/Wire.h
                 include/yarp/os/WireLink.h
                 include/yarp/os/YarpNameSpace.h
                 include/yarp/os/YarpPlugin.h
                 include/yarp/os/YarpPluginSelector.h
                 include/yarp/os/YarpPluginSettings.h
                 include/yarp/os/YarprunLogger.h)

set(YARP_OS_IDL_HDRS include/yarp/os/idl/BareStyle.h
                     include/yarp/os/idl/BottleStyle.h
                     include/yarp/os/idl/Unwrapped.h
                     include/yarp/os/idl/WirePortable.h
                     include/yarp/os/idl/WireReader.h
                     include/yarp/os/idl/WireState.h
                     include/yarp/os/idl/WireTypes.h
                     include/yarp/os/idl/WireVocab.h
                     include/yarp/os/idl/WireWriter.h)

set(YARP_OS_IMPL_HDRS include/yarp/os/impl/ACESemaphoreImpl.h
                      include/yarp/os/impl/AuthHMAC.h
                      include/yarp/os/impl/BottleImpl.h
                      include/yarp/os/impl/BufferedConnectionWriter.h
                      include/yarp/os/impl/Carriers.h
                      include/yarp/os/impl/Companion.h
                      include/yarp/os/impl/CXX11SemaphoreImpl.h
                      include/yarp/os/impl/DgramTwoWayStream.h
                      include/yarp/os/impl/Dispatcher.h
                      include/yarp/os/impl/FakeFace.h
                      include/yarp/os/impl/FakeTwoWayStream.h
                      include/yarp/os/impl/FallbackNameClient.h
                      include/yarp/os/impl/FallbackNameServer.h
                      include/yarp/os/impl/hmac/hmac_sha2.h
                      include/yarp/os/impl/hmac/sha2.h
                      include/yarp/os/impl/HttpCarrier.h
                      include/yarp/os/impl/IOException.h
                      include/yarp/os/impl/LocalCarrier.h
                      include/yarp/os/impl/Logger.h
                      include/yarp/os/impl/MachSemaphoreImpl.h
                      include/yarp/os/impl/McastCarrier.h
                      include/yarp/os/impl/MemoryOutputStream.h
                      include/yarp/os/impl/NameClient.h
                      include/yarp/os/impl/NameConfig.h
                      include/yarp/os/impl/NameserCarrier.h
                      include/yarp/os/impl/NameServer.h
                      include/yarp/os/impl/PlatformList.h
                      include/yarp/os/impl/PlatformMap.h
                      include/yarp/os/impl/PlatformSet.h
                      include/yarp/os/impl/PlatformSignal.h
                      include/yarp/os/impl/PlatformSize.h
                      include/yarp/os/impl/PlatformStdio.h
                      include/yarp/os/impl/PlatformStdlib.h
                      include/yarp/os/impl/PlatformThread.h
                      include/yarp/os/impl/PlatformTime.h
                      include/yarp/os/impl/PlatformVector.h
                      include/yarp/os/impl/PortCommand.h
                      include/yarp/os/impl/PortCore.h
                      include/yarp/os/impl/PortCoreInputUnit.h
                      include/yarp/os/impl/PortCoreOutputUnit.h
                      include/yarp/os/impl/PortCorePacket.h
                      include/yarp/os/impl/PortCoreUnit.h
                      include/yarp/os/impl/PortManager.h
                      include/yarp/os/impl/POSIXSemaphoreImpl.h
                      include/yarp/os/impl/Protocol.h
                      include/yarp/os/impl/RunCheckpoints.h
                      include/yarp/os/impl/Runnable.h
                      include/yarp/os/impl/RunProcManager.h
                      include/yarp/os/impl/RunReadWrite.h
                      include/yarp/os/impl/SemaphoreImpl.h
                      include/yarp/os/impl/ShmemCarrier.h
                      include/yarp/os/impl/ShmemHybridStream.h
                      include/yarp/os/impl/ShmemInputStream.h
                      include/yarp/os/impl/ShmemOutputStream.h
                      include/yarp/os/impl/ShmemTwoWayStream.h
                      include/yarp/os/impl/ShmemTypes.h
                      include/yarp/os/impl/SocketTwoWayStream.h
                      include/yarp/os/impl/SplitString.h
                      include/yarp/os/impl/StreamConnectionReader.h
                      include/yarp/os/impl/String.h
                      include/yarp/os/impl/SystemInfo.h
                      include/yarp/os/impl/SystemInfoSerializer.h
                      include/yarp/os/impl/TcpAcceptor.h
                      include/yarp/os/impl/TcpCarrier.h
                      include/yarp/os/impl/TcpConnector.h
                      include/yarp/os/impl/TcpFace.h
                      include/yarp/os/impl/TcpStream.h
                      include/yarp/os/impl/TextCarrier.h
                      include/yarp/os/impl/ThreadImpl.h
                      include/yarp/os/impl/UdpCarrier.h
                      include/yarp/os/impl/UnitTest.h
                      include/yarp/os/impl/ydr.h)

set(YARP_OS_SRCS src/AbstractCarrier.cpp
                 src/AuthHMAC.cpp
                 src/Bottle.cpp
                 src/BottleImpl.cpp
                 src/BufferedConnectionWriter.cpp
                 src/Bytes.cpp
                 src/Carriers.cpp
                 src/Companion.cpp
                 src/ConnectionReader.cpp
                 src/ConnectionWriter.cpp
                 src/ConstString.cpp
                 src/Contactable.cpp
                 src/Contact.cpp
                 src/ContactStyle.cpp
                 src/DgramTwoWayStream.cpp
                 src/Dispatcher.cpp
                 src/DummyConnector.cpp
                 src/Election.cpp
                 src/Event.cpp
                 src/FakeFace.cpp
                 src/FallbackNameClient.cpp
                 src/FallbackNameServer.cpp
                 src/HttpCarrier.cpp
                 src/IConfig.cpp
                 src/InputStream.cpp
                 src/LocalCarrier.cpp
                 src/LockGuard.cpp
                 src/Logger.cpp
                 src/ManagedBytes.cpp
                 src/ModifyingCarrier.cpp
                 src/Module.cpp
                 src/MultiNameSpace.cpp
                 src/Mutex.cpp
                 src/NameClient.cpp
                 src/NameConfig.cpp
                 src/Name.cpp
                 src/NameserCarrier.cpp
                 src/NameServer.cpp
                 src/NameSpace.cpp
                 src/NameStore.cpp
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
                 src/NullConnectionReader.cpp
                 src/NullConnectionWriter.cpp
                 src/Os.cpp
                 src/Ping.cpp
                 src/Portable.cpp
                 src/PortablePair.cpp
                 src/PortCommand.cpp
                 src/PortCore.cpp
                 src/PortCoreInputUnit.cpp
                 src/PortCoreOutputUnit.cpp
                 src/Port.cpp
                 src/PortInfo.cpp
                 src/PortReaderBuffer.cpp
                 src/PortReader.cpp
                 src/PortReaderCreator.cpp
                 src/PortReport.cpp
                 src/PortWriterBuffer.cpp
                 src/PortWriter.cpp
                 src/Property.cpp
                 src/Protocol.cpp
                 src/Random.cpp
                 src/RateThread.cpp
                 src/ResourceFinder.cpp
                 src/ResourceFinderOptions.cpp
                 src/RFModule.cpp
                 src/RosNameSpace.cpp
                 src/RpcClient.cpp
                 src/RpcServer.cpp
                 src/RunCheckpoints.cpp
                 src/Run.cpp
                 src/Runnable.cpp
                 src/RunProcManager.cpp
                 src/RunReadWrite.cpp
                 src/Searchable.cpp
                 src/Semaphore.cpp
                 src/SharedLibrary.cpp
                 src/SharedLibraryFactory.cpp
                 src/ShmemHybridStream.cpp
                 src/ShmemInputStream.cpp
                 src/ShmemOutputStream.cpp
                 src/ShmemTwoWayStream.cpp
                 src/SocketTwoWayStream.cpp
                 src/Stamp.cpp
                 src/StreamConnectionReader.cpp
                 src/SystemClock.cpp
                 src/SystemInfo.cpp
                 src/TcpAcceptor.cpp
                 src/TcpCarrier.cpp
                 src/TcpConnector.cpp
                 src/TcpFace.cpp
                 src/TcpStream.cpp
                 src/Terminator.cpp
                 src/TextCarrier.cpp
                 src/Things.cpp
                 src/Thread.cpp
                 src/ThreadImpl.cpp
                 src/Time.cpp
                 src/TwoWayStream.cpp
                 src/UnitTest.cpp
                 src/Value.cpp
                 src/Vocab.cpp
                 src/YarpNameSpace.cpp
                 src/YarpPlugin.cpp
                 src/YarprunLogger.cpp
                 src/WireReader.cpp
                 src/WireState.cpp
                 src/WireWriter.cpp
                 src/ydr.cpp
                 src/hmac_sha2.c
                 src/sha2.c)


if (NOT SKIP_ACE)
  # these carriers have not yet been implemented without ACE
  set(YARP_OS_SRCS ${YARP_OS_SRCS}
                   src/McastCarrier.cpp
                   src/ShmemCarrier.cpp
                   src/UdpCarrier.cpp)
endif()

source_group("Source Files" FILES ${YARP_OS_SRCS})
source_group("Header Files" FILES ${YARP_OS_HDRS})
source_group("IDL Header Files" FILES ${YARP_OS_IDL_HDRS})
source_group("Implementation Header Files" FILES ${YARP_OS_IMPL_HDRS})

option(USE_PORT_AUTHENTICATION "Use a 3-way passphrase authentication for connections to ports" FALSE)
mark_as_advanced(USE_PORT_AUTHENTICATION)
if (USE_PORT_AUTHENTICATION)
    add_definitions(-DPORT_AUTH)
endif (USE_PORT_AUTHENTICATION)

option(ENABLE_YARPRUN_LOG "Enable yarprun log file in temp dir" OFF)
if (ENABLE_YARPRUN_LOG)
    add_definitions(-DYARPRUN_LOG)
endif(ENABLE_YARPRUN_LOG)

set_property(GLOBAL APPEND PROPERTY YARP_TREE_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
get_property(YARP_TREE_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)

include_directories(${YARP_TREE_INCLUDE_DIRS})
if (NOT SKIP_ACE)
  include_directories(${ACE_INCLUDE_DIRS})
endif ()

add_library(YARP_OS ${YARP_OS_SRCS}
                    ${YARP_OS_HDRS}
                    ${YARP_OS_IDL_HDRS}
                    ${YARP_OS_IMPL_HDRS})
target_link_libraries(YARP_OS LINK_PRIVATE ${ACE_LIBRARIES})

set_property(TARGET YARP_OS PROPERTY PUBLIC_HEADER ${YARP_OS_HDRS})
set_property(TARGET YARP_OS PROPERTY PRIVATE_HEADER ${YARP_OS_IMPL_HDRS})

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(YARP_OS LINK_PRIVATE pthread)
endif()

if (YARP_USE_READLINE)
    add_definitions(-DWITH_READLINE)
    include_directories(${Readline_INCLUDE_DIRS})
    target_link_libraries(YARP_OS LINK_PRIVATE ${Readline_LIBRARY})
endif ()

install(TARGETS YARP_OS
        EXPORT YARP
        COMPONENT runtime
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp/os
        PRIVATE_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp/os/impl)
install(FILES ${YARP_OS_IDL_HDRS}
        COMPONENT development
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp/os/idl)

set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_OS)
set_property(TARGET YARP_OS PROPERTY INCLUDE_DIRS ${YARP_TREE_INCLUDE_DIRS})
