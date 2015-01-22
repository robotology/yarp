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
#include "iCub/utils.h"
#include <yarp/os/ResourceFinder.h>
#include <yarp/os/Value.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/RpcClient.h>
#include <yarp/os/Module.h>
#include <yarp/os/RpcServer.h>
#include "dataSetPlayer_IDL.h"
#include "loadingwidget.h"

using namespace std;
using namespace yarp::os;

class InitThread;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow,public yarp::os::ResourceFinder, public yarp::os::Module, public dataSetPlayer_IDL
{
    Q_OBJECT
    friend class Utilities;


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
    bool step();
    /**
     * function that handles an IDL message - setFrame
     */
    bool setFrame(const std::string &name, const int frameNum);
    /**
     * function that handles an IDL message - getFrame
     */
    int  getFrame(const std::string &name);
    /**
     * function that handles an IDL message - load
     */
    bool load(const std::string &path);
    /**
     * function that handles an IDL message - play
     */
    bool play();
    /**
     * function that handles an IDL message - pause
     */
    bool pause();
    /**
     * function that handles an IDL message - stop
     */
    bool stop();
    /**
     * function that handles an IDL message - quit
     */
    bool quit();


private:
    /**
     * function that setups the required actions
     */
    void setupActions(void);
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
    bool safeExit(void);
    /**
     * function that closes the module from the terminal
     */
    bool cmdSafeExit(void);

    QTreeWidgetItem *getRowByPart(QString szName);

protected:
    /**
     * function that updates the frame number
     */
    bool updateFrameNumber(const char* part, int number);
    /**
     * function that creates utilities
     */
    void createUtilities();
    /**
     * function that deletes utilities
     */
    void clearUtilities();
//    /**
//     * function that lists the list of commands
//     */
//    void listOfCommands(yarp::os::Bottle &reply);
    /**
     * function that gets the frame command
     */
    void  getFrameCmd(const char* part, int *frame);
    /**
     * function steps datasets when requeted from terminal
     */
    void stepFromCommand(yarp::os::Bottle &reply);

    void closeEvent(QCloseEvent *event);


/***********************************************************/
private:
    Ui::MainWindow              *ui;
    QString                     moduleName;
    bool                        add_prefix; //indicates if ports have to be opened with /<moduleName> as prefix
    yarp::os::RpcServer         rpcPort;
    std::vector<std::string>    partsName;
    std::vector<std::string>    partsFullPath;
    std::vector<std::string>    partsInfoPath;
    std::vector<std::string>    partsLogPath;
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
    Utilities                   *utilities;
    std::map<const char*,int>   partMap;
    int                         itr;
    int                         column;
    bool                        withExtraTimeCol;

/***********************************************************/
public slots:
    /**
     * function that gets which parts are activated
     */
    bool getPartActivation(const char* szName);

signals:
    void internalLoad(QString);
    void internalPlay();
    void internalPause();
    void internalStop();
    void internalStep(Bottle *reply);
    void internalSetFrame(const std::string &name, const int frameNum);
    void internalGetFrame(const std::string &name, int *frame);


private slots:
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
    void onInternalStep(Bottle *reply);
    void onInternalSetFrame(const std::string &name, const int frameNum);
    void onInternalGetFrame(const std::string &name, int *frame);

};


class InitThread : public QThread
{
    Q_OBJECT

public:
    InitThread(Utilities *utilities, QString newPath,
               std::vector<std::string>    *partsName,
               std::vector<std::string>    *partsFullPath,
               std::vector<std::string>    *partsInfoPath,
               std::vector<std::string>    *partsLogPath,
               QObject *parent = 0);

protected:
    void run();

private:
    Utilities *utilities;
    QString newPath;
    QMainWindow *mainWindow;
    std::vector<std::string>    *partsName;
    std::vector<std::string>    *partsFullPath;
    std::vector<std::string>    *partsInfoPath;
    std::vector<std::string>    *partsLogPath;

signals:
    void initDone(int subDirCount);
};


#endif // MAINWINDOW_H

