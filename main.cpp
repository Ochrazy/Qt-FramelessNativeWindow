#include "MachineClicker/MachineClicker.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::addLibraryPath("C:\\Qt\\5.11.1\\Src\\qtbase\\plugins");
    QCoreApplication::addLibraryPath("//Users//Ochrazy//Qt//5.11.1//Src//qtbase//plugins");
    QApplication a(argc, argv);
    MachineClicker w;
    w.show();

    return a.exec();
}
