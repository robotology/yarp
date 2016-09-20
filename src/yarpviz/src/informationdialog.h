#ifndef INFORMATIONDIALOG_H
#define INFORMATIONDIALOG_H

#include <QDialog>
#include <NetworkProfiler.h>
#include <ggraph.h>

namespace Ui {
class InformationDialog;
}

class InformationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InformationDialog(QWidget *parent = 0);
    ~InformationDialog();
    Ui::InformationDialog* getUi();

    void setProcessVertexInfo(ProcessVertex* vertex);
    void setPortVertexInfo(PortVertex* vertex);
    void setEdgeInfo(const yarp::graph::Edge* edge);

private:
    Ui::InformationDialog *ui;
};

#endif // INFORMATIONDIALOG_H
