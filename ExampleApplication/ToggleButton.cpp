#include "ToggleButton.h"
#include <QPainter>
#include <QPaintEvent>

ToggleButton::ToggleButton(QWidget *parent) : QAbstractButton(parent)
{
    setFixedSize(44, 20);
    setCheckable(true);

    animationTimer.stop();
    animationTimer.setInterval(10);

    connect(&animationTimer, &QTimer::timeout, this, &ToggleButton::playToggleAnimation, Qt::UniqueConnection);
}

void ToggleButton::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw button background
    QPainterPath path;

    QColor color("#0078d7");

    if(mouseHover)
        color = QColor("#006cc1");
    if(isChecked() && mouseHover == false)
    {
        color = QColor("#cccccc");
    }
    else if(isChecked() && mouseHover == true)
    {
        color = QColor("#ffffff");
    }

    if(isDown())
    {
        color = QColor("#cccccc");
    }

    if(!isEnabled())
        color = Qt::darkGray;
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
    if(!isChecked() || isDown()) painter.fillPath(path, color);

    // Draw filled circle
    color = QColor(Qt::white);
    if(!mouseHover && isChecked())
    {
        color = QColor("#cccccc");
    }
    if(!isEnabled())
        color = Qt::darkGray;

    painter.setPen(color);
    painter.setBrush(color);

    static constexpr float d = 23.f/100.f;
    if(isChecked())
    {
        painter.drawEllipse(29 - static_cast<int>(d*percentAnimationPlayed), 6, 8, 8);
    }
    else
    {
        painter.drawEllipse(6 + static_cast<int>(d*percentAnimationPlayed), 6, 8, 8);
    }
}

void ToggleButton::startAnimation()
{
    // If the animation is still playing this immediatley ends it and starts the new one
    percentAnimationPlayed = 0.0;
    animationTimer.start();
}

void ToggleButton::playToggleAnimation()
{
    if(percentAnimationPlayed < 100)
    {
        percentAnimationPlayed += 10;
        if(percentAnimationPlayed > 100)
            percentAnimationPlayed = 100;
        update();
    }
    else
    {
        animationTimer.stop();
    }
}

void ToggleButton::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        startAnimation();
    }
    QAbstractButton::mouseReleaseEvent(event);
}

void ToggleButton::enterEvent(QEvent* event)
{
    mouseHover = true;
    update();
    QAbstractButton::enterEvent(event);
}

void ToggleButton::leaveEvent(QEvent* event)
{
    mouseHover = false;
    update();
    QAbstractButton::leaveEvent(event);
}

bool ToggleButton::event(QEvent* event)
{
    if(event->type() == QEvent::EnabledChange)
    {
        if(isEnabled())
        {
            startAnimation();
        }
    }
    return QAbstractButton::event(event);
}
