#include "MachineClicker.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::addLibraryPath("C:\\Qt\\5.11.2\\Src\\qtbase\\plugins");
    QApplication a(argc, argv);
    MachineClicker w;
    w.show();

    return a.exec();
}
