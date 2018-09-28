#include "ExampleApplication.h"
#include <QPushButton>
#include <QLayout>
#include <QLabel>
#include <QApplication>
#include <QResizeEvent>
#include <QPainter>
#include <QWindow>

ExampleApplication::ExampleApplication(QWidget *parent) : QWidget(parent),
    framelessWindowConverter(), translucencyBlurEffect(this, this)
{
    CloseButton = new QPushButton;
    CloseButton->setFixedSize(35, 25);
    CloseButton->setStyleSheet("QPushButton { "
                               "image:url(:/images/icon_window_close.png);"
                               "background-color:none;"
                               "border:none;"
                               "width:35px;"
                               "height:25px;"
                               "padding:2px;"
                               "border-top-right-radius: 0px;}"
                               "QPushButton:hover{ background-color:red; }");

    MinimizeButton = new QPushButton;
    MinimizeButton->setFixedSize(35, 25);
    MinimizeButton->setStyleSheet("QPushButton { "
                                  "image:url(:/images/icon_window_minimize.png);"
                                  "background-color:none;"
                                  "border:none;"
                                  "width:35px;"
                                  "height:25px;"
                                  "padding:2px;"
                                  "border-top-right-radius: 0px;}"
                                  "QPushButton:hover{ background-color:grey; }");

    MaximizeButton = new QPushButton;
    MaximizeButton->setFixedSize(35, 25);
#ifdef __APPLE__
    MaximizeButton->setStyleSheet("QPushButton { "
                                  "image:url(:/images/icon_window_macOS_fullscreen.png);"
                                  "background-color:none;"
                                  "border:none;"
                                  "width:35px;"
                                  "height:25px;"
                                  "padding:2px;"
                                  "border-top-right-radius: 0px;}"
                                  "QPushButton:hover{ background-color:grey; }");
#else
    MaximizeButton->setStyleSheet("QPushButton { "
                                  "image:url(:/images/icon_window_maximize.png);"
                                  "background-color:none;"
                                  "border:none;"
                                  "width:35px;"
                                  "height:25px;"
                                  "padding:2px;"
                                  "border-top-right-radius: 0px;}"
                                  "QPushButton:hover{ background-color:grey; }");
#endif

    // Background Widget
    leftBackgroundWidget = new QWidget;
    leftBackgroundWidget->setStyleSheet("background-color:none;");
    leftBackgroundWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    leftBackgroundWidget->setFixedWidth(widthOfRightBackgroundWidget);

    testLabel = new QLabel(leftBackgroundWidget);
    testLabel->setText("Options go here - WIP");
    testLabel->setStyleSheet("QLabel { background-color : none; color : white; }");
    testLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    QVBoxLayout* optionsLayout = new QVBoxLayout(leftBackgroundWidget);
    optionsLayout->addWidget(testLabel);

    // Right Background Widget
    rightBackgroundWidget = new QWidget;
    rightBackgroundWidget->setObjectName("rightBackgroundWidget");
    rightBackgroundWidget->setStyleSheet("#rightBackgroundWidget { background-color:black; }");

    QHBoxLayout* hBoxLayout = new QHBoxLayout;
    hBoxLayout->setSpacing(0);
    hBoxLayout->addStretch();
    hBoxLayout->addWidget(MinimizeButton);
    hBoxLayout->addWidget(MaximizeButton);
    hBoxLayout->addWidget(CloseButton);

    QVBoxLayout* rightWidgetLayout = new QVBoxLayout(rightBackgroundWidget);
    rightWidgetLayout->setSpacing(0);
    rightWidgetLayout->addLayout(hBoxLayout);
    machineClicker = new MachineClicker;
    rightWidgetLayout->addWidget(machineClicker);

    // Main Layout left side are options -> right side is content
    QHBoxLayout* TopLevelLayout = new QHBoxLayout(this);
    TopLevelLayout->setSpacing(0);
    TopLevelLayout->addWidget(leftBackgroundWidget);
    TopLevelLayout->addWidget(rightBackgroundWidget);
    TopLevelLayout->setContentsMargins(0,0,0,0);

    qApp->installEventFilter(this);

    setWindowFlags(Qt::Widget | Qt::WindowSystemMenuHint/* | Qt::WindowStaysOnTopHint*/ | Qt::WindowTitleHint | Qt::FramelessWindowHint);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_NoSystemBackground, true);

    // Only necessary on macOS to hide Traffic Lights when they are used
    // Otherwise empty methods are called and hopefully optimized away
    connect(&translucencyBlurEffect, &TranslucentBlurEffect::hideNonQtWidgets, [this]() { framelessWindowConverter.hideForTranslucency(); });
    connect(&translucencyBlurEffect, &TranslucentBlurEffect::showNonQtWidgets, [this]() { framelessWindowConverter.showForTranslucency(); });

    adjustSize(); // apply layout size (constraints) to window
    framelessWindowConverter.setMinMaxWindowSizes(minimumSize().width() - widthOfRightBackgroundWidget, minimumSize().height(), maximumSize().width(), maximumSize().height());

    framelessWindowConverter.useTrafficLightsOnMacOS(true);

    FWC::FWCPARAMS fwcParams;
    fwcParams.windowHandle = winId();
    fwcParams.releaseMouseGrab = [this]() { windowHandle()->setMouseGrabEnabled(false); };
    fwcParams.shouldPerformWindowDrag =  [this](int mousePosXInWindow, int mousePosYInWindow)
    {
        QWidget* widgetUnderCursor = childAt(mousePosXInWindow, mousePosYInWindow);
        if(widgetUnderCursor == nullptr ||  widgetUnderCursor == rightBackgroundWidget ||  widgetUnderCursor == leftBackgroundWidget)
            return true;
        else return false;
    };

    framelessWindowConverter.convertWindowToFrameless(fwcParams);

    connect(CloseButton, &QAbstractButton::clicked, this, [this]()
    {
        framelessWindowConverter.closeWindow();
    });
    connect(MinimizeButton, &QAbstractButton::clicked, this, [this]()
    {
        framelessWindowConverter.minimizeWindow();
    });
    connect(MaximizeButton, &QAbstractButton::clicked, this, [this]()
    {
#ifdef __APPLE__
        if(!(qApp->keyboardModifiers() & Qt::AltModifier) || window()->isFullScreen())
        {
            framelessWindowConverter.toggleFullscreen();
        }
        else
#endif
        {
            if(window()->isMaximized())
            {
                framelessWindowConverter.restoreWindow();
#ifdef __APPLE__
                MaximizeButton->setStyleSheet("QPushButton { "
                                              "image:url(:/images/icon_window_macOS_maximize.png);"
                                              "background-color:none;"
                                              "border:none;"
                                              "width:35px;"
                                              "height:25px;"
                                              "padding:2px;"
                                              "border-top-right-radius: 0px;}"
                                              "QPushButton:hover{ background-color:grey; }");
#else
                MaximizeButton->setStyleSheet("QPushButton { "
                                              "image:url(:/images/icon_window_maximize.png);"
                                              "background-color:none;"
                                              "border:none;"
                                              "width:35px;"
                                              "height:25px;"
                                              "padding:2px;"
                                              "border-top-right-radius: 0px;}"
                                              "QPushButton:hover{ background-color:grey; }");
#endif
            }
            else
            {
                framelessWindowConverter.maximizeWindow();
#ifdef __APPLE__
                MaximizeButton->setStyleSheet("QPushButton { "
                                              "image:url(:/images/icon_window_macOS_maximize.png);"
                                              "background-color:none;"
                                              "border:none;"
                                              "width:20px;"
                                              "height:20px;"
                                              "padding:0px;"
                                              "border-top-right-radius: 0px;}"
                                              "QPushButton:hover{ background-color:grey; }");
#else
                MaximizeButton->setStyleSheet("QPushButton { "
                                              "image:url(:/images/icon_window_restore.png);"
                                              "background-color:none;"
                                              "border:none;"
                                              "width:20px;"
                                              "height:20px;"
                                              "padding:0px;"
                                              "border-top-right-radius: 0px;}"
                                              "QPushButton:hover{ background-color:grey; }");
#endif
            }
        }
    });
}

bool ExampleApplication::nativeEvent(const QByteArray& eventType, void* message, long* result)
{
    Q_UNUSED(eventType)
    return framelessWindowConverter.filterNativeEvents(message, result);
}

void ExampleApplication::resizeEvent(QResizeEvent* ev)
{
    if(leftBackgroundWidget && ev->oldSize().width() != -1)
    {

        if(!leftBackgroundWidget->isHidden() && ev->size().width() <= (framelessWindowConverter.getMinimumWindowWidth() + widthOfRightBackgroundWidget))
        {
            leftBackgroundWidget->hide();
        }
        else if(leftBackgroundWidget->isHidden() && ev->size().width() > (framelessWindowConverter.getMinimumWindowWidth() + widthOfRightBackgroundWidget))
        {
            leftBackgroundWidget->show();
        }
    }
}

void ExampleApplication::keyPressEvent(QKeyEvent* ev)
{
    if(ev->modifiers() & Qt::AltModifier)
    {
#ifdef __APPLE__
        MaximizeButton->setStyleSheet("QPushButton { "
                                      "image:url(:/images/icon_window_macOS_maximize.png);"
                                      "background-color:black;"
                                      "border:none;"
                                      "width:20px;"
                                      "height:20px;"
                                      "padding:4px;"
                                      "border-top-right-radius: 0px;}"
                                      "QPushButton:hover{ background-color:grey; }");
#endif
    }
}

void ExampleApplication::keyReleaseEvent(QKeyEvent* ev)
{
    if(!(ev->modifiers() & Qt::AltModifier))
    {
#ifdef __APPLE__
        MaximizeButton->setStyleSheet("QPushButton { "
                                      "image:url(:/images/icon_window_macOS_fullscreen.png);"
                                      "background-color:black;"
                                      "border:none;"
                                      "width:20px;"
                                      "height:20px;"
                                      "padding:4px;"
                                      "border-top-right-radius: 0px;}"
                                      "QPushButton:hover{ background-color:grey; }");
#endif
    }
}

void ExampleApplication::paintEvent(QPaintEvent* ev)
{
    // Color the blurred background (supports macOS DarkMode)
    QPainter painter(this);
#ifdef __APPLE__
    QColor color = this->palette().color(QPalette::ColorRole::Background);
#else
    QColor color(0,0,0);
#endif
    color.setAlpha(255);
    painter.setOpacity(0.4);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.fillRect(ev->rect(),color);
}
