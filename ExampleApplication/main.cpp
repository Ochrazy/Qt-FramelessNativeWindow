#include "ExampleApplication.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ExampleApplication exampleApplication;
    exampleApplication.show();
    exampleApplication.resize(exampleApplication.minimumWidth()+50, exampleApplication.minimumHeight()+100);

    return a.exec();
}
