#ifndef RESOURCEVIEWWIDGET_H
#define RESOURCEVIEWWIDGET_H
#include "manager.h"
#include <QWidget>
#include "genericviewwidget.h"


namespace Ui {
class ResourceViewWidget;
}

class ResourceViewWidget : public GenericViewWidget
{
    Q_OBJECT

public:
    explicit ResourceViewWidget(Computer *res,QWidget *parent = 0);
    ~ResourceViewWidget();

private:
    Computer *res;
    Ui::ResourceViewWidget *ui;
};

#endif // RESOURCEVIEWWIDGET_H
