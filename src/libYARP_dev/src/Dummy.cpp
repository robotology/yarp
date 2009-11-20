// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2009 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

// This is a trivial file.
// When device modules are compiled, and configured to be automatically
// loaded, all device code is moved to the "yarpmod" library in order to 
// avoid a circular dependency which can occasionally cause trouble.

extern "C" int libYARP_dev_dummmy() {
	return 1;
}
