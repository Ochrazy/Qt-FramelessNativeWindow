#include "ExampleApplication.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ExampleApplication optionTab;
    optionTab.show();

    return a.exec();
}
