#include "MachineClicker.h"

#include <QTimer>
#include <QKeyEvent>
#include <QAbstractEventDispatcher>
#include "QStatusBar"
#include "Clicker.h"
#include <QPushButton>
#include <QSpinBox>
#include <QKeySequenceEdit>
#include <QGridLayout>
#include <QLabel>
#include <QApplication>
#include <QFrame>
#include <QPainter>
#include <QStyle>
#include <QWindow>
#include <QPalette>

MachineClicker::MachineClicker(QWidget *parent) : QWidget(parent)
{
    StartStopButton = new QPushButton;
    StartStopButton->setCheckable(true);
    StartStopButton->setText("Start");
    StartStopButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    IntervalSpinBox = new QSpinBox;
    IntervalSpinBox->setMinimum(1);
    IntervalSpinBox->setMaximum(999);
    IntervalSpinBox->setValue(999);

    HotkeyEdit = new QKeySequenceEdit;
    HotkeyEdit->setKeySequence(QKeySequence(Qt::Key_Space));

    HotkeyEditLabel = new QLabel;
    HotkeyEditLabel->setText("Start/Stop Hotkey: ");
    HotkeyEditLabel->setStyleSheet("QLabel { background-color : none; color : white; }");
    HotkeyEditLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    QGridLayout* GridLayout = new QGridLayout(this);
    GridLayout->addWidget(StartStopButton, 0, 0, 2, 1);
    GridLayout->addWidget(IntervalSpinBox, 0, 1, 2, 1);
    GridLayout->addWidget(HotkeyEditLabel, 2, 0);
    GridLayout->addWidget(HotkeyEdit, 2, 1);
    GridLayout->setSizeConstraint(QLayout::SetDefaultConstraint);

    connect(StartStopButton, SIGNAL (toggled(bool)), this, SLOT (handleStartStopButton(bool)));
    connect(IntervalSpinBox, SIGNAL (valueChanged(int)), this, SLOT (handleTimeSpinBoxChanged(int)));
    connect(HotkeyEdit, &QKeySequenceEdit::editingFinished, this, &MachineClicker::editFinished);

    // Connect the hotkey
    connect(&startStopHotkey, &SystemWideHotkey::hotkeyPressed, StartStopButton, &QAbstractButton::toggle);

    clicker = new Clicker(IntervalSpinBox->value());
    timerThread = new QThread();
    clicker->moveToThread(timerThread);
    connect(timerThread, &QThread::finished, clicker, &QObject::deleteLater);
    connect(this, &MachineClicker::signalStartClicking, clicker, &Clicker::startClicking);
    connect(this, &MachineClicker::signalStopClicking, clicker, &Clicker::stopClicking);
    connect(this, &MachineClicker::signalSetNewClickRate, clicker, &Clicker::setClickRate);
    timerThread->start();

    qApp->installEventFilter(this);
}

MachineClicker::~MachineClicker()
{
}

bool MachineClicker::eventFilter(QObject *object, QEvent *event)
{
    // Only filter window events
    if(object != window()) return false;

    switch(event->type())
    {
    case QEvent::Enter:
        if(StartStopButton->isChecked())
        {
            emit signalStopClicking();
        }
        break;
    case QEvent::Leave:
        if(StartStopButton->isChecked())
        {
            emit signalStartClicking();
        }
        break;
    default:
        break;
    }

    return false;
}

void MachineClicker::editFinished()
{
    int keycode = HotkeyEdit->keySequence()[0];
    startStopHotkey.registerNewHotkey(Qt::Key(keycode));
    HotkeyEdit->clearFocus();
}

void MachineClicker::handleStartStopButton(bool isActive)
{
    // Hotkey button switches the start stop button too
    if(isActive)
    {
        if(window()->frameGeometry().contains(QCursor::pos()) == false)
        {
            emit signalStartClicking();
        }

        StartStopButton->setText("Stop");
    }
    else
    {
        emit signalStopClicking();
        StartStopButton->setText("Start");
    }
}

void MachineClicker::handleTimeSpinBoxChanged(int TimeInMs)
{
    emit signalSetNewClickRate(TimeInMs);
}
