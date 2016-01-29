/*
 * Copyright (C) 2010 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Marco Randazzo <marco.randazzo@iit.it>
 *         Francesco Nori <francesco.nori@iit.it>
 *         Davide Perrone <dperrone@aitek.it>
 * CopyPolicy: Released under the terms of the GPLv2 or later, see GPL.TXT
 */


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <yarp/os/ResourceFinder.h>

#include <QMainWindow>
#include <QResizeEvent>
#include <QLabel>
#include <QTimer>
#include <QAction>
#include <QMutex>
#include <QTreeWidget>

#include "partitem.h"
#include "sliderOptions.h"

namespace Ui {
class MainWindow;
}

using namespace yarp::os;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    bool init(QString robotName, QStringList enabledParts,
              ResourceFinder *finder,
              bool debug_param_enabled,
              bool speedview_param_enabled,
              bool enable_calib_all);
    ~MainWindow();

    void term();

protected:
    void closeEvent(QCloseEvent *event);
private:
    Ui::MainWindow *ui;
    QTabWidget *tabPanel;
    QToolBar *globalToolBar;
    QToolBar *partToolBar;
    QLabel *partName;
    QTimer timer;
    QMenu *currentPartMenu;
    QMutex mutex;
    int sequenceActiveCount;
    sliderOptions* sliderOpt;

    QAction *goAll;
    QAction *runAllSeq;
    QAction *runAllSeqTime;
    QAction *saveAllSeq;
    QAction *loadAllSeq;
    QAction *cycleAllSeq;
    QAction *cycleAllSeqTime;
    QAction *stopAllSeq;
    QAction *runAllParts;
    QAction *homeAllParts;
    QAction *openSequenceAction;
    QAction *runAll;
    QAction *calibAll;
    QAction *homeAll;
    QAction *idleAll;

private:
    void updateModesTree(PartItem *part);
    QString getStringMode(int mode);
    QColor getColorMode(int m);
private slots:
    void onSequenceActivated();
    void onSequenceStopped();
    void onSaveAllSeq();
    void onLoadAllSeq();
    void onStopAllSeq();
    void onCurrentPartChanged(int index);
    void onOpenSequenceTab();
    void onRunAll();
    void onRunAllParts();
    void onRunTimeAllSeq();
    void onRunAllSeq();
    void onCycleAllSeq();
    void onCycleTimeAllSeq();
    void onUpdate();
    void onIdleAll();
    void onHomeAll();
    void onHomeAllParts();
    void onCalibAll();
    void onGoAll();
    void onViewGlobalToolbar(bool);
    void onViewPartToolbar(bool);
    void onViewSpeeds(bool);
    void onViewPositionTarget(bool);
    void onEnableControlVelocity(bool val);
    void onEnableControlMixed(bool val);
    void onEnableControlPositionDirect(bool val);
    void onEnableControlOpenloop(bool val);
    void onSliderOptionsClicked();
    void onSetPosSliderOptionMW(int, double);
    void onSetVelSliderOptionMW(int, double);
    void onSetTrqSliderOptionMW(int, double);

signals:
    void sig_enableControlVelocity(bool);
    void sig_enableControlMixed(bool);
    void sig_enableControlPositionDirect(bool);
    void sig_enableControlOpenloop(bool);
    void sig_viewSpeedValues(bool);
    void sig_setPosSliderOptionMW(int, double);
    void sig_setVelSliderOptionMW(int, double);
    void sig_setTrqSliderOptionMW(int, double);
    void sig_viewPositionTarget(bool);
    void sig_internalClose();

};

class ModesTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    ModesTreeWidget(QWidget * parent = 0);


};

#endif // MAINWINDOW_H
