/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */
//# @author Luca Tricerri <luca.tricerri@iit.it>
#pragma once

#include <string>

#include "USBcameraLogComponent.h"


//For FPS statistics purpouse 
class Statistics
{
   public:
	explicit Statistics(const std::string& info) : info_(info)
	{
		timeStart_ = yarp::os::Time::now();
	};
	void add()
	{
		++frameCounter_;
		double timeNow = yarp::os::Time::now();
		double timeElapsed;
		if ((timeElapsed = timeNow - timeStart_) >= statPeriod_)
		{
			yCInfo(USBCAMERA) << info_ << " frame number:" << frameCounter_ << " fps:" << (static_cast<double>(frameCounter_)) / statPeriod_ << " interval:" << timeElapsed << " sec.";

			frameCounter_ = 0;
			timeStart_ = timeNow;
		}
	}

   private:
	std::string info_;
	double timeStart_{0};
	unsigned int frameCounter_{0};
	static constexpr double statPeriod_{5.0};
};
