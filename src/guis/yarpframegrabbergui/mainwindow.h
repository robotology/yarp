/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "loadingwidget.h"
#include "dc1394thread.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    explicit MainWindow(std::string loc, std::string rem, QWidget* parent = 0);
    ~MainWindow();


    void initFeatureTab();
    void initFeatureAdvTab();

    void initCameraTab();

private:
    void initMainWindow();
    void connectWidgets();
    void disconnectWidgets();

private:
    Ui::MainWindow *ui;
    unsigned int m_VideoModeLut[32];
    unsigned int m_ColorCodingLut[32];
    unsigned int m_FPSLut[8];
    QList <QWidget*>m_pSli;


    LoadingWidget loadingWidget;
    DC1394Thread dc1394Thread;      //  --> dinamico o può rimanere lo stesso?
    QMutex sliderInitMutex;


signals:
    void internalFormatTab(unsigned int videoModeMaskDC1394,
                           unsigned int fPSMaskDC1394,
                           unsigned int colorCodingMaskDC1394);
private slots:
    void initFormatTab(unsigned int videoModeMaskDC1394, unsigned int fPSMaskDC1394, unsigned int colorCodingMaskDC1394);
    void Init(uint, uint fPSDC1394, uint iSOSpeedDC1394,
                      bool operationModeDC1394, uint colorCodingDC1394, QSize max, QSize step, QSize offset,
                      QSize dim, QSize pos, uint bytesPerPacketDC1394, bool transmissionDC1394);

    void Reload(uint, uint, QSize max, QSize step, QSize offset, QSize dim, QSize pos, uint colorCodingDC1394, uint bytesPerPacketDC1394, uint fPSMaskDC1394, uint fPSDC1394, uint iSOSpeedDC1394);
    void loadDefault(uint bytesPerPacketDC1394,uint colorCodingMaskDC1394,bool transmissionDC1394);
    void reset(uint bytesPerPacketDC1394,uint colorCodingMaskDC1394,bool transmissionDC1394);
    void onTransmissionOnoffDone();
    void onPowerOnoffDone();
    void onFormat7WindowDone();
    void onVideoFormatCurrentDone();
    void onColorCodingDone();
    void onFramerateDone();
    void onISOSpeedDone();
    void onSizeByteDone();
    void onOpModeDone();

    void onVideoFormatCurrentIndexChanged(int);
    void onColorCodingCurrentIndexChanged(int);
    void onFramerateCurrentIndexChanged(int);
    void onISOSpeedCurrentIndexChanged(int);
    void onOpModeCurrentIndexChanged(int);
    void onSizeByteValueChanged(int);
    void onFormat7WindowChange(int);
    void onPowerOnoffChange(bool value);
    void onTransmissionOnoffChange(bool value);
    void onLoadDefaultsChange();
    void onResetChange();

    void onStartLoading();
    void onStopLoading();

    void onSliderDisabled(QObject *slider);

    void onReloadClicked();

    void onTabClicked(int index);
};

#endif // MAINWINDOW_H
