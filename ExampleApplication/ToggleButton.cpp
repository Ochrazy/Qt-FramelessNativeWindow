#include "ToggleButton.h"
#include <QPainter>
#include <QPaintEvent>

ToggleButton::ToggleButton(QWidget *parent) : QAbstractButton(parent)
{
    setFixedSize(44, 20);
    setCheckable(true);

    animationTimer.setInterval(33);

    connect(&animationTimer, &QTimer::timeout, this, &ToggleButton::playToggleAnimation);
    connect(this, &QAbstractButton::toggled, [this]() { animationTimer.start(); });
}

void ToggleButton::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;

    QColor color("#0078d7");
    if(!isEnabled())
        color = Qt::gray;
    int arcWidth = 2;
    int arcHalfWidth = 1;
    QPen pen(color, arcWidth);
    painter.setPen(pen);

    int angleSpan = 180.0;
    int startAngle = -90.0;
    QBrush fillBrush = QBrush(Qt::white, Qt::SolidPattern);
    QPoint centrePt(this->width() / 2, this->height() / 2);

    path.moveTo(this->width() / 2, this->height() - arcHalfWidth);

    QRect rectOfRightHalfCircle = event->rect();
    rectOfRightHalfCircle.setX(event->rect().x() + this->width() / 2);
    rectOfRightHalfCircle.adjust(arcHalfWidth, arcHalfWidth, -arcHalfWidth, -arcHalfWidth);
    path.arcTo(rectOfRightHalfCircle, startAngle, angleSpan);

    QRect rectOfLeftHalfCircle = event->rect();
    rectOfLeftHalfCircle.setWidth(event->rect().width() - this->width() / 2);
    rectOfLeftHalfCircle.adjust(arcHalfWidth, arcHalfWidth, -arcHalfWidth,-arcHalfWidth);
    path.arcTo(rectOfLeftHalfCircle, startAngle + 180.0, angleSpan);
    path.closeSubpath();

    painter.drawPath(path);
    if(!isChecked()) painter.fillPath(path, color);

    color = QColor(Qt::white);
    painter.setPen(color);
    painter.setBrush(color);
    painter.drawEllipse(29, 6, 8, 8);
}

void ToggleButton::playToggleAnimation()
{
    percentAnimationPlayed += 1;
    update();
}
