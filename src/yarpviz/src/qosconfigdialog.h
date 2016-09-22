#ifndef QOSCONFIGDIALOG_H
#define QOSCONFIGDIALOG_H

#include <QDialog>
#include <NetworkProfiler.h>
#include <ggraph.h>
#include <qcombobox.h>
#include <qlineedit.h>

namespace Ui {
class QosConfigDialog;
}

class QosConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QosConfigDialog(yarp::graph::Edge *edge, QWidget *parent = 0);
    ~QosConfigDialog();

private slots:
    void apply();
    void reset();

private:
    void createGui();

private:
    yarp::graph::Edge *edge;
    Ui::QosConfigDialog *ui;
    QComboBox* srcPacketProCombo;
    QLineEdit* srcThreadPro;
    QLineEdit* srcThreadPolicy;
    QComboBox* dstPacketProCombo;
    QLineEdit* dstThreadPro;
    QLineEdit* dstThreadPolicy;
};

#endif // QOSCONFIGDIALOG_H
