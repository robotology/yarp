// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#ifndef _YARP2_SERVERINERTIAL_
#define _YARP2_SERVERINERTIAL_

#include <stdio.h>

//#include <yarp/XSensMTx.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/GenericSensorInterfaces.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/Bottle.h>


namespace yarp
{
	namespace dev
	{
		class ServerInertial;
	}
}


/**
 * @ingroup dev_impl_wrapper
 *
 * Export an inertial sensor.
 * The network interface is a single Port.
 * We will stream bottles with 12 floats:
 * 0  1   2  = Euler orientation data (Kalman filter processed)
 * 3  4   5  = Calibrated 3-axis (X, Y, Z) acceleration data
 * 6  7   8  = Calibrated 3-axis (X, Y, Z) gyroscope data
 * 9 10 11   = Calibrated 3-axis (X, Y, Z) magnetometer data
 *
 * @author Alexis Maldonado, Radu Bogdan Rusu
 */
class yarp::dev::ServerInertial : public DeviceDriver,
			private yarp::os::Thread,
			public yarp::os::PortReader,
			public yarp::dev::IGenericSensor
{
	private:
		bool spoke;
		PolyDriver poly;
		IGenericSensor *IMU; //The inertial device
		yarp::os::Port p;
		yarp::os::PortWriterBuffer<yarp::os::Bottle> writer;
	public:
		/**
		 * Constructor.
		 */
		ServerInertial()
		{
			IMU = NULL;
			spoke = false;
		}

		/**
		* Configure with a set of options. These are:
		* <TABLE>
		* <TR><TD> subdevice </TD><TD> Common name of device to wrap (e.g. "test_grabber"). </TD></TR>
		* <TR><TD> name </TD><TD> Port name to assign to this server (default /grabber). </TD></TR>
		* </TABLE>
		*
		* @param config The options to use
		* @return true iff the object could be configured.
		*/
		virtual bool open(yarp::os::Searchable& config)
		{
			p.setReader(*this);

			//Look for the device name (serial Port). Usually /dev/ttyUSB0
			yarp::os::Value *name;
			if (config.check("subdevice",name))
			{
				printf("Subdevice %s\n", name->toString().c_str());
				if (name->isString())
				{
					// maybe user isn't doing nested configuration
					yarp::os::Property p;
					p.fromString(config.toString());
					p.put("device",name->toString());
					poly.open(p);
				}
				else
					poly.open(*name);
				if (!poly.isValid())
					printf("cannot make <%s>\n", name->toString().c_str());
			}
			else
			{
				printf("\"--subdevice <name>\" not set for server_framegrabber\n");
				return false;
			}
			if (poly.isValid())
				poly.view(IMU);

			if (IMU!=NULL)
				writer.attach(p);

			//Look for the portname to register (--name option)
			if (config.check("name",name))
				p.open(name->asString());
			else
				p.open("/inertial");

			//Look for the portname to register (--name option)
//			p.open(config.check("name", Value("/inertial")).asString());
			
			if (IMU!=NULL)
			{
				start();
				return true;
			}
			else
				return false;
		}

		virtual bool close()
		{
			return true;
		}


		virtual bool getInertial(yarp::os::Bottle &bot)
		{
			if (IMU==NULL)
			{
				return false;
			}
			else
			{
				int nchannels;
				IMU->getChannels (&nchannels);
				
				yarp::sig::Vector indata(nchannels);
				bool worked(false);

				worked=IMU->read(indata);
				if (worked)
				{
					bot.clear();
					
			    		// Euler+accel+gyro+magn orientation values
			                for (int i = 0; i < nchannels; i++)
					    bot.addDouble (indata[i]);
				}
				else
				{
					bot.clear(); //dummy info.
				}

				return(worked);
			}
		}

		virtual void run()
		{
			printf("Server Inertial starting\n");
			while (!isStopping())
			{
				if (IMU!=NULL)
				{
					yarp::os::Bottle& bot = writer.get();
					getInertial(bot);

					if (!spoke)
					{
						printf("Writting an Inertial measurement.\n");
						spoke = true;
					}
					writer.write();
					
				}
				yarp::os::Time::delay (0.001);
			}
			printf("Server Inertial stopping\n");
		}

		virtual bool read(ConnectionReader& connection)
		{
			yarp::os::Bottle cmd, response;
			cmd.read(connection);
			printf("command received: %s\n", cmd.toString().c_str());
			// We receive a command but don't do anything with it.
			return true;
		}

		virtual bool read(yarp::sig::Vector &out)
		{
			if (IMU == NULL) { return false; }
			return IMU->read (out);
		}
		
		virtual bool getChannels(int *nc)
		{
			if (IMU == NULL) { return false; }
			return IMU->getChannels (nc);
		}
};

#endif

