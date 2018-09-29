#ifndef OPTIONTAB_H
#define OPTIONTAB_H

#include <QWidget>
#include "MachineClicker/MachineClicker.h"
#include "FramelessWindowConverter/FramelessWindowConverter.h"
#include "TranslucentBlurEffect/TranslucentBlurEffect.h"

class ExampleApplication : public QWidget
{
    Q_OBJECT
public:
    explicit ExampleApplication(QWidget *parent = nullptr);

    bool nativeEvent(const QByteArray& eventType, void* message, long* result) override;
    void resizeEvent(QResizeEvent* ev) override;
    void keyPressEvent(QKeyEvent* ev) override;
    void keyReleaseEvent(QKeyEvent* ev) override;
    void paintEvent(QPaintEvent* ev) override;

private:
    FWC::FramelessWindowConverter framelessWindowConverter;
    TranslucentBlurEffect translucencyBlurEffect;

    // Frameless Window
    void setupFramelessWindow();
    int titleBarHeight = 25;

    // Main Widgets
    QWidget* rightBackgroundWidget;
    QWidget* leftBackgroundWidget;
    int widthOfRightBackgroundWidget = 200;

    // Left side widgets
    void createLeftSideWidgets();
    QLabel* windowTitle;
    class QPushButton* machineClickerOptionButton;

    // Right side widgets
    void createRightSideWidgets();
    QPushButton* CloseButton;
    QPushButton* MinimizeButton;
    QPushButton* MaximizeButton;
    MachineClicker* machineClicker;
    QPushButton* rightTest;
    class QStackedLayout* rightStackedLayout;


    QString getSystemButtonStyleSheetString(QString iconName, QString hoverBackgroundColor);
    QString getOptionButtonStyleSheetString();
};

#endif // OPTIONTAB_H
