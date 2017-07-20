#ifndef BATCHQOSCONFDIALOG_H
#define BATCHQOSCONFDIALOG_H

#include <QDialog>
#include "NetworkProfiler.h"

namespace Ui {
class BatchQosConfDialog;
}

class BatchQosConfDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BatchQosConfDialog(QWidget *parent = 0);
    ~BatchQosConfDialog();

private slots:
    void openCons();
    void updateQos();
    void configureQos();

private:
    Ui::BatchQosConfDialog *ui;
    yarp::graph::Graph* graph;
};

#endif // BATCHQOSCONFDIALOG_H
