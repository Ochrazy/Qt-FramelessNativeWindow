#include "MachineClicker/MachineClicker.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MachineClicker w;
    w.show();

    return a.exec();
}
