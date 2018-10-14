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
    CloseButton->setStyleSheet(getStyleSheetString(":/images/icon_window_close.png", "red" , "lightcoral"));

    MinimizeButton = new QPushButton;
    MinimizeButton->setFixedSize(35, inTitleBarHeight);
    MinimizeButton->setStyleSheet(getStyleSheetString(":/images/icon_window_minimize.png", "grey", "lightgrey"));

    MaximizeButton = new QPushButton;
    MaximizeButton->setFixedSize(35, inTitleBarHeight);
#ifdef __APPLE__
    MaximizeButton->setStyleSheet(getStyleSheetString(":/images/icon_window_macOS_fullscreen.png", "grey", "lightgrey"));
#else
    MaximizeButton->setStyleSheet(getStyleSheetString(":/images/icon_window_maximize.png", "grey", "lightgrey"));
#endif

    QHBoxLayout* hBoxLayout = new QHBoxLayout(this);
    hBoxLayout->setSpacing(0);
    hBoxLayout->addStretch();
    hBoxLayout->addWidget(MinimizeButton);
    hBoxLayout->addWidget(MaximizeButton);
    hBoxLayout->addWidget(CloseButton);
    hBoxLayout->setContentsMargins(1,1,1,1);

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

QString WindowButtons::getStyleSheetString(QString iconName, QString hoverBackgroundColor, QString pressedBackgroundColor)
{
    return  QString("QPushButton { image:url("
                    + iconName + ");"
                                 "background-color:none;"
                                 "border:none;"
                                 "padding:0px;"
                                 "border-top-right-radius: 0px;}"
                                 "QPushButton:hover{ background-color:" + hoverBackgroundColor + "; }"
                                 "QPushButton:pressed{ background-color:" + pressedBackgroundColor + "; }");
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
            // Do nothing -> show fullscreen icon
#else
            MaximizeButton->setStyleSheet(getStyleSheetString(":/images/icon_window_maximize.png", "grey", "lightgrey"));
#endif
        }
        else if (windowWidget->windowState() & Qt::WindowMaximized && !(stateEvent->oldState() & Qt::WindowMaximized))
        {
#ifdef __APPLE__
            // Do nothing -> show fullscreen icon
#else
            MaximizeButton->setStyleSheet(getStyleSheetString(":/images/icon_window_restore.png", "grey", "lightgrey"));
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
            MaximizeButton->setStyleSheet(getStyleSheetString(":/images/icon_window_macOS_maximize.png", "grey"));
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
            MaximizeButton->setStyleSheet(getStyleSheetString(":/images/icon_window_macOS_fullscreen.png", "grey"));
#endif
        }
        break;
    }
    default:
        break;
    }
    return false;
}
