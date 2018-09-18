#include "Clicker.h"
#include "InputSimulation.h"
#include <QTimer>
#include <QTime>
#include <QElapsedTimer>
#include <math.h>
#include <QMainWindow>

Clicker::Clicker(int clickRate, QObject *parent) : QObject(parent)
{
    timer = new QTimer(this);
    timer->setTimerType(Qt::TimerType::CoarseTimer);
    timer->setInterval(clickRate);
    connect(timer, SIGNAL(timeout()), this, SLOT(click()));

#ifdef Q_OS_MACOS
    suspendAppNap();
#endif
}

Clicker::~Clicker()
{
#ifdef Q_OS_MACOS
    resumeAppNap();
#endif
}

void Clicker::startClicking()
{
    timer->start();
}

void Clicker::stopClicking()
{
    timer->stop();
}

void Clicker::setClickRate(int newClickRate)
{
    timer->setInterval(newClickRate);
}

void Clicker::click()
{
    InputSimulation::SimulateLeftClick();
}
