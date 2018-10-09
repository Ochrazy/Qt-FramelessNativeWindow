#ifndef MACHINECLICKER_H
#define MACHINECLICKER_H

#include <QMainWindow>
#include <QThread>
#include "InputSimulation/InputSimulation.h"
#include "SystemWideHotkey/SystemWideHotkey.h"
#include <QOpenGLWidget>
#include <QFrame>
#include <QTimer>
#include <QSpinBox>

class MachineClicker : public QWidget
{
    Q_OBJECT

public:
    explicit MachineClicker(QWidget *parent = nullptr);
    ~MachineClicker() override;

    bool eventFilter(QObject *object, QEvent *event) override;

signals:
    void signalStartClicking();
    void signalStopClicking();
    void signalSetNewClickRate(int newClickRate);

private slots:
    void handleStartStopButton(bool isActive);
    void handleTimeSpinBoxChanged(int i);
    void editFinished();

private:
    QThread* timerThread;
    class Clicker* clicker;
    SystemWideHotkey startStopHotkey;

    class QPushButton* StartStopButton;
    class QSpinBox* IntervalSpinBox;
    class QLabel* HotkeyEditLabel;
    class QKeySequenceEdit* HotkeyEdit;
};

#endif // MACHINECLICKER_H
