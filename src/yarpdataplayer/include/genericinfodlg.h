#ifndef GENERICINFODLG_H
#define GENERICINFODLG_H

#include <QDialog>

namespace Ui {
class GenericInfoDlg;
}

class GenericInfoDlg : public QDialog
{
    Q_OBJECT

public:
    explicit GenericInfoDlg(QString title, QString description, QString text, QWidget *parent = 0);
    ~GenericInfoDlg();

private:
    Ui::GenericInfoDlg *ui;
};

#endif // GENERICINFODLG_H
