/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "dc1394slider.h"
#include "dc1394sliderwb.h"
#include <iostream>

static QStringList video_mode_labels;
static QStringList video_rate_labels;
static QStringList color_coding_labels;
static QStringList iso_speed_labels;
static QStringList op_mode_labels;

using namespace yarp::dev;

MainWindow::MainWindow(std::string loc, std::string rem, QWidget* parent) :
    QMainWindow(parent),
    loadingWidget(this),
    dc1394Thread(loc,rem),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initMainWindow();
    initFeatureTab();
    initFeatureAdvTab();
    dc1394Thread.doTask(_initFormatTab);

    dc1394Thread.doTask(_init);
    dc1394Thread.doTask(_reload);

    connect(ui->tabWidget,SIGNAL(currentChanged(int)),
            this,SLOT(onTabClicked(int)));

}

MainWindow::~MainWindow()
{
    disconnectWidgets();
    dc1394Thread.stop();

    delete ui;
}

void MainWindow::onTabClicked(int index)
{
    if(index == 0 || index == 1){
        for(int i=0;i<m_pSli.count();i++){
            ((DC1394SliderBase*)m_pSli.at(i))->updateSliders();
        }
    }
}

void MainWindow::initMainWindow()
{
    setWindowTitle("Grabber Remote GUI");

    video_mode_labels <<
        "160x120 YUV444" <<
        "320x240 YUV422" <<
        "640x480 YUV411" <<
        "640x480 YUV422" <<
        "640x480 RGB8" <<
        "640x480 MONO8" <<
        "640x480 MONO16" <<
        "800x600 YUV422" <<
        "800x600 RGB8" <<
        "800x600_MONO8" <<
        "1024x768 YUV422" <<
        "1024x768 RGB8" <<
        "1024x768 MONO8" <<
        "800x600 MONO16" <<
        "1024x768 MONO16" <<
        "1280x960 YUV422" <<
        "1280x960 RGB8" <<
        "1280x960_MONO8" <<
        "1600x1200 YUV422" <<
        "1600x1200 RGB8" <<
        "1600x1200 MONO8" <<
        "1280x960 MONO16" <<
        "1600x1200_MONO16" <<
        "EXIF" <<
        "FORMAT7 0" <<
        "FORMAT7 1" <<
        "FORMAT7 2" <<
        "FORMAT7 3" <<
        "FORMAT7 4" <<
        "FORMAT7 5" <<
        "FORMAT7 6" <<
        "FORMAT7 7";

    video_rate_labels << "1.875 fps" << "3.75 fps" << "7.5 fps" << "15 fps" << "30 fps" << "60 fps" << "120 fps" <<"240 fps";
    color_coding_labels << "MONO8" << "YUV411" << "YUV422" << "YUV444" << "RGB8" << "MONO16" << "RGB16" << "MONO16S" << "RGB16S" << "RAW8" << "RAW16";
    iso_speed_labels << "100 Mbps" << "200 Mbps" << "400 Mbps" << "800 Mbps" << "1600 Mbps" << "3200 Mbps";
    op_mode_labels << "LEGACY" << "1394b";

    connect(ui->btnRefresh1,SIGNAL(clicked()),this,SLOT(onReloadClicked()));
    connect(ui->btnRefresh2,SIGNAL(clicked()),this,SLOT(onReloadClicked()));


    connect(&dc1394Thread,SIGNAL(initFormatTabDone(uint,uint,uint)),
            this,SLOT(initFormatTab(uint,uint,uint)),Qt::QueuedConnection);
    connect(&dc1394Thread,SIGNAL(initDone(uint,uint,uint,bool,uint,QSize,QSize,QSize,QSize,QSize,uint,bool)),
            this,SLOT(Init(uint,uint,uint,bool,uint,QSize,QSize,QSize,QSize,QSize,uint,bool)),Qt::QueuedConnection);
    connect(&dc1394Thread,SIGNAL(reloadDone(uint,uint,QSize,QSize,QSize,QSize,QSize,uint,uint,uint,uint,uint)),
            this,SLOT(Reload(uint,uint,QSize,QSize,QSize,QSize,QSize,uint,uint,uint,uint,uint)),Qt::QueuedConnection);
    connect(&dc1394Thread,SIGNAL(loadDefaultDone(uint,uint,bool)),
            this,SLOT(loadDefault(uint,uint,bool)),Qt::QueuedConnection);
    connect(&dc1394Thread,SIGNAL(resetDone(uint,uint,bool)),
            this,SLOT(reset(uint,uint,bool)),Qt::QueuedConnection);
    connect(&dc1394Thread,SIGNAL(setTransmissionDC1394Done()),
            this,SLOT(onTransmissionOnoffDone()),Qt::QueuedConnection);
    connect(&dc1394Thread,SIGNAL(setPowerDC1394Done()),
            this,SLOT(onPowerOnoffDone()),Qt::QueuedConnection);
    connect(&dc1394Thread,SIGNAL(setFormat7WindowDC1394Done()),
            this,SLOT(onFormat7WindowDone()),Qt::QueuedConnection);
    connect(&dc1394Thread,SIGNAL(setVideoModeDC1394Done()),
            this,SLOT(onVideoFormatCurrentDone()),Qt::QueuedConnection);
    connect(&dc1394Thread,SIGNAL(setColorCodingDC1394Done()),
            this,SLOT(onColorCodingDone()),Qt::QueuedConnection);
    connect(&dc1394Thread,SIGNAL(setFPSDC1394Done()),
            this,SLOT(onFramerateDone()),Qt::QueuedConnection);
    connect(&dc1394Thread,SIGNAL(setISOSpeedDC1394Done()),
            this,SLOT(onISOSpeedDone()),Qt::QueuedConnection);
    connect(&dc1394Thread,SIGNAL(setBytesPerPacketDC1394Done()),
            this,SLOT(onSizeByteDone()),Qt::QueuedConnection);
    connect(&dc1394Thread,SIGNAL(setOperationModeDC1394Done()),
            this,SLOT(onOpModeDone()),Qt::QueuedConnection);

    connect(&dc1394Thread,SIGNAL(startLoading()),
            this,SLOT(onStartLoading()),Qt::QueuedConnection);
    connect(&dc1394Thread,SIGNAL(stopLoading()),
            this,SLOT(onStopLoading()),Qt::QueuedConnection);

    dc1394Thread.start();

    m_pSli.append(ui->sliderSharpness);
    m_pSli.append(ui->sliderHue);
    m_pSli.append(ui->sliderSaturation);
    m_pSli.append(ui->sliderGamma);
    m_pSli.append(ui->sliderIris);
    m_pSli.append(ui->sliderFocus);
    m_pSli.append(ui->sliderShutter);
    m_pSli.append(ui->sliderBrightness);
    m_pSli.append(ui->sliderGain);
    m_pSli.append(ui->sliderExposure);
    m_pSli.append(ui->sliderWB);

    for(int i=0;i<m_pSli.count();i++){
        connect(((DC1394SliderBase*)m_pSli.at(i)),SIGNAL(featureDisabled(QObject*)),
                this,SLOT(onSliderDisabled(QObject *)),Qt::QueuedConnection);
    }
}

void MainWindow::onReloadClicked()
{
    dc1394Thread.doTask(_reload);
}

void MainWindow::connectWidgets()
{
    connect(ui->m_MenuMode,SIGNAL(currentIndexChanged(int)),this,SLOT(onVideoFormatCurrentIndexChanged(int)));
    connect(ui->m_MenuColorCoding,SIGNAL(currentIndexChanged(int)),this,SLOT(onColorCodingCurrentIndexChanged(int)));
    connect(ui->m_MenuFPS,SIGNAL(currentIndexChanged(int)),this,SLOT(onFramerateCurrentIndexChanged(int)));
    connect(ui->m_MenuISO,SIGNAL(currentIndexChanged(int)),this,SLOT(onISOSpeedCurrentIndexChanged(int)));
    connect(ui->m_MenuOpMode,SIGNAL(currentIndexChanged(int)),this,SLOT(onOpModeCurrentIndexChanged(int)));
    connect(ui->m_bpp,SIGNAL(valueChanged(int)),this,SLOT(onSizeByteValueChanged(int)));
    connect(ui->m_xdim,SIGNAL(valueChanged(int)),this,SLOT(onFormat7WindowChange(int)));
    connect(ui->m_ydim,SIGNAL(valueChanged(int)),this,SLOT(onFormat7WindowChange(int)));
    connect(ui->m_xoff,SIGNAL(valueChanged(int)),this,SLOT(onFormat7WindowChange(int)));
    connect(ui->m_yoff,SIGNAL(valueChanged(int)),this,SLOT(onFormat7WindowChange(int)));
    connect(ui->m_power,SIGNAL(toggled(bool)),this,SLOT(onPowerOnoffChange(bool)));
    connect(ui->m_transmission,SIGNAL(toggled(bool)),this,SLOT(onTransmissionOnoffChange(bool)));
    connect(ui->m_reset,SIGNAL(clicked()),this,SLOT(onResetChange()));
    connect(ui->m_defaults,SIGNAL(clicked()),this,SLOT(onLoadDefaultsChange()));

}

void MainWindow::disconnectWidgets()
{
    disconnect(ui->m_MenuMode,SIGNAL(currentIndexChanged(int)),this,SLOT(onVideoFormatCurrentIndexChanged(int)));
    disconnect(ui->m_MenuColorCoding,SIGNAL(currentIndexChanged(int)),this,SLOT(onColorCodingCurrentIndexChanged(int)));
    disconnect(ui->m_MenuFPS,SIGNAL(currentIndexChanged(int)),this,SLOT(onFramerateCurrentIndexChanged(int)));
    disconnect(ui->m_MenuISO,SIGNAL(currentIndexChanged(int)),this,SLOT(onISOSpeedCurrentIndexChanged(int)));
    disconnect(ui->m_MenuOpMode,SIGNAL(currentIndexChanged(int)),this,SLOT(onOpModeCurrentIndexChanged(int)));
    disconnect(ui->m_bpp,SIGNAL(valueChanged(int)),this,SLOT(onSizeByteValueChanged(int)));
    disconnect(ui->m_xdim,SIGNAL(valueChanged(int)),this,SLOT(onFormat7WindowChange(int)));
    disconnect(ui->m_ydim,SIGNAL(valueChanged(int)),this,SLOT(onFormat7WindowChange(int)));
    disconnect(ui->m_xoff,SIGNAL(valueChanged(int)),this,SLOT(onFormat7WindowChange(int)));
    disconnect(ui->m_yoff,SIGNAL(valueChanged(int)),this,SLOT(onFormat7WindowChange(int)));
    disconnect(ui->m_power,SIGNAL(toggled(bool)),this,SLOT(onPowerOnoffChange(bool)));
    disconnect(ui->m_transmission,SIGNAL(toggled(bool)),this,SLOT(onTransmissionOnoffChange(bool)));
    disconnect(ui->m_reset,SIGNAL(clicked()),this,SLOT(onResetChange()));
    disconnect(ui->m_defaults,SIGNAL(clicked()),this,SLOT(onLoadDefaultsChange()));

}

void MainWindow::initFeatureAdvTab()
{
    ui->sliderSharpness->init(cameraFeature_id_t::YARP_FEATURE_SHARPNESS, (char*)"Sharpness", &dc1394Thread);
    ui->sliderHue->init(cameraFeature_id_t::YARP_FEATURE_HUE, (char*)"Hue", &dc1394Thread);
    ui->sliderSaturation->init(cameraFeature_id_t::YARP_FEATURE_SATURATION, (char*)"Saturation", &dc1394Thread);
    ui->sliderGamma->init(cameraFeature_id_t::YARP_FEATURE_GAMMA, (char*)"Gamma", &dc1394Thread);
    ui->sliderIris->init(cameraFeature_id_t::YARP_FEATURE_IRIS, (char*)"Iris", &dc1394Thread);
    ui->sliderFocus->init(cameraFeature_id_t::YARP_FEATURE_FOCUS, (char*)"Focus", &dc1394Thread);
}

void MainWindow::initFeatureTab()
{
    ui->sliderShutter->init(cameraFeature_id_t::YARP_FEATURE_SHUTTER, (char*)"Shutter", &dc1394Thread);
    ui->sliderBrightness->init(cameraFeature_id_t::YARP_FEATURE_BRIGHTNESS, (char*)"Brightness", &dc1394Thread);
    ui->sliderGain->init(cameraFeature_id_t::YARP_FEATURE_GAIN, (char*)"Gain", &dc1394Thread);
    ui->sliderExposure->init(cameraFeature_id_t::YARP_FEATURE_EXPOSURE, (char*)"Exposure", &dc1394Thread);
    ui->sliderWB->init(&dc1394Thread);
}

void MainWindow::onSliderDisabled(QObject *slider)
{
    sliderInitMutex.lock();

    QWidget *s = (QWidget*)slider;
    QWidget *p = (QWidget*)s->parent();
    p->layout()->removeWidget(s);
    m_pSli.removeOne(s);

    sliderInitMutex.unlock();
}

void MainWindow::initFormatTab(unsigned int videoModeMaskDC1394,
                               unsigned int fPSMaskDC1394,
                               unsigned int colorCodingMaskDC1394)
{

    //disconnectWidgets();
    unsigned int mask=videoModeMaskDC1394;

    for (int i=0,e=0; i<32; ++i){
        if (((1<<i) & mask) && (i<video_mode_labels.length())){
            m_VideoModeLut[e]=i;
            ui->m_MenuMode->insertItem(e++,video_mode_labels.at(i));
        }
    }

    mask=fPSMaskDC1394;
    for (int i=0,e=0; i<8; ++i){
        if (((1<<i) & mask) && (i<video_rate_labels.length())){
            m_FPSLut[e]=i;
            ui->m_MenuFPS->insertItem(e++,video_rate_labels.at(i));
        }
    }

    ui->m_MenuISO->insertItem(0,iso_speed_labels.at(0));
    ui->m_MenuISO->insertItem(1,iso_speed_labels.at(1));
    ui->m_MenuISO->insertItem(2,iso_speed_labels.at(2));


    mask=colorCodingMaskDC1394;
    for (int i=0,e=0; i<32; ++i){
        if (((1<<i) & mask) && (i<color_coding_labels.length())){
            m_ColorCodingLut[e]=i;
            ui->m_MenuColorCoding->insertItem(e++,color_coding_labels.at(i));
        }
    }

    //connectWidgets();
}





void MainWindow::initCameraTab()
{
    //disconnectWidgets();
    ui->m_MenuOpMode->insertItem(0,op_mode_labels.at(0));
    ui->m_MenuOpMode->insertItem(1,op_mode_labels.at(1));

    ui->m_power->setChecked(true);

    //connectWidgets();
}


void MainWindow::Init(uint videoModeDC1394, uint fPSDC1394, uint iSOSpeedDC1394, bool operationModeDC1394,
                      uint colorCodingDC1394, QSize max, QSize step, QSize offset, QSize dim, QSize pos,
                      uint bytesPerPacketDC1394, bool transmissionDC1394)
{
    yDebug("Init\n");
    QString sst;

    //for (int n=0; n<m_nFeatures; ++n) m_pSli[n]->Refresh();
    disconnectWidgets();

    sst=video_mode_labels.at(videoModeDC1394);
    ui->m_MenuMode->setCurrentText(sst);

    bool bFormat7=videoModeDC1394>=24;

    ui->m_MenuColorCoding->setEnabled(bFormat7);
    ui->m_xdim->setEnabled(bFormat7);
    ui->m_ydim->setEnabled(bFormat7);
    ui->m_xoff->setEnabled(bFormat7);
    ui->m_yoff->setEnabled(bFormat7);

    ui->m_MenuFPS->setEnabled(!bFormat7);
    if (!bFormat7){
        sst = video_rate_labels.at(fPSDC1394);
        ui->m_MenuFPS->setCurrentText(sst);
    }

    sst = iso_speed_labels.at(iSOSpeedDC1394);
    ui->m_MenuISO->setCurrentText(sst);
    sst = operationModeDC1394 ? op_mode_labels.at(1) : op_mode_labels.at(0);
    ui->m_MenuOpMode->setCurrentText(sst);
    sst = color_coding_labels.at(colorCodingDC1394);
    ui->m_MenuColorCoding->setCurrentText(sst);

    unsigned int xmax,ymax,xstep,ystep,xoffstep,yoffstep;
    xmax = max.width();
    ymax = max.height();
    xstep = step.width();
    ystep = step.height();
    xoffstep = offset.width();
    yoffstep = offset.height();

    unsigned int xdim,ydim;
    int x0,y0;
    xdim = dim.width();
    ydim = dim.height();
    x0 = pos.width();
    y0 = pos.height();


    if (xstep<2){
        xstep=2;
    }
    if (ystep<2){
        ystep=2;
    }

    ui->m_xdim->setRange(0,xmax);
    ui->m_ydim->setRange(0,ymax);
    ui->m_xdim->setSingleStep(xstep);
    ui->m_ydim->setSingleStep(ystep);
    ui->m_xdim->setValue(xdim);
    ui->m_ydim->setValue(ydim);

    int xoffMax=(xmax-xdim)/2;
    int yoffMax=(ymax-ydim)/2;


    ui->m_xoff->setRange(-xoffMax,xoffMax);
    ui->m_yoff->setRange(-yoffMax,yoffMax);
    ui->m_xoff->setSingleStep(xoffstep);
    ui->m_yoff->setSingleStep(yoffstep);
    ui->m_xoff->setValue(x0);
    ui->m_yoff->setValue(y0);

    ui->labelMaxWidth->setText(QString("%1").arg(xmax));
    ui->labelMaxHeight->setText(QString("%1").arg(ymax));

    ui->m_bpp->setValue(bytesPerPacketDC1394);
    ui->m_bpp->setEnabled(bFormat7);
    ui->m_transmission->setChecked(transmissionDC1394);


    yarp::dev::CameraDescriptor camera;
    if(dc1394Thread.getCameraDescription(camera))
    {
        std::cout << "BUS type is " << (int)camera.busType << "; description is " << camera.deviceDescription << std::endl;

        if(camera.busType != yarp::dev::BusType::BUS_FIREWIRE)
            ui->tabWidget->removeTab(ui->tabWidget->indexOf(ui->tabCamera));
        else
            initCameraTab();
    }
    else
        yDebug("Remote camera device has no IFrameGrabberControl2 interface\n");

    connectWidgets();
}

void MainWindow::Reload(uint videoModeDC1394, uint colorCodingMaskDC1394,
                        QSize max, QSize step, QSize offset, QSize dim, QSize pos,
                        uint colorCodingDC1394, uint bytesPerPacketDC1394,uint fPSMaskDC1394,
                        uint fPSDC1394,uint iSOSpeedDC1394)
{
    disconnectWidgets();

    unsigned int video_mode=videoModeDC1394;
    ui->m_MenuMode->setCurrentText(video_mode_labels.at(video_mode));

    ui->m_MenuColorCoding->clear();

    unsigned int mask=colorCodingMaskDC1394;

    for (int i=0,e=0; i<32; ++i){
        if (((1<<i) & mask) && i<color_coding_labels.length()){
            m_ColorCodingLut[e]=i;
            ui->m_MenuColorCoding->insertItem(e++,color_coding_labels.at(i));
        }
    }

    unsigned int xmax,ymax,xstep,ystep,xoffstep,yoffstep;
    xmax = max.width();
    ymax = max.height();
    xstep = step.width();
    ystep = step.height();
    xoffstep = offset.width();
    yoffstep = offset.height();

    unsigned int xdim,ydim;
    int x0,y0;
    xdim = dim.width();
    ydim = dim.height();
    x0 = pos.width();
    y0 = pos.height();

    if (xstep<2){
        xstep=2;
    }
    if (ystep<2){
        ystep=2;
    }


    ui->m_xdim->setRange(0,xmax);
    ui->m_ydim->setRange(0,ymax);
    ui->m_xdim->setSingleStep(xstep);
    ui->m_ydim->setSingleStep(ystep);
    ui->m_xdim->setValue(xdim);
    ui->m_ydim->setValue(ydim);

    int xoffMax=(xmax-xdim)/2;
    int yoffMax=(ymax-ydim)/2;

    ui->m_xoff->setRange(-xoffMax,xoffMax);
    ui->m_yoff->setRange(-yoffMax,yoffMax);
    ui->m_xoff->setSingleStep(xoffstep);
    ui->m_yoff->setSingleStep(yoffstep);
    ui->m_xoff->setValue(x0);
    ui->m_yoff->setValue(y0);

    ui->labelMaxWidth->setText(QString("%1").arg(xmax));
    ui->labelMaxHeight->setText(QString("%1").arg(ymax));


    // FORMAT 7
    if (mask) {
        ui->m_MenuColorCoding->setEnabled(true);
        ui->m_MenuColorCoding->setCurrentText(color_coding_labels.at(colorCodingDC1394));

        //ui->m_MenuFPS->setEnabled(false);

        ui->m_bpp->setEnabled(true);

        ui->m_bpp->setValue(bytesPerPacketDC1394);

        ui->m_xdim->setEnabled(true);
        ui->m_ydim->setEnabled(true);
        ui->m_xoff->setEnabled(true);
        ui->m_yoff->setEnabled(true);

    } else {

        ui->m_MenuFPS->clear();

        mask=fPSMaskDC1394;

        for (int i=0,e=0; i<8; ++i){
            if (((1<<i) & mask) && i<video_rate_labels.length()){
                m_FPSLut[e]=i;
                ui->m_MenuFPS->insertItem(e++,video_rate_labels.at(i));
            }
        }

        ui->m_MenuFPS->setCurrentText(video_rate_labels.at(fPSDC1394));

        ui->m_MenuColorCoding->setEnabled(false);
        ui->m_bpp->setEnabled(false);
    }

    ui->m_MenuISO->setCurrentText(iso_speed_labels.at(iSOSpeedDC1394));

    for (int n=0; n<m_pSli.count(); n++){
        DC1394SliderBase *slider = ((DC1394SliderBase*)m_pSli.at(n));
        if(slider->getSliderType() == SLIDER){
            ((DC1394Slider*)slider)->Refresh();
        }else{
            ((DC1394SliderWB*)slider)->Refresh();
        }
    }

    connectWidgets();
}




/*******************************************************/
void MainWindow::onResetChange()
{
    yDebug("on_reset_change()\n");
    dc1394Thread.doTask(_reset);
}

void MainWindow::reset(uint bytesPerPacketDC1394,uint colorCodingMaskDC1394,bool transmissionDC1394)
{
    ui->m_bpp->setValue(bytesPerPacketDC1394);
    ui->m_bpp->setEnabled(colorCodingMaskDC1394);
    ui->m_transmission->setChecked(transmissionDC1394);
}

void MainWindow::onLoadDefaultsChange()
{
    yDebug("on_load_defaults_change()\n");
    QVariantList list;
    dc1394Thread.doTask(_loadDefault);
}

void MainWindow::loadDefault(uint bytesPerPacketDC1394,uint colorCodingMaskDC1394,bool transmissionDC1394)
{

    ui->m_bpp->setValue(bytesPerPacketDC1394);
    ui->m_bpp->setEnabled(colorCodingMaskDC1394);
    ui->m_transmission->setChecked(transmissionDC1394);
}

void MainWindow::onTransmissionOnoffChange(bool value)
{
    yDebug("on_transmission_onoff_change()\n");
    QVariantList list;
    list.append(QVariant(value));
    dc1394Thread.doTask(_setTransmissionDC1394,list);
}

void MainWindow::onTransmissionOnoffDone()
{

}

void MainWindow::onPowerOnoffChange(bool value)
{
    yDebug("on_power_onoff_change()\n");
    QVariantList list;
    list.append(QVariant(value));
    dc1394Thread.doTask(_setPowerDC1394,list);
}

void MainWindow::onPowerOnoffDone()
{

}

void MainWindow::onFormat7WindowChange(int value)
{
    Q_UNUSED(value);

    if (!ui->m_xdim->isEnabled() || !ui->m_ydim->isEnabled() || !ui->m_xoff->isEnabled() || !ui->m_yoff->isEnabled()){
        return;
    }

    yDebug("on_format7_window_change()\n");

    unsigned int xdim=(unsigned)ui->m_xdim->value();
    unsigned int ydim=(unsigned)ui->m_ydim->value();
    int x0=(unsigned)ui->m_xoff->value();
    int y0=(unsigned)ui->m_yoff->value();

    QVariantList list;
    list.append(QVariant(xdim));
    list.append(QVariant(ydim));
    list.append(QVariant(x0));
    list.append(QVariant(y0));
    dc1394Thread.doTask(_setFormat7WindowDC1394,list);
}

void MainWindow::onFormat7WindowDone()
{

}

void MainWindow::onVideoFormatCurrentIndexChanged(int value)
{
    yDebug("on_mode_change()\n");
    unsigned int video_mode=m_VideoModeLut[value];
    QVariantList list;
    list.append(video_mode);

    dc1394Thread.doTask(_setVideoModeDC1394,list);
}

void MainWindow::onVideoFormatCurrentDone()
{
    dc1394Thread.doTask(_reload);
}

void MainWindow::onColorCodingCurrentIndexChanged(int value)
{
    if (!ui->m_MenuColorCoding->isEnabled()){
        return;
    }

    if (value<0){
        return;
    }

    yDebug("on_color_coding_change()\n");

    QVariantList list;
    list.append(m_ColorCodingLut[value]);

    dc1394Thread.doTask(_setColorCodingDC1394,list);


}

void MainWindow::onColorCodingDone()
{
    for (int n=0; n<m_pSli.count(); n++){
        DC1394SliderBase *slider = ((DC1394SliderBase*)m_pSli.at(n));
        if(slider->getSliderType() == SLIDER){
            ((DC1394Slider*)slider)->Refresh();
        }else{
            ((DC1394SliderWB*)slider)->Refresh();
        }
    }
}

void MainWindow::onFramerateCurrentIndexChanged(int value)
{
    if(!ui->m_MenuFPS->isEnabled()){
        return;
    }

    if (value<0){
        return;
    }

    yDebug("on_framerate_change()\n");

    QVariantList list;
    list.append(m_FPSLut[value]);

    dc1394Thread.doTask(_setFPSDC1394,list);
}

void MainWindow::onFramerateDone()
{

}

void MainWindow::onISOSpeedCurrentIndexChanged(int value)
{
    yDebug("on_iso_speed_change()\n");
    QVariantList list;
    list.append(value);

    dc1394Thread.doTask(_setISOSpeedDC1394,list);

}

void MainWindow::onISOSpeedDone()
{
    dc1394Thread.doTask(_reload);
}

void MainWindow::onSizeByteValueChanged(int value)
{
    if (!ui->m_bpp->isEnabled()){
        return;
    }

    yDebug("on_bpp_change()\n");
    QVariantList list;
    list.append((unsigned int)value);

    dc1394Thread.doTask(_setBytesPerPacketDC1394,list);
}

void MainWindow::onSizeByteDone()
{

}

void MainWindow::onOpModeCurrentIndexChanged(int value)
{
    yDebug("on_operation_mode_change()\n");
    QVariantList list;
    list.append(value);

    dc1394Thread.doTask(_setOperationModeDC1394,list);
}
void MainWindow::onOpModeDone()
{

}


void MainWindow::onStartLoading()
{
    loadingWidget.start();
}

void MainWindow::onStopLoading()
{
    loadingWidget.accept();
    loadingWidget.stop();
}
