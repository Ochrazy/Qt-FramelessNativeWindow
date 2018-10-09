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
    explicit ExampleApplication(QWidget *parent = nullptr);

    bool nativeEvent(const QByteArray& eventType, void* message, long* result) override;
    void resizeEvent(QResizeEvent* ev) override;
    void paintEvent(QPaintEvent* ev) override;

private:
    FWC::FWCPARAMS fwcParams;
    FWC::FramelessWindowConverter framelessWindowConverter;
    TranslucentBlurEffect translucencyBlurEffect;

    // Frameless Window
    void setupFramelessWindow();
    int titleBarHeight = 25;

    // Main Widgets
    QWidget* rightBackgroundWidget;
    QWidget* leftBackgroundWidget;
    int widthOfLeftBackgroundWidget = 400;

    // Left side widgets
    void createLeftSideWidgets();
    class QVBoxLayout* settingSelectionLayout;
    class QPushButton* createSettingSelectionButton(const QString& inText, QWidget* inOptionWidget, QLayout* inLayout);
    QLabel* windowTitle;
    QWidget* selectionIndicator;

    // Right side widgets
    void createRightSideWidgets();
    WindowButtons* windowButtons;
    MachineClicker* machineClicker;
    QWidget* createTransparencySettingWidget();
    int windowOpacity = 40;
    class SettingWidget* transparencySettingWidget;
    SettingWidget* macOSSettingWidget;
    SettingWidget* framelessSettingWidget;

    class ToggleButton* translucentBlurSwitch;
    ToggleButton* transparencySwitch;
    ToggleButton* framelessSwitch;
    ToggleButton* trafficLightSwitch;
    class QStackedLayout* rightStackedLayout;
};

#endif // OPTIONTAB_H
