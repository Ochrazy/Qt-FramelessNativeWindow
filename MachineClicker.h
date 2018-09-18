#ifndef MACHINECLICKER_H
#define MACHINECLICKER_H

#include <QMainWindow>
#include <QThread>
#include "InputSimulation.h"
#include "SystemWideHotkey.h"
#include "FramelessWindowConverter.h"
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
    bool eventFilter(QObject* obj, QEvent* ev) override;
    void enterEvent(QEvent *) override;
    void leaveEvent(QEvent *) override;
    void paintEvent(QPaintEvent*) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void moveEvent(QMoveEvent*) override;
    bool event(QEvent*) override;
    void resizeEvent(QResizeEvent* ev) override;

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
    QColor colora;

    QPixmap pixmap;
    bool jj = true;

};

#endif // MACHINECLICKER_H
