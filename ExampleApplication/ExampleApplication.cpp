#include "ExampleApplication.h"
#include <QPushButton>
#include <QLabel>
#include <QApplication>
#include <QResizeEvent>
#include <QPainter>
#include <QWindow>
#include <QStackedLayout>
#include <QScrollArea>
#include <QSlider>
#include "MinimalScrollBar.h"
#include "ToggleButton.h"
#include "QPropertyAnimation"
#include "ControlHLabel.h"
#include "SettingWidget.h"

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

QPushButton* ExampleApplication::createSettingSelectionButton(const QString& inText, QWidget* inOptionWidget, QLayout* inLayout)
{
    QPushButton* newSettingSelectionButton = new QPushButton;
    newSettingSelectionButton->setText(inText);
    newSettingSelectionButton->setFixedHeight(50);
    newSettingSelectionButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    newSettingSelectionButton->setStyleSheet("QPushButton { background-color:none;"
                                             "border:none;"
                                             "padding:0px;"
                                             "border-top-right-radius: 0px;"
                                             "Text-align:left;"
                                             "padding-left: 12px;"
                                             "font-size: 15px;"
                                             "color: white;}"
                                             "QPushButton:hover{ background-color: lightgrey; border: none; }"
                                             "QPushButton:pressed{ background-color: grey;}");
    connect(newSettingSelectionButton, &QPushButton::clicked, [this, inOptionWidget, newSettingSelectionButton] () {
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
        selectionIndicator->setParent(newSettingSelectionButton);
        selectionIndicator->show();
        update();

        // Resize widget to always show option selection widget after clicking an option
        int minimumWindowWidth = minimumWidth + widthOfLeftBackgroundWidget;
        if(minimumWindowWidth >= windowHandle()->size().width())
        {
            resize(minimumWindowWidth+1, windowHandle()->size().height());
        }
    });

    inLayout->addWidget(newSettingSelectionButton);
    return newSettingSelectionButton;
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

    QScrollArea* leftScrollArea = new QScrollArea;
    leftScrollArea->setFrameShape(QFrame::NoFrame);
    leftScrollArea->viewport()->setStyleSheet("background-color: rgba(0,0,0,0);"
                                              "margin: 0px 4px 0px 0px;");
    leftScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    MinimalScrollBar* minimalScrollBar = new MinimalScrollBar;
    leftScrollArea->setVerticalScrollBar(minimalScrollBar);

    QWidget* containerWidget = new QWidget;
    leftScrollArea->setWidget(containerWidget);
    leftScrollArea->setWidgetResizable(true);

    settingSelectionLayout = new QVBoxLayout(containerWidget);
    settingSelectionLayout->addSpacing(5);
    settingSelectionLayout->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout* leftTitleBar = new QVBoxLayout(leftBackgroundWidget);
    leftTitleBar->setSpacing(0);
    leftTitleBar->addWidget(windowTitle);
    leftTitleBar->addWidget(leftScrollArea); // content
    leftTitleBar->setContentsMargins(0, 0, 0, 0);

    // Create selection buttons
    QWidget* selectionButtonMC = createSettingSelectionButton("Machine Clicker", machineClicker, settingSelectionLayout);
    createSettingSelectionButton("Transparency", transparencyWidget, settingSelectionLayout);
    createSettingSelectionButton("Frameless window", framelessSettingWidget, settingSelectionLayout);
    createSettingSelectionButton("Fullscreen", fullscreenSettingWidget, settingSelectionLayout);
    createSettingSelectionButton("MacOS", macOSSettingWidget, settingSelectionLayout);
    settingSelectionLayout->addStretch(1);

    // Set selection to first
    selectionIndicator->setParent(selectionButtonMC);
    selectionIndicator->show();
}

QWidget* ExampleApplication::createTransparencyWidget()
{
    transparencySwitch = new ToggleButton;
    ControlHLabel* transparencyControl = new ControlHLabel(transparencySwitch);
    SettingWidget* transparentSetting = new SettingWidget("Turn all transparency effects on/off", transparencyControl);
    connect(transparencySwitch, &QAbstractButton::toggled, this, [this](bool checked) {
        if(checked)
        {
            setAttribute(Qt::WA_TranslucentBackground, false);
            setAttribute(Qt::WA_NoSystemBackground, false);

            translucencyBlurEffect.deactivateEffect();
            translucentBlurSwitch->setEnabled(false);
            update();
        }
        else
        {
            setAttribute(Qt::WA_TranslucentBackground, true);
            setAttribute(Qt::WA_NoSystemBackground, true);

            translucentBlurSwitch->setEnabled(true);
            if(!translucentBlurSwitch->isChecked())
                translucencyBlurEffect.reactivateEffect();

            update();
        }
    });

    QSlider* windowOpacitySlider = new QSlider(Qt::Horizontal);
    windowOpacitySlider->setMinimum(0);
    windowOpacitySlider->setMaximum(100);
    windowOpacitySlider->setFixedWidth(220);
    ControlHLabel* opacityControl = new ControlHLabel(windowOpacitySlider, false, "100");
    connect(windowOpacitySlider, &QSlider::valueChanged, this, [this, opacityControl](int value) {
        windowOpacity = value;
        opacityControl->getLabel()->setText(QString::number(value));
        update();
    });
    windowOpacitySlider->setValue(windowOpacity);
    SettingWidget* opacitySetting = new SettingWidget("Window opacity", opacityControl);

    translucentBlurSwitch = new ToggleButton;
    ControlHLabel* translucentControl = new ControlHLabel(translucentBlurSwitch);
    SettingWidget* translucentBlurSetting = new SettingWidget("Turn the translucent blur effect on/off", translucentControl);
    connect(translucentBlurSwitch, &QAbstractButton::toggled, this, [this]() {
        if(translucentBlurSwitch->isChecked())
            translucencyBlurEffect.deactivateEffect();
        else translucencyBlurEffect.reactivateEffect();
    });

    QSlider* translucentBlurStrengthSlider = new QSlider(Qt::Horizontal);
    translucentBlurStrengthSlider->setMinimum(0);
    translucentBlurStrengthSlider->setMaximum(40);
    translucentBlurStrengthSlider->setFixedWidth(220);
    ControlHLabel* blurSliderControl = new ControlHLabel(translucentBlurStrengthSlider, false, "100");
    connect(translucentBlurStrengthSlider, &QSlider::valueChanged, this, [this, blurSliderControl](int value) {
        translucencyBlurEffect.setBlurStrength(value);
        blurSliderControl->getLabel()->setText(QString::number(value));
        update();
    });
    translucentBlurStrengthSlider->setValue(translucencyBlurEffect.getBlurStrength());
    SettingWidget* blurSliderSetting = new SettingWidget("Translucent blur strength", blurSliderControl);

    transparencyWidget = new QWidget;
    QVBoxLayout* transparencyOptionLayout = new QVBoxLayout(transparencyWidget);
    transparencyOptionLayout->addWidget(transparentSetting);
    transparencyOptionLayout->addWidget(opacitySetting);
    transparencyOptionLayout->addWidget(translucentBlurSetting);
    transparencyOptionLayout->addWidget(blurSliderSetting);
    transparencyOptionLayout->addStretch(1);

    return transparencyWidget;
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
    rightTitleBarSpacer = new QSpacerItem(0, 0);
    rightTitleBar->addSpacerItem(rightTitleBarSpacer);
    rightTitleBar->addSpacing(5);

    // Create MachineClicker widget
    machineClicker = new MachineClicker;
    transparencyWidget = createTransparencyWidget();

    // Create frameless setting widget
    ToggleButton* framelessSwitch = new ToggleButton;
    ControlHLabel* framelessControl = new ControlHLabel(framelessSwitch);
    framelessSettingWidget = new SettingWidget("Convert window to a frameless native window", framelessControl);
    connect(framelessSwitch, &QAbstractButton::toggled, this, [this](bool checked) {
        if(checked)
        {
            framelessWindowConverter.convertToWindowWithFrame();
            // Override window flags (do not set FramelessWindowHint)
            setWindowFlags(Qt::Widget | Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowMinMaxButtonsHint
                           | Qt::WindowFullscreenButtonHint);

            // On Windows Qt::FramelessWindowHint needs to be set for translucency
            // Disable all transparency effects on all platforms when not frameless
            setAttribute(Qt::WA_TranslucentBackground, false);
            setAttribute(Qt::WA_NoSystemBackground, false);
            transparencySwitch->setEnabled(false);
            // This effect would still work but there is no quick way to hide the window anymore
            translucencyBlurEffect.deactivateEffect();
            translucentBlurSwitch->setEnabled(false);
            show();

            // No need for custom window buttons and title
            windowButtons->hide();
            rightTitleBarSpacer->changeSize(0, titleBarHeight);
            windowTitle->hide();

            // This would just maximize the window -> disable it
            fullscreenSwitch->setEnabled(false);
        }
        else
        {
            // NoDropShadowWindowHint is needed to get proper shadow...
            setWindowFlags(Qt::Widget | Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
            transparencySwitch->setEnabled(true);

            if(!transparencySwitch->isChecked())
            {
                setAttribute(Qt::WA_TranslucentBackground, true);
                setAttribute(Qt::WA_NoSystemBackground, true);
                translucentBlurSwitch->setEnabled(true);

                if(!translucentBlurSwitch->isChecked())
                    translucencyBlurEffect.reactivateEffect();
            }
            show();

            // Convert window
            framelessWindowConverter.convertWindowToFrameless(fwcParams);
            show();
            // Show custom window buttons and title
            if(!framelessWindowConverter.isUsingTrafficLightsOnMacOS())
            {
                rightTitleBarSpacer->changeSize(0, 0);
                windowButtons->show();
            }
            windowTitle->show();

            fullscreenSwitch->setEnabled(true);
        }
    });

    // Create macOS setting widget
    ToggleButton* trafficLightSwitch = new ToggleButton;
    ControlHLabel* trafficLightControl = new ControlHLabel(trafficLightSwitch);
    macOSSettingWidget = new SettingWidget("Use original traffic light buttons", trafficLightControl);
    connect(trafficLightSwitch, &QAbstractButton::toggled, this, [this, trafficLightSwitch]() {
        if(trafficLightSwitch->isChecked())
        {
            framelessWindowConverter.useTrafficLightsOnMacOS(false);
            windowTitle->setAlignment(Qt::AlignLeft | Qt::AlignTop);
            windowTitle->setContentsMargins(5,10,0,0);
            framelessWindowConverter.hideForTranslucency();
            rightTitleBarSpacer->changeSize(0, 0);
            windowButtons->show();
        }
        else
        {
            framelessWindowConverter.useTrafficLightsOnMacOS(true);
            windowTitle->setAlignment(Qt::AlignRight | Qt::AlignTop);
            windowTitle->setContentsMargins(0,10,10,0);
            framelessWindowConverter.showForTranslucency();
            rightTitleBarSpacer->changeSize(0, titleBarHeight);
            windowButtons->hide();
        }
    });

    // Create fullscreen setting widget
    fullscreenSwitch = new ToggleButton;
    fullscreenSwitch->toggle();
    fullscreenSettingWidget = new SettingWidget("Toggle frameless fullscreen window", new ControlHLabel(fullscreenSwitch));
    connect(fullscreenSwitch, &QAbstractButton::toggled, this, [this]() {
        framelessWindowConverter.toggleFullscreen();
    });

    rightStackedLayout = new QStackedLayout;
    rightStackedLayout->addWidget(machineClicker);
    rightStackedLayout->addWidget(transparencyWidget);
    rightStackedLayout->addWidget(framelessSettingWidget);
    rightStackedLayout->addWidget(fullscreenSettingWidget);
    rightStackedLayout->addWidget(macOSSettingWidget);

    machineClicker->layout()->setContentsMargins(8, 5, 8, 8);
    rightTitleBar->addLayout(rightStackedLayout);
    rightTitleBar->setContentsMargins(0, 0, 0, 0);
}

void ExampleApplication::setupFramelessWindow()
{
    setWindowFlags(Qt::Widget | Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_NoSystemBackground, true);

    setWindowTitle("Example Application");

    // Only necessary on macOS to hide Traffic Lights when they are used
    // Otherwise empty methods are called and hopefully optimized away
    connect(&translucencyBlurEffect, &TranslucentBlurEffect::hideNonQtWidgets, [this]() { framelessWindowConverter.hideForTranslucency(); });
    connect(&translucencyBlurEffect, &TranslucentBlurEffect::showNonQtWidgets, [this]() { framelessWindowConverter.showForTranslucency(); });

    framelessWindowConverter.setEnableShadow(false);

    // Set some settings
    adjustSize(); // apply layout size (constraints) to window
    rightStackedLayout->currentWidget()->adjustSize();
    framelessWindowConverter.setMinMaxWindowSizes(rightStackedLayout->currentWidget()->size().width() + 16,
                                                  rightStackedLayout->currentWidget()->size().height() + titleBarHeight + 6,
                                                  maximumSize().width(), maximumSize().height());
    rightBackgroundWidget->setMinimumWidth(rightStackedLayout->currentWidget()->size().width() + 16);
    rightBackgroundWidget->setMinimumHeight(rightStackedLayout->currentWidget()->size().height() + titleBarHeight + 6);

    framelessWindowConverter.setPosOfGreenTrafficLightOnMacOS(50, 10);
    framelessWindowConverter.setPosOfRedTrafficLightOnMacOS(10, 10);
    framelessWindowConverter.setPosOfYellowTrafficLightOnMacOS(30, 10);

#ifdef __APPLE__
    framelessWindowConverter.useTrafficLightsOnMacOS(true);
    windowTitle->setAlignment(Qt::AlignRight | Qt::AlignTop);
    windowTitle->setContentsMargins(0,10,10,0);
    windowButtons->hide();
    rightTitleBarSpacer->changeSize(0, titleBarHeight);
#else
    macOSSettingWidget->setEnabled(false);
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
            {
                fullscreenSwitch->setChecked(true);
                framelessWindowConverter.restoreWindow();
            }
            else
                framelessWindowConverter.maximizeWindow();
        }
    });
}

 bool ExampleApplication::event(QEvent* event)
 {
     switch(event->type())
     {
     case QEvent::WindowStateChange:
     {
         // When user drags title bar check fullscreen switch
         QWindowStateChangeEvent* stateEvent = static_cast<QWindowStateChangeEvent*>(event);
         if (!(windowState() & Qt::WindowMaximized) && (stateEvent->oldState() & Qt::WindowMaximized))
             fullscreenSwitch->setChecked(true);
         break;
     }
     default:
         break;
     }

     return QWidget::event(event);
 }

bool ExampleApplication::nativeEvent(const QByteArray& eventType, void* message, long* result)
{
    Q_UNUSED(eventType)
    return framelessWindowConverter.filterNativeEvents(message, result);
}

void ExampleApplication::resizeEvent(QResizeEvent* ev)
{
    // Show/Hide left background widget if the window size is too small
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
    // Color the blurred background
    QPainter painter(this);
    QColor color(0,0,0);

    color.setAlpha(255);
    painter.setOpacity(static_cast<double>(windowOpacity) / 100.0);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.fillRect(ev->rect(),color);
}
