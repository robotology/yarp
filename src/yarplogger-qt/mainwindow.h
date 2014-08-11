#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QTimer>
#include <QSortFilterProxyModel>
#include <yarp/os/YarprunLogger.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_lineEdit_2_textChanged(const QString &arg1);

    void updateMain();

    void on_startLogger_clicked();


    void on_stopLogger_clicked();

    void on_refreshLogger_clicked();

    void on_logtabs_tabCloseRequested(int index);

    void on_treeView_doubleClicked(const QModelIndex &index);

    void on_DisplayErrorEnable_toggled(bool checked);

    void on_DisplayWarningEnable_toggled(bool checked);

    void on_DisplayDebugEnable_toggled(bool checked);

    void on_DisplayInfoEnable_toggled(bool checked);

    void on_DisplayUnformattedEnable_toggled(bool checked);

private:
    yarp::os::YarprunLogger::LoggerEngine* theLogger;

    Ui::MainWindow *ui;
    QStandardItemModel        *model_yarprunports;
    QTimer *mainTimer;

    void loadTextFile();
};

#endif // MAINWINDOW_H
