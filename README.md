# Qt-FramelessNativeWindow

This project is work in progress. 

The Qt-FramelessNativeWindow is a frameless Qt Desktop Window with added support for native OS-features. A frameless Window has no title bar and needs custom close, minimze and maximize buttons. The class **FramelessWindowConverter** converts existing native windows and turns them into frameless ones with all of the expected native os features. It only uses native os calls and is completely independent of Qt. As the name suggests **FramelessWindowConverter** does not create a window at all but rather works by filtering native OS Events. In the example Qt-Application **MachineClicker** the FramelessWindowConverter is used to get a frameless window.
Supported platforms are Windows 10, macOS and Unix/Linux. Tested with Qt 5.11.2.

The native OS features supported are:
- Resizable
- Draggable
- Minimize animation
- Aero shake (Windows 10 only)
- Aero snap (Windows 10 only)
- Window snapping (Unix/Linux)

On macOS there is optional support for the original traffic light buttons. This is the only way i know to support split view on macOS. The traffic lights can also be individually positioned wherever you want. 

On top of that a custom **TranslucentBlurEffect** is implemented. This works by taking screenshots and setting a blurred version of the image as the background of the Qt-Widget. For it to work properly the Qt-Window needs to be hidden before taking the screenshot. This is done everytime the Window gets activated. The drawbacks of this method are that the window disappears for a short amount of time when activated and that only static images are captured. So when a video is playing behind the Qt-Window a screenshot is taken once and shown as a blurred image until the window gets activated again. 

As an example this project is an automated Clicker Application. It simulates mouse clicks at the current cursor location at the given interval. You can also register new Hotkeys to start and stop the clicking. The hotkeys are global system-wide hotkeys and work even if the Qt-Window is not focused. 

| Windows 10 
:-------------------------:
<img width="700" src="https://raw.githubusercontent.com/Ochrazy/Qt-FramelessNativeWindow/master/github-images/WindowsNativeFramelessWindow.gif"> 

| Ubuntu 18.04
:-------------------------:
<img width="700" src="https://raw.githubusercontent.com/Ochrazy/Qt-FramelessNativeWindow/master/github-images/UbuntuNativeFramelessWindow.gif"> 

| macOS
:-------------------------:
<img width="700" src="https://raw.githubusercontent.com/Ochrazy/Qt-FramelessNativeWindow/master/github-images/MacosNativeFramelessWindow.gif"> 
