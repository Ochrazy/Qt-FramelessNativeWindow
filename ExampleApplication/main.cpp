#include "TransparentBorderWidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TransparentBorderWidget transparentBorderWidget;
    transparentBorderWidget.show();
    transparentBorderWidget.resize(transparentBorderWidget.minimumWidth()+60, transparentBorderWidget.minimumHeight()+110);

    return a.exec();
}
