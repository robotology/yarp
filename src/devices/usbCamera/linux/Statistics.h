#pragma once

#include <string>

class Statistics
{
   public:
	Statistics(const std::string& info) : info_(info) { timeStart_ = yarp::os::Time::now(); };
	void add()
	{
        ++frameCounter_;
		double timeNow = yarp::os::Time::now();
		double timeElapsed;
		if ((timeElapsed = timeNow - timeStart_) >= statPeriod_)
		{
			yCInfo(USBCAMERA)<<info_<<" frame number:"<<frameCounter_ <<" fps:"<<((double)frameCounter_)/statPeriod_<<" interval:"<< timeElapsed<<" sec.";

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