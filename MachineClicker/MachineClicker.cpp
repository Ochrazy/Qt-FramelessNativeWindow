#include "MachineClicker.h"

#include <QTimer>
#include <QKeyEvent>
#include <QAbstractEventDispatcher>
#include "QStatusBar"
#include "Clicker.h"
#include <QPushButton>
#include <QSpinBox>
#include <QKeySequenceEdit>
#include <QGridLayout>
#include <QLabel>
#include <QApplication>
#include <QFrame>
#include <QPainter>
#include <QStyle>
#include <QWindow>
#include <QPalette>

MachineClicker::MachineClicker(QWidget *parent) :
    QWidget(parent), framelessWindowConverter(), translucencyBlurEffect(this, this)
{
    CloseButton = new QPushButton;
    CloseButton->setFixedSize(20, 20);
    CloseButton->setStyleSheet("QPushButton { "
                               "image:url(:/images/icon_window_close.png);"
                               "background-color:black;"
                               "border:none;"
                               "width:20px;"
                               "height:20px;"
                               "padding:0px;"
                               "border-top-right-radius: 0px;}"
                               "QPushButton:hover{ background-color:red; }");

    MinimizeButton = new QPushButton;
    MinimizeButton->setFixedSize(20, 20);
    MinimizeButton->setStyleSheet("QPushButton { "
                                  "image:url(:/images/icon_window_minimize.png);"
                                  "background-color:black;"
                                  "border:none;"
                                  "width:20px;"
                                  "height:20px;"
                                  "padding:0px;"
                                  "border-top-right-radius: 0px;}"
                                  "QPushButton:hover{ background-color:grey; }");

    MaximizeButton = new QPushButton;
    MaximizeButton->setFixedSize(20, 20);
#ifdef __APPLE__
    MaximizeButton->setStyleSheet("QPushButton { "
                                  "image:url(:/images/icon_window_macOS_fullscreen.png);"
                                  "background-color:black;"
                                  "border:none;"
                                  "width:20px;"
                                  "height:20px;"
                                  "padding:0px;"
                                  "border-top-right-radius: 0px;}"
                                  "QPushButton:hover{ background-color:grey; }");
#else
    MaximizeButton->setStyleSheet("QPushButton { "
                                  "image:url(:/images/icon_window_maximize.png);"
                                  "background-color:black;"
                                  "border:none;"
                                  "width:20px;"
                                  "height:20px;"
                                  "padding:0px;"
                                  "border-top-right-radius: 0px;}"
                                  "QPushButton:hover{ background-color:grey; }");
#endif

    StartStopButton = new QPushButton;
    StartStopButton->setCheckable(true);
    StartStopButton->setText("Start");
    StartStopButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    IntervalSpinBox = new QSpinBox;
    IntervalSpinBox->setMinimum(1);
    IntervalSpinBox->setMaximum(999);
    IntervalSpinBox->setValue(999);

    HotkeyEdit = new QKeySequenceEdit;

    HotkeyEditLabel = new QLabel;
    HotkeyEditLabel->setText("Start/Stop Hotkey: ");
    HotkeyEditLabel->setStyleSheet("QLabel { background-color : none; color : white; }");

    qApp->installEventFilter(this);

    QLabel* windowTitle = new QLabel;
    windowTitle->setText("Native Frameless Window");
    windowTitle->setStyleSheet("QLabel { background-color : none; color : white; }");

    QGridLayout* GridLayout = new QGridLayout;
    GridLayout->addWidget(windowTitle, 0, 0, 1, 1);
    GridLayout->addWidget(StartStopButton, 1, 0, 2, 1);
    GridLayout->addWidget(IntervalSpinBox, 1, 1, 2, 1);
    GridLayout->addWidget(HotkeyEditLabel, 3, 0);
    GridLayout->addWidget(HotkeyEdit, 3, 1);
    GridLayout->setSizeConstraint(QLayout::SetDefaultConstraint);

    QHBoxLayout* hBoxLayout = new QHBoxLayout;
    hBoxLayout->addWidget(MinimizeButton);
    hBoxLayout->addWidget(MaximizeButton);
    hBoxLayout->addWidget(CloseButton);
    GridLayout->addLayout(hBoxLayout, 0, 1, Qt::AlignRight);
    GridLayout->setContentsMargins(8, 8, 8, 8);

    // Background Widget
    leftBackgroundWidget = new QWidget;
    leftBackgroundWidget->setStyleSheet("background-color:none;");
    leftBackgroundWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    leftBackgroundWidget->setFixedWidth(widthOfRightBackgroundWidget);

    testLabel = new QLabel(leftBackgroundWidget);
    testLabel->setText("Options go here - WIP");
    testLabel->setStyleSheet("QLabel { background-color : none; color : white; }");

    QVBoxLayout* optionsLayout = new QVBoxLayout(leftBackgroundWidget);
    optionsLayout->addWidget(testLabel);

    // Right Background Widget
    rightBackgroundWidget = new QWidget;
    rightBackgroundWidget->setObjectName("rightBackgroundWidget");
    rightBackgroundWidget->setStyleSheet("#rightBackgroundWidget { background-color:black; }");
    rightBackgroundWidget->setLayout(GridLayout);

    // Main Layout left side are options -> right side are content
    QHBoxLayout* TopLevelLayout = new QHBoxLayout(this);
    TopLevelLayout->addWidget(leftBackgroundWidget);
    TopLevelLayout->addWidget(rightBackgroundWidget);
    TopLevelLayout->setContentsMargins(0,0,0,0);

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
                                              "background-color:black;"
                                              "border:none;"
                                              "width:20px;"
                                              "height:20px;"
                                              "padding:0px;"
                                              "border-top-right-radius: 0px;}"
                                              "QPushButton:hover{ background-color:grey; }");
#else
                MaximizeButton->setStyleSheet("QPushButton { "
                                              "image:url(:/images/icon_window_maximize.png);"
                                              "background-color:black;"
                                              "border:none;"
                                              "width:20px;"
                                              "height:20px;"
                                              "padding:0px;"
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
                                              "background-color:black;"
                                              "border:none;"
                                              "width:20px;"
                                              "height:20px;"
                                              "padding:0px;"
                                              "border-top-right-radius: 0px;}"
                                              "QPushButton:hover{ background-color:grey; }");
#else
                MaximizeButton->setStyleSheet("QPushButton { "
                                              "image:url(:/images/icon_window_restore.png);"
                                              "background-color:black;"
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
    connect(StartStopButton, SIGNAL (toggled(bool)), this, SLOT (handleStartStopButton(bool)));
    connect(IntervalSpinBox, SIGNAL (valueChanged(int)), this, SLOT (handleTimeSpinBoxChanged(int)));
    connect(HotkeyEdit, &QKeySequenceEdit::editingFinished, this, &MachineClicker::editFinished);

    // Connect the hotkey
    connect(&startStopHotkey, &SystemWideHotkey::hotkeyPressed, StartStopButton, &QAbstractButton::toggle);

    clicker = new Clicker(IntervalSpinBox->value());
    timerThread = new QThread();
    clicker->moveToThread(timerThread);
    connect(timerThread, &QThread::finished, clicker, &QObject::deleteLater);
    connect(this, &MachineClicker::signalStartClicking, clicker, &Clicker::startClicking);
    connect(this, &MachineClicker::signalStopClicking, clicker, &Clicker::stopClicking);
    connect(this, &MachineClicker::signalSetNewClickRate, clicker, &Clicker::setClickRate);
    timerThread->start();
}

MachineClicker::~MachineClicker()
{
}

bool MachineClicker::nativeEvent(const QByteArray& eventType, void* message, long* result)
{
    Q_UNUSED(eventType)
    return framelessWindowConverter.filterNativeEvents(message, result);
}

void MachineClicker::resizeEvent(QResizeEvent* ev)
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

void MachineClicker::keyPressEvent(QKeyEvent* ev)
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

void MachineClicker::keyReleaseEvent(QKeyEvent* ev)
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

void MachineClicker::paintEvent(QPaintEvent* ev)
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

void MachineClicker::enterEvent(QEvent *)
{
    if(StartStopButton->isChecked())
    {
        emit signalStopClicking();
    }
}

void MachineClicker::leaveEvent(QEvent *)
{
    if(StartStopButton->isChecked())
    {
        emit signalStartClicking();
    }
}

void MachineClicker::editFinished()
{
    int keycode = HotkeyEdit->keySequence()[0];
    startStopHotkey.registerNewHotkey(Qt::Key(keycode));
}

void MachineClicker::handleStartStopButton(bool isActive)
{
    if(isActive)
    {
        if(frameGeometry().contains(QCursor::pos()) == false)
        {
            emit signalStartClicking();
        }

        StartStopButton->setText("Stop");
    }
    else
    {
        emit signalStopClicking();
        StartStopButton->setText("Start");
    }
}

void MachineClicker::handleTimeSpinBoxChanged(int TimeInMs)
{
    emit signalSetNewClickRate(TimeInMs);
}

void MachineClicker::doClicking()
{
    if(frameGeometry().contains(QCursor::pos()) == true) return;

    InputSimulation::SimulateLeftClick();
}

