#ifndef ABOUTDLG_H
#define ABOUTDLG_H

#include <QDialog>

namespace Ui {
class AboutDlg;
}

class AboutDlg : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDlg(QString appName, QString version, QString copyright, QString website, QWidget *parent = 0);
    ~AboutDlg();

private:
    Ui::AboutDlg *ui;

private slots:
    void onCredits();
    void onLicense();
};

#endif // ABOUTDLG_H
