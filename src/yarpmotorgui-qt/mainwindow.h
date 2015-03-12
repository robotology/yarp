#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QResizeEvent>
#include <QLabel>
#include <yarp/os/ResourceFinder.h>
#include <QTimer>
#include <QAction>
#include <QMutex>
#include <QTreeWidget>
#include "partitem.h"

namespace Ui {
class MainWindow;
}

using namespace yarp::os;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void init(QString robotName, QStringList enabledParts,
              ResourceFinder *finder,
              bool debug_param_enabled,
              bool speedview_param_enabled,
              bool enable_calib_all, bool position_direct_enabled, bool openloop_enabled);
    ~MainWindow();

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

signals:
    void viewSpeedValues(bool);
};

class ModesTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    ModesTreeWidget(QWidget * parent = 0);


};

#endif // MAINWINDOW_H
