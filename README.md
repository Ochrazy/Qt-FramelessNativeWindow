# Qt-FramelessNativeWindow

This project is work in progress. 

The Qt-FramelessNativeWindow is a frameless Qt Desktop Window with added support for native OS-features.
Supported platforms are Windows 10, Macos and Unix/Linux. 

On Windows 10 the native OS features supported are:
- Resizable
- Draggable
- Minimize animation
- Aero snap support
- Aero shake support

On top of that a custom translucent blur effect is implemented. This works by taking screenshots and setting a blurred version of the image as the background of the Qt-Widget. For it to work properly the Qt-Window needs to be hidden before taking the screenshot. This is done everytime the Window gets activated. The drawbacks of this method are that the window disappears for a short amount of time when activated and that only static images are captured. So when a video is playing behind the Qt-Window a screenshot is taken once and shown as a blurred image until the window gets activated again. 

As an example this project is an automated Clicker Application. It simulates mouse clicks at the current cursor location at the given interval. You can also register new Hotkeys to start and stop the clicking. The hotkeys are global system-wide hotkeys and work even if the Qt-Window is not focused. 

![Alt Text](https://raw.githubusercontent.com/Ochrazy/Qt-FramelessNativeWindow/master/github-images/WindowsNativeFramelessWindow.gif)
