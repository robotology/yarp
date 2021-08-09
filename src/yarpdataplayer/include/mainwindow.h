/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWaitCondition>
#include <QMutex>
#include <QTreeWidgetItem>
#include <QCloseEvent>
#include <QThread>
#include <QTimer>
#include <string>
#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include "include/utils.h"
#include "include/worker.h"
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Value.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/RpcClient.h>
#include <yarp/os/RpcServer.h>
#include "yarpdataplayer_IDL.h"
#include "include/loadingwidget.h"
#include <yarp/dataplayer/YarpDataplayer.h>

class InitThread;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, public yarp::os::ResourceFinder, public yarpdataplayer_IDL
{
    Q_OBJECT
    friend class QUtilities;

public:
    explicit MainWindow(yarp::os::ResourceFinder &rf,QWidget *parent = 0);
    ~MainWindow();

    /**
     * function that adds a data part to the main window
     */
    void addPart(const char* szName, const char* type, int frames, const char* portName, const char* szFileName=NULL);
    /**
     * function that sets the dataset part progress bar
     */
    bool setPartProgress(const char* szName, int percentage);
    /**
     * function that sets the initial dataset part progress bar
     */
    bool setInitialPartProgress(const char* szName, int percentage);

    /**
     * function that handles individual dataset part ports
     */
    bool getPartPort(const char* szName, QString *dest);
    /**
     * function that sets the frame rate
     */
    bool setFrameRate(const char* szName, int frameRate);
    /**
     * function that sets the time taken
     */
    bool setTimeTaken(const char* szName, double time);
    /**
     * function that sets the play progress bar
     */
    void setPlayProgress(int percentage);

    /**
     * function that that attaches the rpcServer port for IDL
     */
    bool attach(yarp::os::RpcServer &source);
    /**
     * function that that handles an IDL message - step
     */
    bool step() override;
    /**
     * function that handles an IDL message - setFrame
     */
    bool setFrame(const int frameNum) override;
    /**
     * function that handles an IDL message - getFrame
     */
    int  getFrame(const std::string &name) override;
    /**
     * function that handles an IDL message - load
     */
    bool load(const std::string &path) override;
    /**
     * function that returns slider percentage
     */
    int  getSliderPercentage() override;
    /**
     * function that returns the player status (playing, paused, stopped)
     */
    std::string getStatus() override;
    /**
     * function that handles an IDL message - play
     */
    bool play() override;
    /**
     * function that handles an IDL message - pause
     */
    bool pause() override;
    /**
     * function that handles an IDL message - stop
     */
    bool stop() override;
    /**
     * function that handles an IDL message - quit
     */
    bool quit() override;


private:
    /**
     * function that setups the required actions
     */
    void setupActions();
    /**
     * function that setups the required signals
     */
    void setupSignals();
    /**
     * function that setups the main window
     */
    bool doGuiSetup(QString newPath);
    /**
     * function that move the progress to a percentage
     */
    void goToPercentage(int value);
    /**
     * function that closes the module from the gui
     */
    bool safeExit();
    /**
     * function that closes the module from the terminal
     */
    bool cmdSafeExit();

    QTreeWidgetItem *getRowByPart(QString szName);

protected:
    /**
     * function that updates the frame number
     */
    bool updateFrameNumber(int number);
    /**
     * function that creates utilities
     */
    void createUtilities();
    /**
     * function that deletes utilities
     */
    void clearUtilities();
    /**
     * function that gets the frame command
     */
    void  getFrameCmd(const char* part, int *frame);
    /**
     * function steps datasets when requeted from terminal
     */
    void stepFromCommand(yarp::os::Bottle &reply);

    void closeEvent(QCloseEvent *event) override;

private:

    Ui::MainWindow              *ui;
    QString                     moduleName;
    bool                        add_prefix; //indicates if ports have to be opened with /<moduleName> as prefix
    bool                        verbose;
    std::string                      dataset;
    yarp::os::RpcServer         rpcPort;
    std::vector<yarp::yarpDataplayer::RowInfo>        rowInfoVec;
    int                         subDirCnt;
    std::vector<std::string>    dataType;

    QMutex waitMutex;
    QWaitCondition waitCond;

    int percentage;
    QMutex mutex;
    bool pressed;
    InitThread *initThread;
    LoadingWidget loadingWidget;
    QString errorMessage;

protected:
    QUtilities *qutilities;

    std::map<const char*,int>   partMap;
    int                         itr;
    int                         column;
    bool                        withExtraTimeCol;
    bool                        quitFromCmd;


public slots:
    /**
     * function that gets which parts are activated
     */
    bool getPartActivation(const char* szName);

signals:
    void internalQuit();
    void internalLoad(QString);
    void internalPlay();
    void internalPause();
    void internalStop();
    void internalStep(yarp::os::Bottle *reply);
    void internalSetFrame(const int frameNum);
    void internalGetFrame(const std::string &name, int *frame);
    void internalGetSliderPercentage(int * percentage);

private slots:
    void onInternalQuit();
    void onItemDoubleClicked(QTreeWidgetItem *item,int column);
    void onErrorMessage(QString msg);
    void onInitDone(int subDirCount);
    void onMenuFileOpen();
    void onMenuPlayBackPlay();
    void onMenuPlayBackPause();
    void onMenuPlayBackStop();
    void onMenuPlayBackForward();
    void onMenuPlayBackBackward();
    void onMenuPlayBackStrict();
    void onMenuPlayBackRepeat();
    void onMenuHelpAbout();
    void onMenuSpeedUp();
    void onMenuSpeedDown();
    void onMenuSpeedNormal();
    void onUpdateGuiRateThread();
    void onSpeedValueChanged(int value);
    void onSliderPressed();
    void onSliderReleased();
    void onClose();
    void resetButtonOnStop();

    void onInternalLoad(QString);
    void onInternalPlay();
    void onInternalPause();
    void onInternalStop();
    void onInternalStep(yarp::os::Bottle *reply);
    void onInternalSetFrame(const int frameNum);
    void onInternalGetFrame(const std::string &name, int *frame);
    void onInternalGetSliderPercentage(int *frame);

};

/***********************************************************/
class InitThread : public QThread
{
    Q_OBJECT

public:
    InitThread(QUtilities *qutilities,
               QString newPath,
               std::vector<yarp::yarpDataplayer::RowInfo>& rowInfoVec,
               QObject *parent = 0);

protected:
    void run() override;

private:
    QUtilities *qutilities;
    QString newPath;
    QMainWindow *mainWindow;
    std::vector<yarp::yarpDataplayer::RowInfo>        rowInfoVec;
signals:
    void initDone(int subDirCount);
};
#endif // MAINWINDOW_H
