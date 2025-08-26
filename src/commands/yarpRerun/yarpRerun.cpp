/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Port.h>
#include <yarp/os/Stamp.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Network.h>

#include <yarp/os/RFModule.h>
#include <yarp/os/Portable.h>

#include <yarp/dev/all.h>
#include <yarp/dev/MultipleAnalogSensorsInterfaces.h>
#include <yarp/dev/SensorStreamingData.h>
#include <yarp/dev/impl/jointData.h>
#include <yarp/sig/LaserScan2D.h>
#include <yarp/sig/PointCloud.h>

#include <rerun.hpp>


using namespace yarp::os;


class RerunLogger : public yarp::os::RFModule
{
    public:
    rerun::RecordingStream* m_rec = nullptr;
    yarp::os::BufferedPort<yarp::os::Bottle> m_port;

    RerunLogger()
    {
    }

    ~RerunLogger()
    {
        if (m_rec)
        {
            delete m_rec;
            m_rec = nullptr;
        }
        m_port.close();
    }

    bool process_RGBImage (const Bottle* pbot);
    bool process_SensorStreamingData (const Bottle* pbot);
    bool process_Bottle (const Bottle* pbot);
    bool process_Vector (const Bottle* pbot);
    bool process_RobotState (const Bottle* pbot);
    bool process_DepthImage (const Bottle* pbot);
    bool process_Lidar2D (const Bottle* pbot);
    bool process_PointCloud (const Bottle* pbot);
    void process_Bottle_nested (const Bottle* pbot, std::vector<size_t> path);

    bool configure(yarp::os::ResourceFinder& rf) override;

    bool interruptModule() override;

    double getPeriod() override
    {  return 1.0; }

    bool updateModule() override;


    private:
    bool m_isRunning = true;
    std::string m_localPortname = "/yarprerun:i";
    std::string m_remotePortname;
    std::string m_datatype = "Bottle";
    std::string m_session_name = "yarprerun";

};

bool RerunLogger::interruptModule()
{
    m_port.interrupt();
    m_isRunning = false;
    return true;
}

bool RerunLogger::configure(yarp::os::ResourceFinder& rf)
{
    if (rf.check("help"))
    {
        yInfo();
        yInfo() << "yarpRerun --data <dataType> [--local <local_portname>] [--remote <remote_portname>] [--session_name <session_name>]";
        yInfo() << "<dataType> : Supported types are: SensorStreamingData, Bottle, RGBImage, DepthImage, Vector, RobotState, Lidar2D, Pointcloud ";
        yInfo() << "<local_portname> : Optional. Sets the name of the input port. By default: /yarprerun:i";
        yInfo() << "<remote_portname> : Optional. If set, it automatically tries to connect the specified port";
        yInfo() << "<session_name> : Optional. Sets the name of the rerun session. By default: yarprerun";
        yInfo();
        return false;
    }

    if (rf.check("local"))
    {
        m_localPortname = rf.find("local").asString();
    }

    if (rf.check("remote"))
    {
        m_remotePortname = rf.find("remote").asString();
    }

    if (rf.check("data"))
    {
        m_datatype = rf.find("data").asString();
    }
    yInfo() << "Selected " << m_datatype << " data type";

    bool b = m_port.open(m_localPortname);
    if (!b)
    {
        yError() << "Unable to open port" << m_localPortname;
        return false;
    }

    if (rf.check("session_name"))
    {
        m_session_name = rf.find("session_name").asString();
    }
    m_rec = new rerun::RecordingStream(m_session_name);
    m_rec->spawn().exit_on_failure();

    if (m_remotePortname != "")
    {
        b = yarp::os::Network::connect(m_remotePortname,m_localPortname);
        if (!b)
        {
            yError() << "Unable to connect to remote port: " << m_remotePortname;
            return false;
        }
        else
        {
            yInfo() << "Connected to: " << m_remotePortname;
        }
    }
    else
    {
        yInfo() << "Waiting for connection..";
    }

    return true;

}

bool RerunLogger::updateModule()
{
    while (m_isRunning)
    {
        yarp::os::Bottle* prt = m_port.read(true);
        if (prt == nullptr) { return true; }

        yarp::os::Stamp envelope;
        bool env_ok = m_port.getEnvelope(envelope);
        if (env_ok)
            {m_rec->set_time_seconds("time", envelope.getTime());}
        else
            {m_rec->set_time_seconds("time", yarp::os::Time::now());}

        if (m_datatype == "SensorStreamingData")
        {
            process_SensorStreamingData(prt);
        }
        else if (m_datatype == "Bottle")
        {
            process_Bottle(prt);
        }
        else if (m_datatype == "RGBImage")
        {
            process_RGBImage(prt);
        }
        else if (m_datatype == "DepthImage")
        {
            process_DepthImage(prt);
        }
        else if (m_datatype == "Vector")
        {
            process_Vector(prt);
        }
        else if (m_datatype == "RobotState")
        {
            process_RobotState(prt);
        }
        else if (m_datatype == "Lidar2D")
        {
            process_Lidar2D(prt);
        }
        else if (m_datatype == "Pointcloud")
        {
            process_PointCloud(prt);
        }
        else
        {
            yError() << "Unknown datatype " << m_datatype;
            return false;
        }
    }

    return true;
}

bool  RerunLogger::process_RGBImage (const Bottle* pbot)
{
    yarp::sig::ImageOf<yarp::sig::PixelRgb> img;
    bool b_cp = yarp::os::Portable::copyPortable(*pbot, img);
    if (b_cp)
    {
        m_rec->log("camera", rerun::Image(
            img.getRawImage(),
            rerun::WidthHeight(img.width(), img.height()),
            rerun::datatypes::ColorModel::RGB
        ));
        return true;
    }
    return false;
}

bool  RerunLogger::process_Lidar2D (const Bottle* pbot)
{
    yarp::sig::LaserScan2D las;
    bool b_cp = yarp::os::Portable::copyPortable(*pbot, las);
    if (b_cp)
    {
        std::vector<rerun::Position2D> positions;
        for (size_t i=0; i<las.scans.size(); i++)
        {
            double step = (las.angle_max - las.angle_min) / double(las.scans.size());
            double angle = double(i)*step;
            double y = las.scans[i]*sin(angle*3.14159265359/180.0);
            double x = las.scans[i]*cos(angle*3.14159265359/180.0);
            positions.push_back(rerun::Position2D(x,y));
        }
        m_rec->log("laserscan",
            rerun::Points2D(positions));
        return true;
    }
    return false;
}

bool  RerunLogger::process_PointCloud (const Bottle* pbot)
{
    yarp::sig::PointCloud<yarp::sig::DataXYZ> pcl;
    bool b_cp = yarp::os::Portable::copyPortable(*pbot, pcl);
    if (b_cp)
    {
        std::vector<rerun::Position3D> positions;
        for (size_t x=0; x<pcl.width(); x++)
        {
            for (size_t y=0; y<pcl.height(); y++)
            {
                positions.push_back(rerun::Position3D(pcl(x,y).x,
                                                      pcl(x,y).y,
                                                      pcl(x,y).z));
            }
        }
        m_rec->log("pointcloud",
            rerun::Points3D(positions));
        return true;
    }
    return false;
}


bool RerunLogger::process_SensorStreamingData (const Bottle* pbot)
{
    yarp::dev::SensorStreamingData data;
    bool b_cp = yarp::os::Portable::copyPortable(*pbot, data);
    if (b_cp)
    {
        size_t sz = 0;
        sz=data.EncoderArrays.measurements.size();
        for (size_t i=0; i< sz; i++)
        {
            std::string name = "EncoderArray_" + std::to_string(i);
            for (size_t j  = 0; j < data.EncoderArrays.measurements[i].measurement.size(); j++)
                m_rec->log(name + "_" + std::to_string(j), rerun::Scalars(data.EncoderArrays.measurements[i].measurement[j]));
        }

        sz=data.LinearVelocitySensors.measurements.size();
        for (size_t i=0; i< sz; i++)
        {
            std::string name = "LinearVelocitySensors_" + std::to_string(i);
            m_rec->log(name + "_x", rerun::Scalars(data.LinearVelocitySensors.measurements[i].measurement[0]));
            m_rec->log(name + "_y", rerun::Scalars(data.LinearVelocitySensors.measurements[i].measurement[1]));
            m_rec->log(name + "_z", rerun::Scalars(data.LinearVelocitySensors.measurements[i].measurement[2]));
        }

        sz=data.OrientationSensors.measurements.size();
        for (size_t i=0; i< sz; i++)
        {
            std::string name = "OrientationSensors_" + std::to_string(i);
            m_rec->log(name + "_x", rerun::Scalars(data.OrientationSensors.measurements[i].measurement[0]));
            m_rec->log(name + "_y", rerun::Scalars(data.OrientationSensors.measurements[i].measurement[1]));
            m_rec->log(name + "_z", rerun::Scalars(data.OrientationSensors.measurements[i].measurement[2]));
        }

        sz=data.PositionSensors.measurements.size();
        for (size_t i=0; i< sz; i++)
        {
            std::string name = "PositionSensors_" + std::to_string(i);
            m_rec->log(name + "_x", rerun::Scalars(data.PositionSensors.measurements[i].measurement[0]));
            m_rec->log(name + "_y", rerun::Scalars(data.PositionSensors.measurements[i].measurement[1]));
            m_rec->log(name + "_z", rerun::Scalars(data.PositionSensors.measurements[i].measurement[2]));
        }

        sz=data.SixAxisForceTorqueSensors.measurements.size();
        for (size_t i=0; i< sz; i++)
        {
            std::string name = "SixAxisForceTorqueSensors_" + std::to_string(i);
            m_rec->log(name + "_Fx", rerun::Scalars(data.PositionSensors.measurements[i].measurement[0]));
            m_rec->log(name + "_Fy", rerun::Scalars(data.PositionSensors.measurements[i].measurement[1]));
            m_rec->log(name + "_Fz", rerun::Scalars(data.PositionSensors.measurements[i].measurement[2]));
            m_rec->log(name + "_Tx", rerun::Scalars(data.PositionSensors.measurements[i].measurement[3]));
            m_rec->log(name + "_Ty", rerun::Scalars(data.PositionSensors.measurements[i].measurement[4]));
            m_rec->log(name + "_Tz", rerun::Scalars(data.PositionSensors.measurements[i].measurement[5]));
        }

        sz=data.TemperatureSensors.measurements.size();
        for (size_t i=0; i< sz; i++)
        {
            std::string name = "TemperatureSensors_" + std::to_string(i);
            for (size_t j  = 0; j < data.TemperatureSensors.measurements[i].measurement.size(); j++)
                m_rec->log(name + "_" + std::to_string(j), rerun::Scalars(data.TemperatureSensors.measurements[i].measurement[j]));
        }

        sz=data.ThreeAxisGyroscopes.measurements.size();
        for (size_t i=0; i< sz; i++)
        {
            std::string name = "ThreeAxisGyroscopes_" + std::to_string(i);
            m_rec->log(name + "_x", rerun::Scalars(data.ThreeAxisGyroscopes.measurements[i].measurement[0]));
            m_rec->log(name + "_y", rerun::Scalars(data.ThreeAxisGyroscopes.measurements[i].measurement[1]));
            m_rec->log(name + "_z", rerun::Scalars(data.ThreeAxisGyroscopes.measurements[i].measurement[2]));
        }

        sz=data.ThreeAxisMagnetometers.measurements.size();
        for (size_t i=0; i< sz; i++)
        {
            std::string name = "ThreeAxisMagnetometers" + std::to_string(i);
            m_rec->log(name + "_x", rerun::Scalars(data.ThreeAxisMagnetometers.measurements[i].measurement[0]));
            m_rec->log(name + "_y", rerun::Scalars(data.ThreeAxisMagnetometers.measurements[i].measurement[1]));
            m_rec->log(name + "_z", rerun::Scalars(data.ThreeAxisMagnetometers.measurements[i].measurement[2]));
        }

        return true;
    }
    return false;
}

void RerunLogger::process_Bottle_nested (const Bottle* data, std::vector<size_t> path)
{
    if (data==nullptr) return;
    for (size_t i=0; i< data->size(); i++)
    {
        auto new_path = path;
        new_path.push_back(i);
        if (data->get(i).isList())
        {
            return process_Bottle_nested(data->get(i).asList(), new_path);
        }
        else if (data->get(i).isString())
        {
        }
        else
        {
            std::string vsname = "value";
            for (auto p : new_path) {vsname += "_" + std::to_string(p);}
            m_rec->log(vsname, rerun::Scalars(data->get(i).asFloat32()));
        }
    }
}

bool  RerunLogger::process_Bottle (const Bottle* pbot)
{
    yarp::os::Bottle data;
    bool bcp = yarp::os::Portable::copyPortable(*pbot, data);
    if (bcp)
    {
        process_Bottle_nested(&data, {});
        return true;
    }
    return false;
}

bool  RerunLogger::process_Vector (const Bottle* pbot)
{
    yarp::sig::Vector data;
    bool bcp = yarp::os::Portable::copyPortable(*pbot, data);
    if (bcp)
    {
        for (size_t i=0; i< data.size(); i++)
        {
            std::string vsname = "value_" + std::to_string(i);
            m_rec->log(vsname, rerun::Scalars(data[i]));
        }
        return true;
    }
    return false;
}

bool  RerunLogger::process_RobotState (const Bottle* pbot)
{
    yarp::dev::impl::jointData data;
    bool bcp = yarp::os::Portable::copyPortable(*pbot, data);
    if (bcp)
    {
        size_t n = data.controlMode.size();
        for (size_t i=0; i< n; i++)
        {
            std::string name = "joint_" + std::to_string(i);
            m_rec->log(name+"_Position", rerun::Scalars(data.jointPosition[i]));
            m_rec->log(name+"_Velocity", rerun::Scalars(data.jointVelocity[i]));
            m_rec->log(name+"_Acceleration", rerun::Scalars(data.jointAcceleration[i]));
            m_rec->log(name+"_torque", rerun::Scalars(data.torque[i]));
            m_rec->log(name+"_current", rerun::Scalars(data.current[i]));
            m_rec->log(name+"_pwmDutycycle", rerun::Scalars(data.pwmDutycycle[i]));
        }
        return true;
    }
    return false;
}

bool  RerunLogger::process_DepthImage (const Bottle* pbot)
{
    yarp::sig::ImageOf<yarp::sig::PixelFloat> img;
    bool b_cp = yarp::os::Portable::copyPortable(*pbot, img);
    if (b_cp)
    {
        m_rec->log("camera", rerun::DepthImage(
            reinterpret_cast<const float*>(img.getRawImage()),
            rerun::WidthHeight(img.width(), img.height())
        ));
        return true;
    }
    return false;
}

int main(int argc, char *argv[])
{
    yarp::os::Network yarp;

    // Read input parameters
    yarp::os::ResourceFinder rf;
    rf.configure(argc,argv);

    RerunLogger logger;
    if (logger.configure(rf)) {
        logger.runModule();
    }

    return 0;
}
