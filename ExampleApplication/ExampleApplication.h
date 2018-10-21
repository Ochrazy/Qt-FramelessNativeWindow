#ifndef OPTIONTAB_H
#define OPTIONTAB_H

#include <QWidget>
#include "MachineClicker/MachineClicker.h"
#include "FramelessWindowConverter/FramelessWindowConverter.h"
#include "TranslucentBlurEffect/TranslucentBlurEffect.h"
#include "WindowButtons.h"

class ExampleApplication : public QWidget
{
    Q_OBJECT
public:
    ExampleApplication(FWC::FramelessWindowConverter* framelessWindowConverter, QWidget *parent = nullptr);

    bool event(QEvent* event) override;
    void resizeEvent(QResizeEvent* ev) override;
    void paintEvent(QPaintEvent* ev) override;

    void setVisibleTitleBarHeight(int inVisibleTitleBarHeight);
    void setResizeInsideBorderWidth(int inBorderWidth);
    int getResizeInsideBorderWidth();

public:
    TranslucentBlurEffect translucencyBlurEffect;

    // Frameless Window
    FWC::FramelessWindowConverter* framelessWindowConverter;
    void setupFramelessWindow();
    int titleBarHeight = 25;

    // Main Widgets
    QWidget* rightBackgroundWidget;
    QWidget* leftBackgroundWidget;
    int widthOfLeftBackgroundWidget = 400;
    // Based on current shown widgets and title bar height
    void setMinMaxWindowSizesAndResizeIfNeeded();
    int resizeInsideBorderWidth = 3;

    // Left side widgets
    void createLeftSideWidgets();
    void dynamicallyShowHidetLeftWidgetBasedOnSize(int windowWidth);
    class QVBoxLayout* settingSelectionLayout;
    class QPushButton* createSettingSelectionButton(const QString& inText, QWidget* inOptionWidget, QLayout* inLayout);
    QLabel* windowTitle;
    QPushButton* settingsButton;
    bool bAlwaysShowSettings = false;
    bool bNeverShowSettings = false;
    QWidget* selectionIndicator;

    // Right side widgets
    QVBoxLayout* rightWidgetLayout;
    class QSpacerItem* rightTitleBarSpacer;
    void createRightSideWidgets();
    WindowButtons* windowButtons;
    MachineClicker* machineClicker;
    QWidget* createTransparencyWidget();
    int windowOpacity = 40;
    QWidget* transparencyWidget;
    QWidget* createMacOSWidget();
    QWidget* macOSWidget;
    QWidget* createFramelessWidget();
    QWidget* framelessWidget;
    class LabelVControl* fullscreenWidget;

    class ToggleButton* translucentBlurSwitch;
    ToggleButton* transparencySwitch;
    ToggleButton* fullscreenSwitch;
    class QStackedLayout* rightStackedLayout;

    // macOS
    int xUpperLeftOfTrafficLights = 50;
    int yUpperLeftOfTrafficLights = 9;
};

#endif // OPTIONTAB_H
