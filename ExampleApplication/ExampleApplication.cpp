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
    qApp->installEventFilter(this);

    createLeftSideWidgets();
    createRightSideWidgets();

    // Main Layout left side are options -> right side is content
    QHBoxLayout* TopLevelLayout = new QHBoxLayout(this);
    TopLevelLayout->setSpacing(0);
    TopLevelLayout->addWidget(leftBackgroundWidget);
    TopLevelLayout->addWidget(rightBackgroundWidget);
    TopLevelLayout->setContentsMargins(0,0,0,0);

    setupFramelessWindow();
}

void ExampleApplication::createLeftSideWidgets()
{
    leftBackgroundWidget = new QWidget;
    leftBackgroundWidget->setStyleSheet("background-color:none;");
    leftBackgroundWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    leftBackgroundWidget->setFixedWidth(widthOfRightBackgroundWidget);
    leftBackgroundWidget->setContentsMargins(0, 0, 0, 0);

    windowTitle = new QLabel(this);
    windowTitle->setText("Example Application");
    windowTitle->setStyleSheet("QLabel { background-color : none; color : white; }");
    windowTitle->setFixedHeight(titleBarHeight);
    windowTitle->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    windowTitle->setContentsMargins(5,5,0,0);

    machineClickerOptionButton = new QPushButton;
    machineClickerOptionButton->setText("Machine Clicker");
    machineClickerOptionButton->setFixedHeight(50);
    machineClickerOptionButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    QVBoxLayout* optionsLayout = new QVBoxLayout;
    optionsLayout->addWidget(machineClickerOptionButton);
    optionsLayout->setSpacing(0);
    optionsLayout->setContentsMargins(0, 5, 0, 0);
    optionsLayout->addStretch(0);

    QVBoxLayout* leftTitleBar = new QVBoxLayout(leftBackgroundWidget);
    leftTitleBar->setSpacing(0);
    leftTitleBar->addWidget(windowTitle);
    leftTitleBar->addLayout(optionsLayout); // content
    leftTitleBar->setContentsMargins(0, 0, 0, 0);
}

void ExampleApplication::createRightSideWidgets()
{
    CloseButton = new QPushButton;
    CloseButton->setFixedSize(35, titleBarHeight);
    CloseButton->setStyleSheet(getSystemButtonStyleSheetString("image:url(:/images/icon_window_close.png)", "red"));

    MinimizeButton = new QPushButton;
    MinimizeButton->setFixedSize(35, titleBarHeight);
    MinimizeButton->setStyleSheet(getSystemButtonStyleSheetString("image:url(:/images/icon_window_minimize.png)", "grey"));

    MaximizeButton = new QPushButton;
    MaximizeButton->setFixedSize(35, titleBarHeight);
#ifdef __APPLE__
    MaximizeButton->setStyleSheet(getSystemButtonStyleSheetString("image:url(:/images/icon_window_macOS_fullscreen.png)", "grey"));
#else
    MaximizeButton->setStyleSheet(getSystemButtonStyleSheetString("image:url(:/images/icon_window_maximize.png)", "grey"));
#endif

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

    QVBoxLayout* rightTitleBar = new QVBoxLayout(rightBackgroundWidget);
    rightTitleBar->setSpacing(0);
    rightTitleBar->addLayout(hBoxLayout);
    machineClicker = new MachineClicker;
    machineClicker->layout()->setContentsMargins(8, 5, 8, 8);
    rightTitleBar->addWidget(machineClicker);
    rightTitleBar->setContentsMargins(0, 0, 0, 0);
}

void ExampleApplication::setupFramelessWindow()
{
    setWindowFlags(Qt::Widget | Qt::WindowSystemMenuHint/* | Qt::WindowStaysOnTopHint*/ | Qt::WindowTitleHint | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_NoSystemBackground, true);

    // Only necessary on macOS to hide Traffic Lights when they are used
    // Otherwise empty methods are called and hopefully optimized away
    connect(&translucencyBlurEffect, &TranslucentBlurEffect::hideNonQtWidgets, [this]() { framelessWindowConverter.hideForTranslucency(); });
    connect(&translucencyBlurEffect, &TranslucentBlurEffect::showNonQtWidgets, [this]() { framelessWindowConverter.showForTranslucency(); });

    // Set some options
    adjustSize(); // apply layout size (constraints) to window
    framelessWindowConverter.setMinMaxWindowSizes(minimumSize().width() - widthOfRightBackgroundWidget, minimumSize().height(), maximumSize().width(), maximumSize().height());
    framelessWindowConverter.useTrafficLightsOnMacOS(true);

    FWC::FWCPARAMS fwcParams;
    fwcParams.windowHandle = winId();
    fwcParams.releaseMouseGrab = [this]() { windowHandle()->setMouseGrabEnabled(false); };
    fwcParams.shouldPerformWindowDrag =  [this](int mousePosXInWindow, int mousePosYInWindow)
    {
        QWidget* widgetUnderCursor = childAt(mousePosXInWindow, mousePosYInWindow);
        // Set all background widgets draggable
        if((widgetUnderCursor == nullptr ||  widgetUnderCursor == rightBackgroundWidget ||  widgetUnderCursor == leftBackgroundWidget
            || widgetUnderCursor == machineClicker || widgetUnderCursor == windowTitle) &&
                (mousePosYInWindow < titleBarHeight))
            return true;
        else return false;
    };

    framelessWindowConverter.convertWindowToFrameless(fwcParams);

    // Connect custom System Buttons
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
                MaximizeButton->setStyleSheet(getSystemButtonStyleSheetString("image:url(:/images/icon_window_macOS_maximize.png)", "grey"));
#else
                MaximizeButton->setStyleSheet(getSystemButtonStyleSheetString("image:url(:/images/icon_window_maximize.png)", "grey"));
#endif
            }
            else
            {
                framelessWindowConverter.maximizeWindow();
#ifdef __APPLE__
                MaximizeButton->setStyleSheet(getSystemButtonStyleSheetString("image:url(:/images/icon_window_macOS_maximize.png)", "grey"));
#else
                MaximizeButton->setStyleSheet(getSystemButtonStyleSheetString("image:url(:/images/icon_window_restore.png)", "grey"));
#endif
            }
        }
    });
}

QString ExampleApplication::getSystemButtonStyleSheetString(QString iconName, QString hoverBackgroundColor)
{
    return  QString("QPushButton { "
                    + iconName + ";"
                                 "background-color:none;"
                                 "border:none;"
                                 "padding:0px;"
                                 "border-top-right-radius: 0px;}"
                                 "QPushButton:hover{ background-color:" + hoverBackgroundColor + "; }");
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
        MaximizeButton->setStyleSheet(getSystemButtonStyleSheetString("image:url(:/images/icon_window_macOS_maximize.png)", "grey"));
#endif
    }
}

void ExampleApplication::keyReleaseEvent(QKeyEvent* ev)
{
    if(!(ev->modifiers() & Qt::AltModifier))
    {
#ifdef __APPLE__
        MaximizeButton->setStyleSheet(getSystemButtonStyleSheetString("image:url(:/images/icon_window_macOS_fullscreen.png)", "grey"));
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
