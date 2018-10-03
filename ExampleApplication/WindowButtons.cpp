#include "WindowButtons.h"
#include <QPushButton>
#include <QHBoxLayout>
#include <QEvent>
#include <QWindowStateChangeEvent>
#include <QApplication>

WindowButtons::WindowButtons(int inTitleBarHeight, QWidget *parent) : QWidget(parent)
{
    CloseButton = new QPushButton;
    CloseButton->setFixedSize(35, inTitleBarHeight);
    CloseButton->setStyleSheet(getStyleSheetString("image:url(:/images/icon_window_close.png)", "red"));

    MinimizeButton = new QPushButton;
    MinimizeButton->setFixedSize(35, inTitleBarHeight);
    MinimizeButton->setStyleSheet(getStyleSheetString("image:url(:/images/icon_window_minimize.png)", "grey"));

    MaximizeButton = new QPushButton;
    MaximizeButton->setFixedSize(35, inTitleBarHeight);
#ifdef __APPLE__
    MaximizeButton->setStyleSheet(getStyleSheetString("image:url(:/images/icon_window_macOS_fullscreen.png)", "grey"));
#else
    MaximizeButton->setStyleSheet(getStyleSheetString("image:url(:/images/icon_window_maximize.png)", "grey"));
#endif

    QHBoxLayout* hBoxLayout = new QHBoxLayout(this);
    hBoxLayout->setSpacing(0);
    hBoxLayout->addStretch();
    hBoxLayout->addWidget(MinimizeButton);
    hBoxLayout->addWidget(MaximizeButton);
    hBoxLayout->addWidget(CloseButton);
    hBoxLayout->setContentsMargins(0,0,0,0);

    qApp->installEventFilter(this);
}

QPushButton* WindowButtons::getCloseButton()
{
    return CloseButton;
}

QPushButton* WindowButtons::getMinimizeButton()
{
    return MinimizeButton;
}

QPushButton* WindowButtons::getMaximizeButton()
{
    return MaximizeButton;
}

QString WindowButtons::getStyleSheetString(QString iconName, QString hoverBackgroundColor)
{
    return  QString("QPushButton { "
                    + iconName + ";"
                                 "background-color:none;"
                                 "border:none;"
                                 "padding:0px;"
                                 "border-top-right-radius: 0px;}"
                                 "QPushButton:hover{ background-color:" + hoverBackgroundColor + "; }");
}

bool WindowButtons::eventFilter(QObject* obj, QEvent* event)
{
    // Only filter window widget events
    QWidget* windowWidget = window();
    if(obj != windowWidget) return false;

    switch(event->type())
    {
    case QEvent::WindowStateChange:
    {
        QWindowStateChangeEvent* stateEvent = static_cast<QWindowStateChangeEvent*>(event);
        if (!(windowWidget->windowState() & Qt::WindowMaximized) && (stateEvent->oldState() & Qt::WindowMaximized))
        {
#ifdef __APPLE__
            MaximizeButton->setStyleSheet(getStyleSheetString("image:url(:/images/icon_window_macOS_maximize.png)", "grey"));
#else
            MaximizeButton->setStyleSheet(getStyleSheetString("image:url(:/images/icon_window_maximize.png)", "grey"));
#endif
        }
        else if (windowWidget->windowState() & Qt::WindowMaximized && !(stateEvent->oldState() & Qt::WindowMaximized))
        {
#ifdef __APPLE__
            MaximizeButton->setStyleSheet(getStyleSheetString("image:url(:/images/icon_window_macOS_maximize.png)", "grey"));
#else
            MaximizeButton->setStyleSheet(getStyleSheetString("image:url(:/images/icon_window_restore.png)", "grey"));
#endif
        }
        break;
    }
    case QEvent::KeyPress:
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if(keyEvent->modifiers() & Qt::AltModifier)
        {
#ifdef __APPLE__
            MaximizeButton->setStyleSheet(getStyleSheetString("image:url(:/images/icon_window_macOS_maximize.png)", "grey"));
#endif
        }
        break;
    }
    case QEvent::KeyRelease:
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if(!(keyEvent->modifiers() & Qt::AltModifier))
        {
#ifdef __APPLE__
            MaximizeButton->setStyleSheet(getStyleSheetString("image:url(:/images/icon_window_macOS_fullscreen.png)", "grey"));
#endif
        }
        break;
    }
    default:
        break;
    }
    return false;
}
