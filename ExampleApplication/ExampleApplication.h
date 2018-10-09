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
    class QPushButton* createSettingSelectionButton(const QString& inText, QWidget* inOptionWidget, QLayout* inLayout);
    QLabel* windowTitle;
    QWidget* selectionIndicator;
    QWidget* transparencyOptionWidget;
    class QScrollArea* leftScrollArea;

    // Right side widgets
    void createRightSideWidgets();
    WindowButtons* windowButtons;
    MachineClicker* machineClicker;
    void createTransparencyOptionWidget();
    QLabel* opacitySliderLabel;
    class QSlider* windowOpacitySlider;
    int windowOpacity = 40;
    QSlider* translucentBlurStrengthSlider;
    class ToggleButton* translucentBlurSwitch;
    ToggleButton* transparencySwitch;
    ToggleButton* framelessSwitch;
    ToggleButton* trafficLightSwitch;
    class QStackedLayout* rightStackedLayout;

    QString getOptionButtonStyleSheetString();
};

#endif // OPTIONTAB_H
