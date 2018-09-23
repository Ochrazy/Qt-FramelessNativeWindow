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
#include <QGraphicsBlurEffect>
#include <QWindow>
#include <QScreen>
#include <QPixmap>

static bool takingScreen = false;
static bool noDraw = false;
static bool bTakeScreenshot = false;

MachineClicker::MachineClicker(QWidget *parent) :
    QWidget(parent), framelessWindowConverter()
{
    CloseButton = new QPushButton(this);
    CloseButton->setFixedSize(20, 20);
    CloseButton->setStyleSheet("QPushButton { "
                               "image:url(:/images/icon_window_close.png);"
                               "background-color:black;"
                               "border:none;"
                               "width:20px;"
                               "height:20px;"
                               "padding:4px;"
                               "border-top-right-radius: 0px;}"
                               "QPushButton:hover{ background-color:red; }");

    MinimizeButton = new QPushButton(this);
    MinimizeButton->setFixedSize(20, 20);
    MinimizeButton->setStyleSheet("QPushButton { "
                                  "image:url(:/images/icon_window_minimize.png);"
                                  "background-color:black;"
                                  "border:none;"
                                  "width:20px;"
                                  "height:20px;"
                                  "padding:4px;"
                                  "border-top-right-radius: 0px;}"
                                  "QPushButton:hover{ background-color:grey; }");

    MaximizeButton = new QPushButton(this);
    MaximizeButton->setFixedSize(20, 20);
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
#else
    MaximizeButton->setStyleSheet("QPushButton { "
                                  "image:url(:/images/icon_window_maximize.png);"
                                  "background-color:black;"
                                  "border:none;"
                                  "width:20px;"
                                  "height:20px;"
                                  "padding:4px;"
                                  "border-top-right-radius: 0px;}"
                                  "QPushButton:hover{ background-color:grey; }");
#endif

    StartStopButton = new QPushButton(this);
    StartStopButton->setCheckable(true);
    StartStopButton->setText("Start");
    StartStopButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    IntervalSpinBox = new QSpinBox(this);
    IntervalSpinBox->setMinimum(1);
    IntervalSpinBox->setMaximum(999);
    IntervalSpinBox->setValue(999);

    HotkeyEdit = new QKeySequenceEdit(this);

    HotkeyEditLabel = new QLabel(this);
    HotkeyEditLabel->setText("Start/Stop Hotkey: ");

    qApp->installEventFilter(this);

    QLabel* windowTitle = new QLabel(this);
    windowTitle->setText("Native Frameless Window");

    QGridLayout* GridLayout = new QGridLayout(this);
    GridLayout->addWidget(windowTitle, 0, 0, 1, 1);
    GridLayout->addWidget(StartStopButton, 1, 0, 2, 1);
    GridLayout->addWidget(IntervalSpinBox, 1, 1, 2, 1);
    GridLayout->addWidget(HotkeyEditLabel, 3, 0);
    GridLayout->addWidget(HotkeyEdit, 3, 1);
    GridLayout->setSizeConstraint(QLayout::SetDefaultConstraint);

    QHBoxLayout* hBoxLayout = new QHBoxLayout();
    hBoxLayout->addWidget(MinimizeButton);
    hBoxLayout->addWidget(MaximizeButton);
    hBoxLayout->addWidget(CloseButton);
    GridLayout->addLayout(hBoxLayout, 0, 1, Qt::AlignRight);

    setWindowFlags(Qt::Widget | Qt::WindowSystemMenuHint/* | Qt::WindowStaysOnTopHint*/ | Qt::WindowTitleHint | Qt::FramelessWindowHint);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_NoSystemBackground, true);

    adjustSize(); // apply layout size (constraints) to window
    framelessWindowConverter.setMinMaxWindowSizes(minimumSize().width(), minimumSize().height(), maximumSize().width(), maximumSize().height());

    framelessWindowConverter.repaint = [this]() {
        framelessWindowConverter.hideForTranslucency();
        noDraw = true;
        jj = false;
        repaint(); };

    FWC::FWCPARAMS fwcParams;
    fwcParams.windowHandle = winId();
    fwcParams.releaseMouseGrab = [this]() { windowHandle()->setMouseGrabEnabled(false); };
    fwcParams.shouldPerformWindowDrag =  [this](int mousePosXInWindow, int mousePosYInWindow)
    {
        return childAt(mousePosXInWindow, mousePosYInWindow) ? false : true;
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
                                              "padding:4px;"
                                              "border-top-right-radius: 0px;}"
                                              "QPushButton:hover{ background-color:grey; }");
#else
                MaximizeButton->setStyleSheet("QPushButton { "
                                              "image:url(:/images/icon_window_maximize.png);"
                                              "background-color:black;"
                                              "border:none;"
                                              "width:20px;"
                                              "height:20px;"
                                              "padding:4px;"
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
                                              "padding:4px;"
                                              "border-top-right-radius: 0px;}"
                                              "QPushButton:hover{ background-color:grey; }");
#else
                MaximizeButton->setStyleSheet("QPushButton { "
                                              "image:url(:/images/icon_window_restore.png);"
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

    pixmap = windowHandle()->screen()->grabWindow(0);
}

MachineClicker::~MachineClicker()
{
}

bool MachineClicker::nativeEvent(const QByteArray& eventType, void* message, long* result)
{
    Q_UNUSED(eventType)
    return framelessWindowConverter.filterNativeEvents(message, result);
}

QImage blurImage(const QImage& image, const QRect& rect, int radius)
{
    int tab[] = { 14, 10, 8, 6, 5, 5, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2 };
    int alpha = (radius < 1)  ? 16 : (radius > 17) ? 1 : tab[radius-1];

    QImage result = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    int r1 = rect.top();
    int r2 = rect.bottom();
    int c1 = rect.left();
    int c2 = rect.right();

    int bpl = result.bytesPerLine();
    int rgba[4];
    unsigned char* p;

    int i1 = 0;
    int i2 = 3;

    for (int col = c1; col <= c2; col++) {
        p = result.scanLine(r1) + col * 4;
        for (int i = i1; i <= i2; i++)
            rgba[i] = p[i] << 4;

        p += bpl;
        for (int j = r1; j < r2; j++, p += bpl)
            for (int i = i1; i <= i2; i++)
                p[i] = static_cast<unsigned char>((rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4);
    }

    for (int row = r1; row <= r2; row++) {
        p = result.scanLine(row) + c1 * 4;
        for (int i = i1; i <= i2; i++)
            rgba[i] = p[i] << 4;

        p += 4;
        for (int j = c1; j < c2; j++, p += 4)
            for (int i = i1; i <= i2; i++)
                p[i] = static_cast<unsigned char>((rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4);
    }

    for (int col = c1; col <= c2; col++) {
        p = result.scanLine(r2) + col * 4;
        for (int i = i1; i <= i2; i++)
            rgba[i] = p[i] << 4;

        p -= bpl;
        for (int j = r1; j < r2; j++, p -= bpl)
            for (int i = i1; i <= i2; i++)
                p[i] = static_cast<unsigned char>((rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4);
    }

    for (int row = r1; row <= r2; row++) {
        p = result.scanLine(row) + c2 * 4;
        for (int i = i1; i <= i2; i++)
            rgba[i] = p[i] << 4;

        p -= 4;
        for (int j = c1; j < c2; j++, p -= 4)
            for (int i = i1; i <= i2; i++)
                p[i] = static_cast<unsigned char>((rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4);
    }

    return result;
}


static QImage blurredScreenshot;
void MachineClicker::paintEvent(QPaintEvent* ev)
{
    if(useTranslucentBackgroundBlur == false)
    {
        QPainter painter(this);
        painter.setOpacity(0.9);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.fillRect(ev->rect(),QColor(100,100,100,255));
        painter.end();

        bTakeScreenshot = false;
        noDraw = false;
        takingScreen = false;
    }
    else
    {
        if(jj)
        {
            QPainter painter;

            if(bTakeScreenshot)
            {
                pixmap = windowHandle()->screen()->grabWindow(0);
                blurredScreenshot = blurImage(pixmap.toImage(), pixmap.rect(), 25);
                bTakeScreenshot = false;
                noDraw = false;
                takingScreen = false;
                framelessWindowConverter.showForTranslucency();
            }

            painter.begin(this);
            painter.setOpacity(1.0);
            painter.setCompositionMode(QPainter::CompositionMode_Clear);
            painter.fillRect(ev->rect(),QColor(100,100,100,255));

            painter.setOpacity(1.0);
            painter.setCompositionMode(QPainter::CompositionMode_Source);
            painter.drawImage(ev->rect(), blurredScreenshot,
                              QRect(mapToGlobal(ev->rect().topLeft()), mapToGlobal(ev->rect().bottomRight())));

            painter.setOpacity(0.25);
            painter.setCompositionMode(QPainter::CompositionMode_Darken);
            painter.fillRect(ev->rect(),QColor(0,0,0,255));

            painter.end();

        }
        else
        {
            QPainter painter;
            painter.begin(this);
            painter.setOpacity(0.0);
            painter.setCompositionMode(QPainter::CompositionMode_Source);
            painter.fillRect(ev->rect(),QColor(250,100,100,255));
            painter.end();

            if(!takingScreen)
            {
                takingScreen = true;
                QTimer::singleShot(400, this, [this](){
                    bTakeScreenshot = true;
                    jj = true;
                    repaint();
                });
            }
        }
    }
}

bool MachineClicker::event(QEvent* ev)
{
    if(useTranslucentBackgroundBlur)
    {
        if(ev->type() == QEvent::WindowActivate)
        {
            framelessWindowConverter.hideForTranslucency();
            noDraw = true;
            jj = false;
            repaint();
        }
    }

    return QWidget::event(ev);
}

void MachineClicker::moveEvent(QMoveEvent*)
{
    if(useTranslucentBackgroundBlur)
    {
        repaint();
        update();
    }
}

bool MachineClicker::eventFilter(QObject* obj, QEvent* ev)
{
    switch(ev->type())
    {
    case QEvent::Paint:
    {
        if(!noDraw || (obj == this) ) { return false; }
        QPainter painter;
        painter.begin(dynamic_cast<QWidget*>(obj));
        painter.setOpacity(0.0);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.fillRect(rect(),QColor(100,100,100,255));
        painter.end();
        return true;
    }
    default:
        break;
    }

    return false;
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

void MachineClicker::mouseReleaseEvent(QMouseEvent *)
{
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

