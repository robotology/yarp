/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: LGPL-2.1-or-later
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

#include <vector>

namespace Ui {
class MainWindow;
}

using namespace yarp::os;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    bool init(QStringList enabledParts,
              ResourceFinder& finder,
              bool debug_param_enabled,
              bool speedview_param_enabled,
              bool enable_calib_all);
    ~MainWindow();

    void term();

protected:
    void closeEvent(QCloseEvent *event) override;
private:
    Ui::MainWindow*  m_ui;
    QTabWidget*      m_tabPanel;
    QToolBar*        m_globalToolBar;
    QToolBar*        m_partToolBar;
    QLabel*          m_partName;
    QTimer           m_timer;
    QMenu*           m_currentPartMenu;
    QMutex           m_mutex;
    int              m_sequenceActiveCount;
    sliderOptions*   m_sliderOpt;
    ResourceFinder   m_finder;
    std::string      m_user_script1;
    std::string      m_user_script2;

    QAction *m_goAll;
    QAction *m_runAllSeq;
    QAction *m_runAllSeqTime;
    QAction *m_saveAllSeq;
    QAction *m_loadAllSeq;
    QAction *m_cycleAllSeq;
    QAction *m_cycleAllSeqTime;
    QAction *m_stopAllSeq;
    QAction *m_idleAllParts;
    QAction *m_runAllParts;
    QAction *m_homeAllParts;
    std::vector<QAction *> m_customPositionsAllParts;
    std::vector<QAction *> m_customPositionsSinglePart;
    std::vector<QAction *> m_customPositionsSinglePartToolbar;
    QAction *openSequenceAction;
    QAction *m_runSinglePart;
    QAction *m_calibSinglePart;
    QAction *m_homeSinglePart;
    QAction *m_idleSinglePart;
    QAction *m_script2;
    QAction *m_script1;

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
    void onRunSinglePart();
    void onRunAllParts();
    void onRunTimeAllSeq();
    void onRunAllSeq();
    void onCycleAllSeq();
    void onCycleTimeAllSeq();
    void onUpdate();
    void onIdleAllParts();
    void onIdleSinglePart();
    void onHomeSinglePart();
    void onHomeAllParts();
    void onHomeSinglePartToCustomPosition(const yarp::os::Bottle& positionElement);
    void onHomeAllPartsToCustomPosition(const yarp::os::Bottle& positionElement);
    void onCalibSinglePart();
    void onGoAll();
    void onExecuteScript1();
    void onExecuteScript2();
    void onViewGlobalToolbar(bool);
    void onViewPartToolbar(bool);
    void onViewSpeeds(bool);
    void onViewCurrents(bool);
    void onViewMotorPositions(bool);
    void onViewDutyCycles(bool);
    void onViewPositionTarget(bool);
    void onEnableControlVelocity(bool val);
    void onEnableControlMixed(bool val);
    void onEnableControlPositionDirect(bool val);
    void onEnableControlPWM(bool val);
    void onEnableControlCurrent(bool val);
    void onSliderOptionsClicked();
    void onSetPosSliderOptionMW(int, double);
    void onSetVelSliderOptionMW(int, double);
    void onSetTrqSliderOptionMW(int, double);

signals:
    void sig_enableControlVelocity(bool);
    void sig_enableControlMixed(bool);
    void sig_enableControlPositionDirect(bool);
    void sig_enableControlPWM(bool);
    void sig_enableControlCurrent(bool);
    void sig_viewSpeedValues(bool);
    void sig_viewCurrentValues(bool);
    void sig_viewMotorPositions(bool);
    void sig_viewDutyCycles(bool);
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
