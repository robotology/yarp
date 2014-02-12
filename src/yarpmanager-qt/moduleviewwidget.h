#ifndef MODULEVIEWWIDGET_H
#define MODULEVIEWWIDGET_H
#include "manager.h"
#include <QWidget>
#include "genericviewwidget.h"

namespace Ui {
class ModuleViewWidget;
}

class ModuleViewWidget : public GenericViewWidget
{
    Q_OBJECT

public:
    explicit ModuleViewWidget(Module *mod, QWidget *parent = 0);
    ~ModuleViewWidget();

private:
    Module *module;
    Ui::ModuleViewWidget *ui;
};

#endif // MODULEVIEWWIDGET_H
