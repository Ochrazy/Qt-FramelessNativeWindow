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

    // First create widgets on the right side
    // Then create selection widget on the left side
    createRightSideWidgets();
    createLeftSideWidgets();

    // Main Layout left side are options -> right side is content
    QHBoxLayout* TopLevelLayout = new QHBoxLayout(this);
    TopLevelLayout->setSpacing(0);
    TopLevelLayout->addWidget(leftBackgroundWidget);
    TopLevelLayout->addWidget(rightBackgroundWidget);
    TopLevelLayout->setContentsMargins(0,0,0,0);

    setupFramelessWindow();

    qApp->installEventFilter(this);
}

QPushButton* ExampleApplication::createOptionSelectionButton(const QString& inText, QWidget* inOptionWidget, QLayout* inLayout)
{
    QPushButton* newOptionSelectionButton = new QPushButton;
    newOptionSelectionButton->setText(inText);
    newOptionSelectionButton->setFixedHeight(50);
    newOptionSelectionButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    newOptionSelectionButton->setStyleSheet(getOptionButtonStyleSheetString());
    connect(newOptionSelectionButton, &QPushButton::clicked, [this, inOptionWidget, newOptionSelectionButton] () {
        rightStackedLayout->setCurrentWidget(inOptionWidget);
        // Set new window size limits
        // +16 and +6 for contents margins
        int minimumWidth = 0;
        int minimumHeight = 0;
        if(inOptionWidget->layout())
        {
            minimumWidth = inOptionWidget->layout()->minimumSize().width() + 16;
            minimumHeight = inOptionWidget->layout()->minimumSize().height() + titleBarHeight + 6;
        }
        else
        {
            minimumWidth = inOptionWidget->minimumWidth() + 16;
            minimumHeight = inOptionWidget->minimumHeight() + titleBarHeight + 6;
        }
        framelessWindowConverter.setMinMaxWindowSizes(minimumWidth,
                                                      minimumHeight,
                                                      maximumSize().width(), maximumSize().height());
        rightBackgroundWidget->setMinimumWidth(minimumWidth);
        rightBackgroundWidget->setMinimumHeight(minimumHeight);
        selectionIndicator->setParent(newOptionSelectionButton);
        selectionIndicator->show();
        update();

        // Resize widget to always show option selection widget after clicking an option
        int minimumWindowWidth = minimumWidth + widthOfLeftBackgroundWidget;
        if(minimumWindowWidth > windowHandle()->size().width())
        {
            adjustSize();
            resize(minimumWindowWidth+1, windowHandle()->size().height());
        }
    });

    inLayout->addWidget(newOptionSelectionButton);
    return newOptionSelectionButton;
}

void ExampleApplication::createLeftSideWidgets()
{
    leftBackgroundWidget = new QWidget;
    leftBackgroundWidget->setStyleSheet("background-color:none;");
    leftBackgroundWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    leftBackgroundWidget->setFixedWidth(widthOfLeftBackgroundWidget);
    leftBackgroundWidget->setContentsMargins(0, 0, 0, 0);
    leftBackgroundWidget->setMinimumWidth(widthOfLeftBackgroundWidget);
    leftBackgroundWidget->setMinimumHeight(titleBarHeight + 6);

    windowTitle = new QLabel(this);
    windowTitle->setText("Example Application");
    windowTitle->setStyleSheet("QLabel { background-color : none; color : white; }");
    windowTitle->setFixedHeight(titleBarHeight);
    windowTitle->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    windowTitle->setContentsMargins(5,5,5,5);

    selectionIndicator = new QWidget;
    selectionIndicator->setFixedSize(10, 24);
    selectionIndicator->setStyleSheet("background-color: #0078d7;");
    selectionIndicator->move(0, 13);

    leftScrollArea = new QScrollArea;
    leftScrollArea->setFrameShape(QFrame::NoFrame);
    leftScrollArea->viewport()->setStyleSheet("background-color: rgba(0,0,0,0);"
                                              "margin: 0px 4px 0px 0px;");
    leftScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    MinimalScrollBar* minimalScrollBar = new MinimalScrollBar;
    leftScrollArea->setVerticalScrollBar(minimalScrollBar);

    QWidget* containerWidget = new QWidget;
    leftScrollArea->setWidget(containerWidget);
    leftScrollArea->setWidgetResizable(true);

    QVBoxLayout* optionsLayout = new QVBoxLayout(containerWidget);
    optionsLayout->addSpacing(5);
    optionsLayout->setContentsMargins(0, 0, 0, 0);

    // Create option buttons
    selectionIndicator->setParent(createOptionSelectionButton("Machine Clicker", machineClicker, optionsLayout));
    selectionIndicator->show();
    createOptionSelectionButton("Transparency", transparencyOptionWidget, optionsLayout);
    createOptionSelectionButton("Frameless window options", framelessOption, optionsLayout);
    createOptionSelectionButton("macOS options", macOSOption, optionsLayout);
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
            setAttribute(Qt::WA_TranslucentBackground, false);
            setAttribute(Qt::WA_NoSystemBackground, false);

            if(!framelessOption->getButton()->isChecked())
            {
                translucencyBlurEffect.deactivateEffect();
                translucentBlurOption->setEnabled(false);
            }
            update();
        }
        else
        {
            setAttribute(Qt::WA_TranslucentBackground, true);
            setAttribute(Qt::WA_NoSystemBackground, true);
            if(!framelessOption->getButton()->isChecked())
            {
                translucentBlurOption->setEnabled(true);
                if(!translucentBlurOption->getButton()->isChecked())
                    translucencyBlurEffect.reactivateEffect();
            }
            update();
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
    framelessOption = new ToggleOption;
    framelessOption->setDescription("Convert window to a frameless native window");
    connect(framelessOption->getButton(), &QAbstractButton::toggled, this, [this]() {
        if(framelessOption->getButton()->isChecked())
        {
            framelessWindowConverter.convertToWindowWithFrame();
            // Override window flags (do not set FramelessWindowHint)
            setWindowFlags(Qt::Widget | Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint
                           | Qt::WindowFullscreenButtonHint);

            // On Windows Qt::FramelessWindowHint needs to be set for translucency
            // Disable all transparency effects on all platforms when not frameless
            setAttribute(Qt::WA_TranslucentBackground, false);
            setAttribute(Qt::WA_NoSystemBackground, false);
            transparentOption->setEnabled(false);
            // This effect would still work but there is no quick way to hide the window anymore
            translucencyBlurEffect.deactivateEffect();
            translucentBlurOption->setEnabled(false);
            show();

            // No need for custom window buttons and title
            windowButtons->hide();
            windowTitle->hide();
        }
        else
        {
            setWindowFlags(Qt::Widget | Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::FramelessWindowHint);
            transparentOption->setEnabled(true);
            if(!transparentOption->getButton()->isChecked())
            {
                setAttribute(Qt::WA_TranslucentBackground, true);
                setAttribute(Qt::WA_NoSystemBackground, true);
                translucentBlurOption->setEnabled(true);
                if(!translucentBlurOption->getButton()->isChecked())
                    translucencyBlurEffect.reactivateEffect();
            }
            show();

            // Convert window
            framelessWindowConverter.convertWindowToFrameless(fwcParams);
            show();
            // Show custom window buttons and title
            windowButtons->show();
            windowTitle->show();
        }
    });

    macOSOption = new ToggleOption;
    macOSOption->setDescription("Use original traffic light buttons");
    connect(macOSOption->getButton(), &QAbstractButton::toggled, this, [this]() {
        if(macOSOption->getButton()->isChecked())
        {
            framelessWindowConverter.useTrafficLightsOnMacOS(false);
            windowTitle->setAlignment(Qt::AlignLeft | Qt::AlignTop);
            windowTitle->setContentsMargins(5,10,0,0);
            framelessWindowConverter.hideForTranslucency();
            windowButtons->show();
        }
        else
        {
            framelessWindowConverter.useTrafficLightsOnMacOS(true);
            windowTitle->setAlignment(Qt::AlignRight | Qt::AlignTop);
            windowTitle->setContentsMargins(0,10,10,0);
            framelessWindowConverter.showForTranslucency();
            windowButtons->hide();
        }
    });

    rightStackedLayout = new QStackedLayout;
    rightStackedLayout->addWidget(machineClicker);
    rightStackedLayout->addWidget(transparencyOptionWidget);
    rightStackedLayout->addWidget(framelessOption);
    rightStackedLayout->addWidget(macOSOption);

    machineClicker->layout()->setContentsMargins(8, 5, 8, 8);
    rightTitleBar->addLayout(rightStackedLayout);
    rightTitleBar->setContentsMargins(0, 0, 0, 0);
}

void ExampleApplication::setupFramelessWindow()
{
    setWindowFlags(Qt::Widget | Qt::WindowSystemMenuHint/* | Qt::WindowStaysOnTopHint*/ | Qt::WindowTitleHint | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_NoSystemBackground, true);

    setWindowTitle("Example Application");

    // Only necessary on macOS to hide Traffic Lights when they are used
    // Otherwise empty methods are called and hopefully optimized away
    connect(&translucencyBlurEffect, &TranslucentBlurEffect::hideNonQtWidgets, [this]() { framelessWindowConverter.hideForTranslucency(); });
    connect(&translucencyBlurEffect, &TranslucentBlurEffect::showNonQtWidgets, [this]() { framelessWindowConverter.showForTranslucency(); });

    // Set some settings
    adjustSize(); // apply layout size (constraints) to window
    rightStackedLayout->currentWidget()->adjustSize();
    framelessWindowConverter.setMinMaxWindowSizes(rightStackedLayout->currentWidget()->size().width() + 16,
                                                  rightStackedLayout->currentWidget()->size().height() + titleBarHeight + 6,
                                                  maximumSize().width(), maximumSize().height());
    rightBackgroundWidget->setMinimumWidth(rightStackedLayout->currentWidget()->size().width() + 16);
    rightBackgroundWidget->setMinimumHeight( rightStackedLayout->currentWidget()->size().height() + titleBarHeight + 6);
    framelessWindowConverter.useTrafficLightsOnMacOS(true);
    framelessWindowConverter.setPosOfGreenTrafficLightOnMacOS(50, 10);
    framelessWindowConverter.setPosOfRedTrafficLightOnMacOS(10, 10);
    framelessWindowConverter.setPosOfYellowTrafficLightOnMacOS(30, 10);

#ifdef __APPLE__
    windowTitle->setAlignment(Qt::AlignRight | Qt::AlignTop);
    windowButtons->hide();
#else
    macOSOption->setEnabled(false);
#endif

    framelessWindowConverter.setBorderWidth(5);

    // Set the parameters
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

    // Convert window
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
                   "border-top-right-radius: 0px;"
                   "Text-align:left;"
                   "padding-left: 12px;"
                   "font-size: 15px;"
                   "color: white;}"
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
