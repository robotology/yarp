#ifndef STARTDLG_H
#define STARTDLG_H

#include <QDialog>
#include <QCheckBox>

namespace Ui {
class StartDlg;
}

class StartDlg : public QDialog
{
    Q_OBJECT

public:
    explicit StartDlg(QWidget *parent = 0);
    ~StartDlg();

    void init(QString robotName, QStringList partsName, QList<int> ENA);
    QList <int> getEnabledParts();
    QString getRobotName();

private:
    Ui::StartDlg *ui;

    QList <QCheckBox*> checkList;
};

#endif // STARTDLG_H
