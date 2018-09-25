#ifndef MACHINECLICKER_H
#define MACHINECLICKER_H

#include <QMainWindow>
#include <QThread>
#include "InputSimulation/InputSimulation.h"
#include "SystemWideHotkey/SystemWideHotkey.h"
#include "FramelessWindowConverter/FramelessWindowConverter.h"
#include "TranslucentBlurEffect/TranslucentBlurEffect.h"
#include <QOpenGLWidget>
#include <QFrame>
#include <QTimer>
#include <QSpinBox>

namespace Ui {
class MachineClicker;
}

class MachineClicker : public QWidget
{
    Q_OBJECT

public:
    explicit MachineClicker(QWidget *parent = nullptr);
    ~MachineClicker() override;

    bool nativeEvent(const QByteArray& eventType, void* message, long* result) override;
    void enterEvent(QEvent *) override;
    void leaveEvent(QEvent *) override;
    void paintEvent(QPaintEvent*) override;
    void keyPressEvent(QKeyEvent* ev) override;
    void keyReleaseEvent(QKeyEvent* ev) override;

signals:
    void signalStartClicking();
    void signalStopClicking();
    void signalSetNewClickRate(int newClickRate);

private slots:
    void handleStartStopButton(bool isActive);
    void doClicking();
    void handleTimeSpinBoxChanged(int i);
    void editFinished();

private:
    QThread* timerThread;
    class Clicker* clicker;
    SystemWideHotkey startStopHotkey;

    class QPushButton* CloseButton;
    QPushButton* MinimizeButton;
    QPushButton* MaximizeButton;

    QPushButton* StartStopButton;
    class QSpinBox* IntervalSpinBox;
    class QLabel* HotkeyEditLabel;
    class QKeySequenceEdit* HotkeyEdit;

    FWC::FramelessWindowConverter framelessWindowConverter;
    TranslucentBlurEffect translucencyBlurEffect;
};

#endif // MACHINECLICKER_H
