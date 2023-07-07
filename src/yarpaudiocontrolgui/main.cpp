/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QThread>
#include <QVBoxLayout>

#include <yarp/os/Os.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Network.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Property.h>
#include <yarp/os/Thread.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/AudioGrabberInterfaces.h>

#ifdef _WIN32
#include <Windows.h>
#endif

#include <iostream>

class CustomWidget : public QWidget, public yarp::os::Thread
{
    Q_OBJECT
private:
    QPushButton* startButton = nullptr;
    QPushButton* stopButton = nullptr;
    QVBoxLayout* vlayout = nullptr;
    QHBoxLayout* hlayout = nullptr;
    QLabel* label = nullptr;

public:
    virtual ~CustomWidget ()
    {
        this->stop();
        hlayout->removeWidget(startButton);
        hlayout->removeWidget(stopButton);
        if (startButton) { delete startButton; startButton = nullptr;}
        if (stopButton) { delete stopButton; stopButton = nullptr;}
        vlayout->removeWidget(label);
        if (label) { delete label; label = nullptr; }
        vlayout->removeItem(hlayout);
        if (vlayout) { delete vlayout; vlayout = nullptr;}
        if (hlayout) {delete hlayout; hlayout = nullptr;}
    }

    CustomWidget(QWidget *parent = nullptr) : QWidget(parent)
    {
        startButton = new QPushButton("Start", this);
        stopButton = new QPushButton("Stop", this);
        vlayout = new QVBoxLayout;
        hlayout = new QHBoxLayout;
        label = new QLabel("Device status: - - -");
        hlayout->addWidget(startButton);
        hlayout->addWidget(stopButton);
        vlayout->addLayout(hlayout);
        vlayout->addWidget(label);
        setLayout(vlayout);

        connect(startButton, &QPushButton::clicked, this, &CustomWidget::startFunction);
        connect(stopButton, &QPushButton::clicked, this, &CustomWidget::stopFunction);
    }

    void run() override
    {
        while(!isStopping())
        {
            if (m_iAudioRec)
            {
                bool enabled;
                m_iAudioRec->isRecording(enabled);
                if (enabled) label->setText("Device status : recording");
                else label->setText("Device status : idle");
            }
            if (m_iAudioPlay)
            {
                bool enabled;
                m_iAudioPlay->isPlaying(enabled);
                if (enabled) label->setText("Device status : playing");
                else label->setText("Device status : idle");
            }
            yarp::os::Time::delay(1);
        }
    }

public slots:
    void startFunction()
    {
        if (m_iAudioRec)  {  m_iAudioRec->startRecording();  }
        if (m_iAudioPlay) {  m_iAudioPlay->startPlayback();  }
    }

    void stopFunction()
    {
        if (m_iAudioRec)  {  m_iAudioRec->stopRecording();  }
        if (m_iAudioPlay) {  m_iAudioPlay->stopPlayback();  }
    }

public:
    void setInterface(yarp::dev::IAudioGrabberSound* audio_rec_interface, yarp::dev::IAudioRender* audio_play_interface)
    {
        m_iAudioRec = audio_rec_interface;
        m_iAudioPlay = audio_play_interface;
        this->start();
    }

private:
    yarp::dev::IAudioGrabberSound* m_iAudioRec = nullptr;
    yarp::dev::IAudioRender* m_iAudioPlay = nullptr;
};

#define ERROR_RETURN_CODE 0
int main(int argc, char *argv[])
{
    //console output on windows
    #ifdef _WIN32
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    #endif

    // Yarp initialization
    yarp::os::Network yarp(yarp::os::YARP_CLOCK_SYSTEM);
    if (!yarp.checkNetwork())
    {
        yError("Error initializing yarp network (is yarpserver running?)");
        return ERROR_RETURN_CODE;
    }
    yarp::os::ResourceFinder rf;
    rf.configure(argc, argv);

    //check parameters
    if (rf.check("help"))
    {
        yInfo() << "yarpaudiocontrolgui accepts the following options:";
        yInfo() << "--local <portname> an optional parameter which defines the name of the local port. Default is /yarpaudiocontrolgui. Multiple yarpaudiocontrolgui must use different local names.";
        yInfo() << "--remote-rec  <portname> an optional parameter which defines the name of the rpc port of a audioRecorder_nws_yarp device.";
        yInfo() << "--remote-play <portname> an optional parameter which defines the name of the rpc port of a audioPlayer_nws_yarp device.";
        yInfo() << "NOTE: remote-rec and remote-play options are mutually exclusive.";
        return 0;
    }

    std::string local = rf.find("local").asString();
    std::string remote_rec = rf.find("remote-rec").asString();
    std::string remote_play = rf.find("remote-play").asString();
    if (local.empty())   { local = "/yarpaudiocontrolgui"; }
    if (remote_play.empty() && remote_rec.empty())
    {
        yError() << "Please choose at least a recorder or a player device. Use --help for list of options.";
        return ERROR_RETURN_CODE;
    }
    if (!remote_play.empty() && !remote_rec.empty())
    {
        yError() << "Please choose either a recorder or a player device, not both. remote-rec and remote-play options are mutually exclusive.";
        return ERROR_RETURN_CODE;
    }

    //yarp audio client instance
    yarp::dev::IAudioGrabberSound* iAudioRec = nullptr;
    yarp::dev::IAudioRender*       iAudioPlay = nullptr;
    yarp::dev::PolyDriver          ddPlay;
    yarp::dev::PolyDriver          ddRec;

    if (!remote_play.empty())
    {
        yarp::os::Property p_cfgPlay;
        p_cfgPlay.put("device", "audioPlayer_nwc_yarp");
        p_cfgPlay.put("remote", remote_play);
        p_cfgPlay.put("local", local);
        ddPlay.open(p_cfgPlay);
        ddPlay.view(iAudioPlay);
        if (iAudioPlay == nullptr)
        {
            yError()<<"Failed to open audioPlayerInterface";
            return ERROR_RETURN_CODE;
        }
    }
    if (!remote_rec.empty())
    {
        yarp::os::Property p_cfgRec;
        p_cfgRec.put("device", "audioRecorder_nwc_yarp");
        p_cfgRec.put("remote", remote_rec);
        p_cfgRec.put("local", local);
        ddRec.open(p_cfgRec);
        ddRec.view(iAudioRec);
        if (iAudioRec == nullptr)
        {
            yError() << "Failed to open audioRecorderInterface";
            return ERROR_RETURN_CODE;
        }
    }

    //qt initialization
    QApplication app(argc, argv);
    CustomWidget* widget = new CustomWidget;
    widget->setInterface(iAudioRec, iAudioPlay);
    widget->setMinimumWidth(400);
    widget->show();
    int r = app.exec();

    //clean up
    if (widget) {delete widget; widget = nullptr;}
    ddRec.close();
    ddPlay.close();

    yInfo() << "yarpaudiocontrolgui says goodbye!";
    return r;
}

#include "main.moc"
