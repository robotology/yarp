// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
//
// $Id: YARPConfigLinux.h,v 1.2 2006-05-15 15:57:58 eshuy Exp $
// YARP config file Linux.
//
//

#ifndef __YARPConfigLinuxh__
#define __YARPConfigLinuxh__

//
// included by YARPConfig.h

#ifndef __LINUX__

#	define __LINUX__

#endif

///
/// configuration flags.
///
#define SINGLE_MUTEX_FOR_REFCOUNTED 1
#define UPDATED_PORT 1
#define MAX_PACKET (128*128*3+100)

/// although the limit for communicating across a net is set to MAX_PACKET
/// TCP and SHMEM protocols support larger packets. This is the limit for
/// SHMEM communication.
#define MAX_SHMEM_BUFFER (384*288*3+100)		

/// disable TCP Nagle's algorithm (experimental).
#define YARP_TCP_NO_DELAY 1

/// set the default protocol in port creation.
#define YARP_DEFAULT_PROTOCOL	YARP_TCP

/// set the def protocol number of req ports.
#define YARP_PROTOCOL_REGPORTS  11

/// set the number of ports used by UDP for a single port.
#define YARP_UDP_REGPORTS       11

/// read timeout on socket transport (in seconds).
#define YARP_SOCK_TIMEOUT		5

/// read short timeout on socket transport (in seconds). - used for waiting short latency delay.
#define YARP_SHORT_SOCK_TIMEOUT	1

/// magic number for handshaking connections.
#define YARP_MAGIC_NUMBER	7777

/// alignment bytes required for mmx/ipl instructions.
#define YARP_IMAGE_ALIGN 8

/// default length for general purpose strings.
#define YARP_STRING_LEN 256

/// default network name (must match your default net description in the name server config).
#define YARP_DEFAULT_NET "default"

#include <ace/config.h>

#endif
