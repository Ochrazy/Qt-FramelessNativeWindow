#include "MachineClicker.h"
#include <QApplication>
#include "NativeWindowWindows.h"

int main(int argc, char *argv[])
{
    QCoreApplication::addLibraryPath("C:\\Qt\\5.11.1\\Src\\qtbase\\plugins");
    QCoreApplication::addLibraryPath("//Users//Ochrazy//Qt//5.11.1//Src//qtbase//plugins");
    QApplication a(argc, argv);
    MachineClicker w;
    w.show();

    return a.exec();

//    try {
//            NativeWindowWindows nativeWindow;

//           MSG msg;
//           while (::GetMessageW(&msg, nullptr, 0, 0) == TRUE) {
//               ::TranslateMessage(&msg);
//               ::DispatchMessageW(&msg);
//           }
//       }
//       catch (const std::exception& e) {
//           ::MessageBoxA(nullptr, e.what(), "Unhandled Exception", MB_OK|MB_ICONERROR);
//       }
}
