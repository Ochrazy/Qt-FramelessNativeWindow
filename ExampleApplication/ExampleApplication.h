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
    MachineClicker* machineClicker;

    QWidget* rightBackgroundWidget;
    QWidget* leftBackgroundWidget;
    QLabel* testLabel;
    int widthOfRightBackgroundWidget = 200;

    class QPushButton* CloseButton;
    QPushButton* MinimizeButton;
    QPushButton* MaximizeButton;
};

#endif // OPTIONTAB_H
