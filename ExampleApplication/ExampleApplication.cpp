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
#include <QFormLayout>
#include <QCheckBox>
#include <QLineEdit>
#include <QGraphicsDropShadowEffect>
#include "MinimalScrollBar.h"
#include "ToggleButton.h"
#include "QPropertyAnimation"
#include "ControlHLabel.h"
#include "LabelVControl.h"
#include "TransparentBorderWidget.h"

ExampleApplication::ExampleApplication(FWC::FramelessWindowConverter* inFramelessWindowConverter, QWidget *parent) : QWidget(parent),
    translucencyBlurEffect(this, this), framelessWindowConverter(inFramelessWindowConverter)
{
    qApp->installEventFilter(this);

    // Set total title bar height
    setVisibleTitleBarHeight(titleBarHeight);
    framelessWindowConverter->setBorderWidth(static_cast<TransparentBorderWidget*>(parentWidget())->getBorderWidth() + 8);

    setStyleSheet("QSpinBox { background: black; color: white; font-size: 16px; }"
                  "QSpinBox:disabled { background: grey; }"
                  "QLineEdit { background: black; color: white; }");

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

    // Enable/Disable window drop shadow (enabling it disables all transparency effects)
    bool hasWindowDropShadow = false;

    // setWindowFlags(Qt::Widget | Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_NoSystemBackground, true);

    setupFramelessWindow();
}

void ExampleApplication::setVisibleTitleBarHeight(int inVisibleTitleBarHeight)
{
    titleBarHeight = inVisibleTitleBarHeight + static_cast<TransparentBorderWidget*>(parentWidget())->getBorderWidth();
}

void ExampleApplication::setupFramelessWindow()
{
    framelessWindowConverter->setShouldPerformWindowDrag([this](int mousePosXInWindow, int mousePosYInWindow)
    {
        QWidget* widgetUnderCursor = childAt(mousePosXInWindow, mousePosYInWindow);
        // Set all background widgets draggable
        if((widgetUnderCursor == nullptr ||  widgetUnderCursor == rightBackgroundWidget ||  widgetUnderCursor == leftBackgroundWidget
            || widgetUnderCursor == machineClicker || widgetUnderCursor == windowTitle || widgetUnderCursor == windowButtons) &&
                (mousePosYInWindow < titleBarHeight))
            return true;
        else return false;
    });

    // Only necessary on macOS to hide Traffic Lights when they are used
    // Otherwise empty methods are called and hopefully optimized away
    connect(&translucencyBlurEffect, &TranslucentBlurEffect::hideNonQtWidgets, [this]() { framelessWindowConverter->hideForTranslucency(); });
    connect(&translucencyBlurEffect, &TranslucentBlurEffect::showNonQtWidgets, [this]() { framelessWindowConverter->showForTranslucency(); });

    // Set some settings
    adjustSize(); // apply layout size (constraints) to window
    rightStackedLayout->currentWidget()->adjustSize();
    setMinMaxWindowSizesAndResizeIfNeeded();

#ifdef __APPLE__
    framelessWindowConverter->useTrafficLightsOnMacOS(true);
    windowTitle->setAlignment(Qt::AlignRight | Qt::AlignTop);
    windowTitle->setContentsMargins(0,10,10,0);
    windowButtons->hide();
    framelessWindowConverter->setUpperLeftXPositionOfTrafficLightsOnMacOS(xUpperLeftOfTrafficLights + 10);
    framelessWindowConverter->setUpperLeftYPositionOfTrafficLightsOnMacOS(yUpperLeftOfTrafficLights + 10);
#else
    macOSWidget->setEnabled(false);
#endif

    // Connect custom System Buttons
    connect(windowButtons->getCloseButton(), &QAbstractButton::clicked, this, [this]()
    {
        framelessWindowConverter->closeWindow();
    });
    connect(windowButtons->getMinimizeButton(), &QAbstractButton::clicked, this, [this]()
    {
        framelessWindowConverter->minimizeWindow();
    });
    connect(windowButtons->getMaximizeButton(), &QAbstractButton::clicked, this, [this]()
    {
#ifdef __APPLE__
        if(!(qApp->keyboardModifiers() & Qt::AltModifier) || window()->isFullScreen())
        {
            framelessWindowConverter->toggleFullscreen();
        }
        else
#endif
        {
            if(window()->isMaximized())
            {
                fullscreenSwitch->setChecked(true);
                framelessWindowConverter->restoreWindow();
            }
            else framelessWindowConverter->maximizeWindow();
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
        if (!(windowState() & Qt::WindowFullScreen) && (stateEvent->oldState() & Qt::WindowFullScreen))
            fullscreenSwitch->setChecked(true);
        break;
    }
    default:
        break;
    }

    return QWidget::event(event);
}

void ExampleApplication::dynamicallyShowHidetLeftWidgetBasedOnSize(int windowWidth)
{
    // Show/Hide left background widget if the window size is too small
    if(!bAlwaysShowSettings && !bNeverShowSettings)
    {
        int transparentWindowBorderWidth = static_cast<TransparentBorderWidget*>(parentWidget())->getBorderWidth() * 2;
        // Hide left widget if current window width is at most one pixel bigger than the minimum window width
        if(!leftBackgroundWidget->isHidden() && windowWidth+1 <= (framelessWindowConverter->getMinimumWindowWidth() + widthOfLeftBackgroundWidget - transparentWindowBorderWidth))
        {
            leftBackgroundWidget->hide();
        }
        // Show left widget if current window width is at least one pixel bigger than the minimum window width
        else if(leftBackgroundWidget->isHidden() && windowWidth+1 > (framelessWindowConverter->getMinimumWindowWidth() + widthOfLeftBackgroundWidget - transparentWindowBorderWidth))
        {
            leftBackgroundWidget->show();
        }
    }
}

void ExampleApplication::resizeEvent(QResizeEvent* ev)
{
    if(ev->oldSize().width() != -1)
        dynamicallyShowHidetLeftWidgetBasedOnSize(ev->size().width());
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

void ExampleApplication::setMinMaxWindowSizesAndResizeIfNeeded()
{
    // Set new window size limits
    // +16 and +6 for contents margins
    int minimumWidth = 0, minimumHeight = 0;
    int differenceParentThisWidth = static_cast<TransparentBorderWidget*>(parentWidget())->getBorderWidth() * 2;
    QWidget* currentRightWidget = rightStackedLayout->currentWidget();
    if(currentRightWidget->layout())
    {
        minimumWidth = currentRightWidget->layout()->minimumSize().width() + 16;
        minimumHeight = currentRightWidget->layout()->minimumSize().height() + titleBarHeight + 6;
    }
    else
    {
        minimumWidth = currentRightWidget->minimumWidth() + 16;
        minimumHeight = currentRightWidget->minimumHeight() + titleBarHeight + 6;
    }

    int minimumWindowWidth = minimumWidth;
    if(bAlwaysShowSettings && !bNeverShowSettings)
        minimumWindowWidth += widthOfLeftBackgroundWidget;

    framelessWindowConverter->setMinMaxWindowSizes(minimumWindowWidth + differenceParentThisWidth, minimumHeight + differenceParentThisWidth, maximumSize().width(), maximumSize().height());
    rightBackgroundWidget->setMinimumWidth(minimumWidth);
    rightBackgroundWidget->setMinimumHeight(minimumHeight);
    update();
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
        selectionIndicator->setParent(newSettingSelectionButton);
        selectionIndicator->show();
        setMinMaxWindowSizesAndResizeIfNeeded();
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
    windowTitle->setContentsMargins(5, 5, 5, 5);

    selectionIndicator = new QWidget;
    selectionIndicator->setFixedSize(10, 24);
    selectionIndicator->setStyleSheet("background-color: #0078d7;");
    selectionIndicator->move(0, 13);

    QScrollArea* leftScrollArea = new QScrollArea;
    leftScrollArea->setFrameShape(QFrame::NoFrame);
    leftScrollArea->viewport()->setStyleSheet("background-color: rgba(0,0,0,0); margin: 0px 4px 0px 0px;");
    leftScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    MinimalScrollBar* minimalScrollBar = new MinimalScrollBar;
    leftScrollArea->setVerticalScrollBar(minimalScrollBar);

    QWidget* containerWidget = new QWidget;
    leftScrollArea->setWidget(containerWidget);
    leftScrollArea->setWidgetResizable(true);

    settingSelectionLayout = new QVBoxLayout(containerWidget);
    settingSelectionLayout->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout* leftTitleBar = new QVBoxLayout(leftBackgroundWidget);
    leftTitleBar->setSpacing(0);
    leftTitleBar->addWidget(windowTitle);
    leftTitleBar->addSpacing(15);
    leftTitleBar->addWidget(leftScrollArea); // content
    leftTitleBar->setContentsMargins(0, 0, 0, 0);

    // Create selection buttons
    QWidget* selectionButtonMC = createSettingSelectionButton("Machine Clicker", machineClicker, settingSelectionLayout);
    createSettingSelectionButton("Transparency", transparencyWidget, settingSelectionLayout);
    createSettingSelectionButton("Frameless window", framelessWidget, settingSelectionLayout);
    createSettingSelectionButton("Fullscreen", fullscreenWidget, settingSelectionLayout);
    createSettingSelectionButton("MacOS", macOSWidget, settingSelectionLayout);
    settingSelectionLayout->addStretch(1);

    // Set selection to first
    selectionIndicator->setParent(selectionButtonMC);
    selectionIndicator->show();
}

QWidget* ExampleApplication::createTransparencyWidget()
{
    transparencySwitch = new ToggleButton;
    ControlHLabel* transparencyControl = new ControlHLabel(transparencySwitch);
    LabelVControl* transparentSetting = new LabelVControl("Turn all transparency effects on/off", transparencyControl);
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
    LabelVControl* opacitySetting = new LabelVControl("Window opacity", opacityControl);

    translucentBlurSwitch = new ToggleButton;
    ControlHLabel* translucentControl = new ControlHLabel(translucentBlurSwitch);
    LabelVControl* translucentBlurSetting = new LabelVControl("Turn the translucent blur effect on/off", translucentControl);
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
    LabelVControl* blurSliderSetting = new LabelVControl("Translucent blur strength", blurSliderControl);

    transparencyWidget = new QWidget;
    QVBoxLayout* transparencyOptionLayout = new QVBoxLayout(transparencyWidget);
    transparencyOptionLayout->addWidget(transparentSetting);
    transparencyOptionLayout->addWidget(opacitySetting);
    transparencyOptionLayout->addWidget(translucentBlurSetting);
    transparencyOptionLayout->addWidget(blurSliderSetting);
    transparencyOptionLayout->addStretch(1);

    return transparencyWidget;
}

QWidget* ExampleApplication::createFramelessWidget()
{
    // Frameless switch
    ToggleButton* framelessSwitch = new ToggleButton;
    ControlHLabel* framelessControl = new ControlHLabel(framelessSwitch);
    LabelVControl* framelessSettingWidget = new LabelVControl("Convert window to a frameless native window", framelessControl);
    connect(framelessSwitch, &QAbstractButton::toggled, this, [this](bool checked) {
        if(checked)
        {
            framelessWindowConverter->toggleWindowFrameAfterConversion();
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
            framelessWindowConverter->toggleWindowFrameAfterConversion();
            show();
            // Show custom window buttons and title
            if(!framelessWindowConverter->isUsingTrafficLightsOnMacOS())
            {
                windowButtons->show();
            }
            windowTitle->show();

            fullscreenSwitch->setEnabled(true);
        }
    });

    // Resizing border width
    QSpinBox* borderWidthSpinBox = new QSpinBox;
    borderWidthSpinBox->setMinimum(0);
    borderWidthSpinBox->setMaximum(100);
    borderWidthSpinBox->setValue(framelessWindowConverter->getBorderWidth());
    borderWidthSpinBox->setMinimumSize(35, 25);
    borderWidthSpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(borderWidthSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [this](int value) {
        framelessWindowConverter->setBorderWidth(value);
    });
    QWidget* borderWidthWidget = new QWidget;
    borderWidthWidget->setStyleSheet("QLabel { margin-top: 1px; background-color : none; color : white; font-size: 15px; }");
    QFormLayout* borderWidthLayout = new QFormLayout(borderWidthWidget);
    borderWidthLayout->addRow("Border width:", borderWidthSpinBox);
    LabelVControl* borderWidthSetting = new LabelVControl("Change window border width where resizing is allowed", borderWidthWidget);
    borderWidthLayout->setFormAlignment(Qt::AlignLeft);

    // Title bar height
    QSpinBox* titleBarHeightSpinBox = new QSpinBox;
    titleBarHeightSpinBox->setMinimum(windowButtons->getCloseButton()->minimumHeight());
    titleBarHeightSpinBox->setMaximum(1000);
    titleBarHeightSpinBox->setValue(titleBarHeight);
    titleBarHeightSpinBox->setMinimumSize(35, 25);
    titleBarHeightSpinBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(titleBarHeightSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [this, titleBarHeightSpinBox](int value) {
        if(titleBarHeightSpinBox->isEnabled())
        {
            framelessWindowConverter->setShouldPerformWindowDrag([this, value](int mousePosXInWindow, int mousePosYInWindow)
            {
                QWidget* widgetUnderCursor = childAt(mousePosXInWindow, mousePosYInWindow);
                // Set all background widgets draggable
                if((widgetUnderCursor == nullptr ||  widgetUnderCursor == rightBackgroundWidget ||  widgetUnderCursor == leftBackgroundWidget
                    || widgetUnderCursor == machineClicker || widgetUnderCursor == windowTitle || widgetUnderCursor == windowButtons) &&
                        (mousePosYInWindow < value))
                    return true;
                else return false;
            });

            rightTitleBarSpacer->changeSize(0, value - windowButtons->getCloseButton()->minimumHeight());
            rightStackedLayout->update();
            setVisibleTitleBarHeight(value);
            setMinMaxWindowSizesAndResizeIfNeeded();
        }
    });
    QWidget* titleBarHeightWidget = new QWidget;
    titleBarHeightWidget->setStyleSheet("QLabel { margin-top: 1px; background-color : none; color : white; font-size: 15px; }");
    QFormLayout* titleBarHeightLayout = new QFormLayout(titleBarHeightWidget);
    titleBarHeightLayout->addRow("Title bar height:", titleBarHeightSpinBox);
    titleBarHeightLayout->setContentsMargins(0, 0, 0, 0);
    titleBarHeightLayout->setFormAlignment(Qt::AlignLeft);

    // Movable background switch
    ToggleButton* backgroundMoveSwitch = new ToggleButton;
    ControlHLabel* backgroundMoveControl = new ControlHLabel(backgroundMoveSwitch);
    backgroundMoveSwitch->setChecked(true);
    QLabel* backgroundMoveLabel = new QLabel("Movable by entire Background:");
    backgroundMoveLabel->setStyleSheet("QLabel { margin-top: 1px; background-color : none; color : white; font-size: 15px; }");

    QHBoxLayout* backgroundMoveLayout = new QHBoxLayout;
    backgroundMoveLayout->addWidget(backgroundMoveLabel);
    backgroundMoveLayout->addWidget(backgroundMoveControl);
    backgroundMoveLayout->addStretch(1);
    backgroundMoveLayout->setContentsMargins(0, 0, 0, 0);
    connect(backgroundMoveSwitch, &QCheckBox::toggled, this, [this, titleBarHeightSpinBox](bool inChecked) {
        if(!inChecked)
        {
            framelessWindowConverter->setShouldPerformWindowDrag([this](int mousePosXInWindow, int mousePosYInWindow)
            {
                QWidget* widgetUnderCursor = childAt(mousePosXInWindow, mousePosYInWindow);
                // Set all background widgets draggable
                if(widgetUnderCursor == windowButtons->getCloseButton() || widgetUnderCursor == windowButtons->getMinimizeButton() || widgetUnderCursor == windowButtons->getMaximizeButton()
                        || qobject_cast<ToggleButton*>(widgetUnderCursor) != nullptr || qobject_cast<QPushButton*>(widgetUnderCursor) != nullptr
                        || qobject_cast<QSpinBox*>(widgetUnderCursor) != nullptr || qobject_cast<QSlider*>(widgetUnderCursor) != nullptr
                        || qobject_cast<QCheckBox*>(widgetUnderCursor) != nullptr || qobject_cast<QLineEdit*>(widgetUnderCursor) != nullptr)
                {
                    return false;
                }
                else return true;
            });

            titleBarHeightSpinBox->setEnabled(false);
        }
        else
        {
            framelessWindowConverter->setShouldPerformWindowDrag([this](int mousePosXInWindow, int mousePosYInWindow)
            {
                QWidget* widgetUnderCursor = childAt(mousePosXInWindow, mousePosYInWindow);
                // Set all background widgets draggable
                if((widgetUnderCursor == nullptr ||  widgetUnderCursor == rightBackgroundWidget ||  widgetUnderCursor == leftBackgroundWidget
                    || widgetUnderCursor == machineClicker || widgetUnderCursor == windowTitle || widgetUnderCursor == windowButtons) &&
                        (mousePosYInWindow < titleBarHeight))
                    return true;
                else return false;
            });
            titleBarHeightSpinBox->setEnabled(true);
        }
    });

    // Move drag area widget
    QWidget* moveDragAreaWidget = new QWidget;
    QVBoxLayout* moveDragAreaLayout = new QVBoxLayout(moveDragAreaWidget);
    moveDragAreaLayout->addLayout(backgroundMoveLayout);
    moveDragAreaLayout->addSpacing(5);
    moveDragAreaLayout->addWidget(titleBarHeightWidget);
    moveDragAreaLayout->addStretch(1);
    LabelVControl* moveDragAreaSetting = new LabelVControl("Move drag area settings", moveDragAreaWidget);

    // Create frameless widget
    QWidget* framelessWidget = new QWidget;
    QVBoxLayout* framelessWidgetLayout = new QVBoxLayout(framelessWidget);
    framelessWidgetLayout->addWidget(framelessSettingWidget);
    framelessWidgetLayout->addWidget(borderWidthSetting);
    framelessWidgetLayout->addWidget(moveDragAreaSetting);
    framelessWidgetLayout->addStretch(1);

    return framelessWidget;
}

QWidget* ExampleApplication::createMacOSWidget()
{
    ToggleButton* trafficLightSwitch = new ToggleButton;
    ControlHLabel* trafficLightControl = new ControlHLabel(trafficLightSwitch);
    LabelVControl* trafficLightSettingWidget = new LabelVControl("Use original traffic light buttons", trafficLightControl);
    connect(trafficLightSwitch, &QAbstractButton::toggled, this, [this, trafficLightSwitch]() {
        if(trafficLightSwitch->isChecked())
        {
            framelessWindowConverter->useTrafficLightsOnMacOS(false);
            windowTitle->setAlignment(Qt::AlignLeft | Qt::AlignTop);
            windowTitle->setContentsMargins(5,10,0,0);
            framelessWindowConverter->hideForTranslucency();
            windowButtons->show();
        }
        else
        {
            framelessWindowConverter->useTrafficLightsOnMacOS(true);
            windowTitle->setAlignment(Qt::AlignRight | Qt::AlignTop);
            windowTitle->setContentsMargins(0,10,10,0);
            framelessWindowConverter->showForTranslucency();
            windowButtons->hide();
        }
    });

    // Hidden buttons
    ToggleButton* greenHiddenSwitch = new ToggleButton;
    ControlHLabel* greenHiddenControl = new ControlHLabel(greenHiddenSwitch, false, "Green");
    connect(greenHiddenSwitch, &QAbstractButton::toggled, this, [this](bool inChecked) {
        framelessWindowConverter->setHiddenGreenTrafficLightOnMacOS(inChecked);
    });

    ToggleButton* redHiddenSwitch = new ToggleButton;
    ControlHLabel* redHiddenControl = new ControlHLabel(redHiddenSwitch, false, "Red");
    connect(redHiddenSwitch, &QAbstractButton::toggled, this, [this](bool inChecked) {
        framelessWindowConverter->setHiddenRedTrafficLightOnMacOS(inChecked);
    });

    ToggleButton* yellowHiddenSwitch = new ToggleButton;
    ControlHLabel* yellowHiddenControl = new ControlHLabel(yellowHiddenSwitch, false, "Yellow");
    connect(yellowHiddenSwitch, &QAbstractButton::toggled, this, [this](bool inChecked) {
        framelessWindowConverter->setHiddenYellowTrafficLightOnMacOS(inChecked);
    });

    QHBoxLayout* hiddenControlsLayout = new QHBoxLayout;
    hiddenControlsLayout->addWidget(redHiddenControl);
    hiddenControlsLayout->addWidget(yellowHiddenControl);
    hiddenControlsLayout->addWidget(greenHiddenControl);
    hiddenControlsLayout->addStretch(1);
    hiddenControlsLayout->setContentsMargins(0, 0, 0, 0);
    QWidget* hiddenControlsWidget = new QWidget;
    hiddenControlsWidget->setLayout(hiddenControlsLayout);
    LabelVControl* hiddenSettingWidget = new LabelVControl("Show/Hide traffic lights", hiddenControlsWidget);

    // Enable buttons
    ToggleButton* greenEnabledSwitch = new ToggleButton;
    ControlHLabel* greenEnabledControl = new ControlHLabel(greenEnabledSwitch, false, "Green");
    connect(greenEnabledSwitch, &QAbstractButton::toggled, this, [this](bool inChecked) {
        framelessWindowConverter->setEnabledGreenTrafficLightOnMacOS(!inChecked);
    });

    ToggleButton* redEnabledSwitch = new ToggleButton;
    ControlHLabel* redEnabledControl = new ControlHLabel(redEnabledSwitch, false, "Red");
    connect(redEnabledSwitch, &QAbstractButton::toggled, this, [this](bool inChecked) {
        framelessWindowConverter->setEnabledRedTrafficLightOnMacOS(!inChecked);
    });

    ToggleButton* yellowEnabledSwitch = new ToggleButton;
    ControlHLabel* yellowEnabledControl = new ControlHLabel(yellowEnabledSwitch, false, "Yellow");
    connect(yellowEnabledSwitch, &QAbstractButton::toggled, this, [this](bool inChecked) {
        framelessWindowConverter->setEnabledYellowTrafficLightOnMacOS(!inChecked);
    });

    QHBoxLayout* enabledControlsLayout = new QHBoxLayout;
    enabledControlsLayout->addWidget(redEnabledControl);
    enabledControlsLayout->addWidget(yellowEnabledControl);
    enabledControlsLayout->addWidget(greenEnabledControl);
    enabledControlsLayout->addStretch(1);
    enabledControlsLayout->setContentsMargins(0, 0, 0, 0);
    QWidget* enabledControlsWidget = new QWidget;
    enabledControlsWidget->setLayout(enabledControlsLayout);
    LabelVControl* enabledSettingWidget = new LabelVControl("Enable/Disable traffic lights", enabledControlsWidget);

    // Position and alignment of traffic lights
    QWidget* positionWidget = new QWidget;
    positionWidget->setStyleSheet("QLabel { margin-top: 1px; background-color : none; color : white; font-size: 15px; }");
    positionWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QHBoxLayout* positionMainLayout = new QHBoxLayout;
    positionMainLayout->setContentsMargins(0, 0, 5, 0);
    QVBoxLayout* posAlignmentLayout = new QVBoxLayout(positionWidget);
    posAlignmentLayout->setContentsMargins(5, 0, 0, 0);

    QCheckBox* horizontalAlignmentCheck = new QCheckBox;
    horizontalAlignmentCheck->setCheckState(Qt::CheckState::Checked);
    QLabel* horizontalAlignmentLabel = new QLabel("Horizontal alignment");
    QHBoxLayout* horizontalAlignmentLayout = new QHBoxLayout;
    horizontalAlignmentLayout->addWidget(horizontalAlignmentCheck);
    horizontalAlignmentLayout->addWidget(horizontalAlignmentLabel);
    horizontalAlignmentLayout->addStretch(1);
    horizontalAlignmentLayout->setContentsMargins(7, 0, 0, 0);
    connect(horizontalAlignmentCheck, &QCheckBox::stateChanged, this, [this](int state) {
        if(state == Qt::CheckState::Checked)
            framelessWindowConverter->setHorizontalAlignmentOfTrafficLightsOnMacOS(true);
        else framelessWindowConverter->setHorizontalAlignmentOfTrafficLightsOnMacOS(false);
    });
    posAlignmentLayout->addLayout(horizontalAlignmentLayout);
    posAlignmentLayout->addLayout(positionMainLayout);

    QSpinBox* xSpinBox = new QSpinBox;
    xSpinBox->setMinimum(0);
    xSpinBox->setMaximum(10000);
    xSpinBox->setValue(xUpperLeftOfTrafficLights);
    xSpinBox->setMinimumSize(35, 25);
    connect(xSpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [this](int value) {
        framelessWindowConverter->setUpperLeftXPositionOfTrafficLightsOnMacOS(value);
    });
    QFormLayout* xLayout = new QFormLayout;
    xLayout->addRow("X:", xSpinBox);
    positionMainLayout->addLayout(xLayout);

    QSpinBox* ySpinBox = new QSpinBox;
    ySpinBox->setMinimum(0);
    ySpinBox->setMaximum(10000);
    ySpinBox->setValue(yUpperLeftOfTrafficLights);
    ySpinBox->setMinimumSize(35, 25);
    connect(ySpinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [this](int value) {
        framelessWindowConverter->setUpperLeftYPositionOfTrafficLightsOnMacOS(value);
    });
    QFormLayout* yLayout = new QFormLayout;
    yLayout->addRow("Y:", ySpinBox);
    positionMainLayout->addLayout(yLayout);
    LabelVControl* positionSettingWidget = new LabelVControl("Upper left position of the traffic lights and alignment", positionWidget);

    // Create the macOS widget
    macOSWidget = new QWidget;
    QVBoxLayout* macOSWidgetLayout = new QVBoxLayout(macOSWidget);
    macOSWidgetLayout->addWidget(trafficLightSettingWidget);
    macOSWidgetLayout->addWidget(hiddenSettingWidget);
    macOSWidgetLayout->addWidget(enabledSettingWidget);
    macOSWidgetLayout->addWidget(positionSettingWidget);
    macOSWidgetLayout->addStretch(1);
    macOSWidgetLayout->setContentsMargins(0, 0, 0, 0);

    return macOSWidget;
}

void ExampleApplication::createRightSideWidgets()
{
    windowButtons = new WindowButtons(titleBarHeight);

    settingsButton = new QPushButton;
    settingsButton->setFixedSize(titleBarHeight, titleBarHeight);
    settingsButton->setCheckable(true);
    settingsButton->setStyleSheet(WindowButtons::getStyleSheetString(":/images/icon_settings_light.png", "grey", "#0078d7") +
                                  "QPushButton { padding: 3px; }"
                                  "QPushButton:checked{"
                                  "image:url(:/images/icon_settings_enabled_light.png);"
                                  "}");
    connect(settingsButton, &QPushButton::toggled, this, [this](bool inChecked){
        bAlwaysShowSettings = inChecked;
        if(bAlwaysShowSettings) leftBackgroundWidget->show();
        setMinMaxWindowSizesAndResizeIfNeeded();
    });
    QCheckBox* settingsEnabledCheck = new QCheckBox;
    settingsEnabledCheck->setCheckState(Qt::CheckState::Checked);
    connect(settingsEnabledCheck, &QCheckBox::stateChanged, this, [this](int inNewState){
        if(inNewState == Qt::CheckState::Unchecked)
        {
            bNeverShowSettings = true;
            leftBackgroundWidget->hide();
            setMinMaxWindowSizesAndResizeIfNeeded();
            settingsButton->setChecked(false);
            settingsButton->setEnabled(false);
        }
        else
        {
            bNeverShowSettings = false;
            dynamicallyShowHidetLeftWidgetBasedOnSize(window()->width());
            setMinMaxWindowSizesAndResizeIfNeeded();
            settingsButton->setEnabled(true);
        }
    });

    QHBoxLayout* titleBarLayout = new QHBoxLayout;
    titleBarLayout->addSpacing(5);
    titleBarLayout->addWidget(settingsButton);
    titleBarLayout->addSpacing(2);
    titleBarLayout->addWidget(settingsEnabledCheck);
    titleBarLayout->addStretch(1);
    titleBarLayout->addWidget(windowButtons);
    titleBarLayout->setContentsMargins(1,1,1,1);

    // Right Background Widget
    rightBackgroundWidget = new QWidget;
    rightBackgroundWidget->setObjectName("rightBackgroundWidget");
    rightBackgroundWidget->setStyleSheet("#rightBackgroundWidget { background-color:black; }");

    rightWidgetLayout = new QVBoxLayout(rightBackgroundWidget);
    rightWidgetLayout->setSpacing(0);
    rightWidgetLayout->addLayout(titleBarLayout);
    rightTitleBarSpacer = new QSpacerItem(0, 0);
    rightWidgetLayout->addSpacerItem(rightTitleBarSpacer);
    rightWidgetLayout->addSpacing(5);

    // Create MachineClicker widget
    machineClicker = new MachineClicker;
    transparencyWidget = createTransparencyWidget();

    // Create frameless setting widget
    framelessWidget = createFramelessWidget();

    // Create macOS setting widget
    macOSWidget = createMacOSWidget();

    // Create fullscreen setting widget
    fullscreenSwitch = new ToggleButton;
    fullscreenSwitch->toggle();
    fullscreenWidget = new LabelVControl("Toggle frameless/borderless fullscreen window", new ControlHLabel(fullscreenSwitch));
    connect(fullscreenSwitch, &QAbstractButton::toggled, this, [this]() {
        framelessWindowConverter->toggleFullscreen();
    });

    rightStackedLayout = new QStackedLayout;
    rightStackedLayout->addWidget(machineClicker);
    rightStackedLayout->addWidget(transparencyWidget);
    rightStackedLayout->addWidget(framelessWidget);
    rightStackedLayout->addWidget(fullscreenWidget);
    rightStackedLayout->addWidget(macOSWidget);

    machineClicker->layout()->setContentsMargins(8, 5, 8, 8);
    rightWidgetLayout->addLayout(rightStackedLayout);
    rightWidgetLayout->setContentsMargins(0, 0, 0, 0);
}
