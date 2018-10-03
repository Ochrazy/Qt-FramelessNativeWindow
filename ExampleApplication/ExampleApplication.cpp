#include "ExampleApplication.h"
#include <QPushButton>
#include <QLayout>
#include <QLabel>
#include <QApplication>
#include <QResizeEvent>
#include <QPainter>
#include <QWindow>
#include <QStackedLayout>
#include <QScrollArea>
#include <QTimer>
#include <QScrollBar>
#include <QStyle>
#include "MinimalScrollBar.h"
#include "ToggleButton.h"
#include "QPropertyAnimation"
#include "ToggleOption.h"

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

    qApp->installEventFilter(this);
}

void ExampleApplication::createLeftSideWidgets()
{
    leftBackgroundWidget = new QWidget;
    leftBackgroundWidget->setStyleSheet("background-color:none;");
    leftBackgroundWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    leftBackgroundWidget->setFixedWidth(widthOfLeftBackgroundWidget);
    leftBackgroundWidget->setContentsMargins(0, 0, 0, 0);

    windowTitle = new QLabel(this);
    windowTitle->setText("Example Application");
    windowTitle->setStyleSheet("QLabel { background-color : none; color : white; }");
    windowTitle->setFixedHeight(titleBarHeight);
    windowTitle->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    windowTitle->setContentsMargins(5,5,0,0);

    selectionIndicator = new QWidget;
    selectionIndicator->setFixedSize(10, 25);
    selectionIndicator->setStyleSheet("background-color: rgba(100,100,255,255);");
    selectionIndicator->move(0, 12);


    machineClickerOptionButton = new QPushButton;
    machineClickerOptionButton->setText("Machine Clicker");
    machineClickerOptionButton->setFixedHeight(50);
    machineClickerOptionButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    machineClickerOptionButton->setStyleSheet(getOptionButtonStyleSheetString());
    connect(machineClickerOptionButton, &QPushButton::clicked, [this] () { rightStackedLayout->setCurrentWidget(machineClicker);
        // Set new window size limits
        // +16 and +6 for contents margins
        framelessWindowConverter.setMinMaxWindowSizes(machineClicker->layout()->minimumSize().width() + 16,
                                                      machineClicker->layout()->minimumSize().height() + titleBarHeight + 6,
                                                      maximumSize().width(), maximumSize().height());
        rightBackgroundWidget->setMinimumWidth(machineClicker->layout()->minimumSize().width() + 16);
        rightBackgroundWidget->setMinimumHeight(machineClicker->layout()->minimumSize().height() + titleBarHeight + 6);
        selectionIndicator->setParent(machineClickerOptionButton);
        selectionIndicator->show();
    });
    selectionIndicator->setParent(machineClickerOptionButton);
    selectionIndicator->show();


    transparencyOptionButton = new QPushButton;
    transparencyOptionButton->setText("Transparency");
    transparencyOptionButton->setFixedHeight(50);
    transparencyOptionButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    transparencyOptionButton->setStyleSheet(getOptionButtonStyleSheetString());
    connect(transparencyOptionButton, &QPushButton::clicked, [this] () {
        rightStackedLayout->setCurrentWidget(transparencyOptionWidget);
        // Set new window size limits
        // +16 and +6 for contents margins
        framelessWindowConverter.setMinMaxWindowSizes(transparencyOptionWidget->layout()->minimumSize().width(),
                                                      transparencyOptionWidget->layout()->minimumSize().height() + titleBarHeight + 6,
                                                      maximumSize().width(), maximumSize().height());
        rightBackgroundWidget->setMinimumWidth(transparencyOptionWidget->layout()->minimumSize().width());
        rightBackgroundWidget->setMinimumHeight( transparencyOptionWidget->layout()->minimumSize().height() + titleBarHeight + 6);
        selectionIndicator->setParent(transparencyOptionButton);
        selectionIndicator->show();
    });

    FramelessOptionButton = new QPushButton;
    FramelessOptionButton->setText("Frameless window options");
    FramelessOptionButton->setFixedHeight(50);
    FramelessOptionButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    FramelessOptionButton->setStyleSheet(getOptionButtonStyleSheetString());
    connect(FramelessOptionButton, &QPushButton::clicked, [this] () {
        rightStackedLayout->setCurrentWidget(translucentBlurOption);
        // Set new window size limits
        // +16 and +6 for contents margins
        framelessWindowConverter.setMinMaxWindowSizes(translucentBlurOption->minimumWidth(),
                                                      translucentBlurOption->minimumHeight() + titleBarHeight + 6,
                                                      maximumSize().width(), maximumSize().height());
        rightBackgroundWidget->setMinimumWidth(translucentBlurOption->minimumWidth());
        rightBackgroundWidget->setMinimumHeight(translucentBlurOption->minimumHeight() + titleBarHeight + 6);
        selectionIndicator->setParent(FramelessOptionButton);
        selectionIndicator->show();
    });

    leftScrollArea = new QScrollArea;
    leftScrollArea->setFrameShape(QFrame::NoFrame);
    leftScrollArea->viewport()->setStyleSheet("background-color: rgba(0,0,0,0);"
                                              "margin: 0px 4px 0px 0px;");
    leftScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    MinimalScrollBar* minimalScrollBar = new MinimalScrollBar;
    leftScrollArea->setVerticalScrollBar(minimalScrollBar);

    QWidget* containerWidget = new QWidget;
    QVBoxLayout* optionsLayout = new QVBoxLayout(containerWidget);
    leftScrollArea->setWidget(containerWidget);
    leftScrollArea->setWidgetResizable(true);

    optionsLayout->addSpacing(5);
    optionsLayout->addWidget(machineClickerOptionButton);
    optionsLayout->addWidget(transparencyOptionButton);
    optionsLayout->addWidget(FramelessOptionButton);
    optionsLayout->setSpacing(0);
    optionsLayout->setContentsMargins(0, 0, 0, 0);
    optionsLayout->addStretch(1);

    QVBoxLayout* leftTitleBar = new QVBoxLayout(leftBackgroundWidget);
    leftTitleBar->setSpacing(0);
    leftTitleBar->addWidget(windowTitle);
    leftTitleBar->addWidget(leftScrollArea); // content
    leftTitleBar->setContentsMargins(0, 0, 0, 0);
}

void ExampleApplication::createTransparencyOptionWidget()
{
    transparentOption = new ToggleOption;
    transparentOption->setDescription("Turn all transparency effects on/off");
    connect(transparentOption->getButton(), &QAbstractButton::toggled, this, [this](bool checked) {
        if(checked)
        {
            translucencyBlurEffect.deactivateEffect();
            setAttribute(Qt::WA_TranslucentBackground, false);
            setAttribute(Qt::WA_NoSystemBackground, false);
            translucentBlurOption->getButton()->setChecked(true);
            translucentBlurOption->setEnabled(false);
        }
        else
        {
            translucencyBlurEffect.reactivateEffect();
            setAttribute(Qt::WA_TranslucentBackground, true);
            setAttribute(Qt::WA_NoSystemBackground, true);
            translucentBlurOption->getButton()->setChecked(false);
            translucentBlurOption->setEnabled(true);
        }
    });

    translucentBlurOption = new ToggleOption;
    translucentBlurOption->setDescription("Turn the translucent blur effect on/off");
    connect(translucentBlurOption->getButton(), &QAbstractButton::toggled, this, [this]() {
        if(translucentBlurOption->getButton()->isChecked())
            translucencyBlurEffect.deactivateEffect();
        else translucencyBlurEffect.reactivateEffect();
    });

    transparencyOptionWidget = new QWidget;
    QVBoxLayout* transparencyOptionLayout = new QVBoxLayout(transparencyOptionWidget);
    transparencyOptionLayout->addWidget(transparentOption);
    transparencyOptionLayout->addWidget(translucentBlurOption);
    transparencyOptionLayout->addStretch(1);
}

void ExampleApplication::createRightSideWidgets()
{
    windowButtons = new WindowButtons(titleBarHeight);

    // Right Background Widget
    rightBackgroundWidget = new QWidget;
    rightBackgroundWidget->setObjectName("rightBackgroundWidget");
    rightBackgroundWidget->setStyleSheet("#rightBackgroundWidget { background-color:black; }");

    QVBoxLayout* rightTitleBar = new QVBoxLayout(rightBackgroundWidget);
    rightTitleBar->setSpacing(0);
    rightTitleBar->addWidget(windowButtons);
    rightTitleBar->addSpacing(5);


    machineClicker = new MachineClicker;
    createTransparencyOptionWidget();

    rightStackedLayout = new QStackedLayout;
    rightStackedLayout->addWidget(machineClicker);
    rightStackedLayout->addWidget(transparencyOptionWidget);

    machineClicker->layout()->setContentsMargins(8, 5, 8, 8);
    rightTitleBar->addLayout(rightStackedLayout);
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
    rightStackedLayout->currentWidget()->adjustSize();
    framelessWindowConverter.setMinMaxWindowSizes(rightStackedLayout->currentWidget()->size().width() + 16,
                                                  rightStackedLayout->currentWidget()->size().height() + titleBarHeight + 6,
                                                  maximumSize().width(), maximumSize().height());
    rightBackgroundWidget->setMinimumWidth(rightStackedLayout->currentWidget()->size().width() + 16);
    rightBackgroundWidget->setMinimumHeight( rightStackedLayout->currentWidget()->size().height() + titleBarHeight + 6);
    framelessWindowConverter.useTrafficLightsOnMacOS(true);

    FWC::FWCPARAMS fwcParams;
    fwcParams.windowHandle = winId();
    fwcParams.releaseMouseGrab = [this]() { windowHandle()->setMouseGrabEnabled(false); };
    fwcParams.shouldPerformWindowDrag =  [this](int mousePosXInWindow, int mousePosYInWindow)
    {
        QWidget* widgetUnderCursor = childAt(mousePosXInWindow, mousePosYInWindow);
        // Set all background widgets draggable
        if((widgetUnderCursor == nullptr ||  widgetUnderCursor == rightBackgroundWidget ||  widgetUnderCursor == leftBackgroundWidget
            || widgetUnderCursor == machineClicker || widgetUnderCursor == windowTitle || widgetUnderCursor == windowButtons) &&
                (mousePosYInWindow < titleBarHeight))
            return true;
        else return false;
    };

    framelessWindowConverter.convertWindowToFrameless(fwcParams);

    // Connect custom System Buttons
    connect(windowButtons->getCloseButton(), &QAbstractButton::clicked, this, [this]()
    {
        framelessWindowConverter.closeWindow();
    });
    connect(windowButtons->getMinimizeButton(), &QAbstractButton::clicked, this, [this]()
    {
        framelessWindowConverter.minimizeWindow();
    });
    connect(windowButtons->getMaximizeButton(), &QAbstractButton::clicked, this, [this]()
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
                framelessWindowConverter.restoreWindow();
            else
                framelessWindowConverter.maximizeWindow();
        }
    });
}

QString ExampleApplication::getOptionButtonStyleSheetString()
{
    return QString("QPushButton { background-color:none;"
                   "border:none;"
                   "padding:0px;"
                   "border-top-right-radius: 0px;}"
                   "QPushButton:hover{ background-color: lightgrey; border: none; }"
                   "QPushButton:pressed{ background-color: grey;}");
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
        if(!leftBackgroundWidget->isHidden() && ev->size().width() <= (framelessWindowConverter.getMinimumWindowWidth() + widthOfLeftBackgroundWidget))
        {
            leftBackgroundWidget->hide();
        }
        else if(leftBackgroundWidget->isHidden() && ev->size().width() > (framelessWindowConverter.getMinimumWindowWidth() + widthOfLeftBackgroundWidget))
        {
            leftBackgroundWidget->show();
        }
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
