#include "TranslucentBlurEffect.h"
#include <QApplication>
#include <QPainter>
#include <QPaintEvent>
#include <QWindow>
#include <QScreen>
#include <QTimer>
#include <QDesktopWidget>

TranslucentBlurEffect::TranslucentBlurEffect(QWidget* inWidgetToAddEffect, QObject *parent, int inBlurStrength) : QObject(parent), blurStrength(inBlurStrength)
{
    qApp->installEventFilter(this);
    widgetToAddEffect = inWidgetToAddEffect;
}

void TranslucentBlurEffect::reactivateEffect()
{
    qApp->installEventFilter(this);
    bIsActive = true;
    widgetToAddEffect->update();
}

void TranslucentBlurEffect::deactivateEffect()
{
    qApp->removeEventFilter(this);
    bIsActive = false;
    widgetToAddEffect->update();
}

void TranslucentBlurEffect::setBlurStrength(int inBlurStrength)
{
    blurStrength = inBlurStrength;
}

int TranslucentBlurEffect::getBlurStrength()
{
    return blurStrength;
}

bool TranslucentBlurEffect::isActive()
{
    return bIsActive;
}

QImage blurImage(const QImage& image, const QRect& rect, int radius)
{
    int tab[] = { 14, 10, 8, 6, 5, 5, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2 };
    int alpha = (radius < 1)  ? 16 : (radius > 17) ? 1 : tab[radius-1];

    QImage result = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    int r1 = rect.top();
    int r2 = rect.bottom();
    int c1 = rect.left();
    int c2 = rect.right();

    int bpl = result.bytesPerLine();
    int rgba[4];
    unsigned char* p;

    int i1 = 0;
    int i2 = 3;

    for (int col = c1; col <= c2; col++) {
        p = result.scanLine(r1) + col * 4;
        for (int i = i1; i <= i2; i++)
            rgba[i] = p[i] << 4;

        p += bpl;
        for (int j = r1; j < r2; j++, p += bpl)
            for (int i = i1; i <= i2; i++)
                p[i] = static_cast<unsigned char>((rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4);
    }

    for (int row = r1; row <= r2; row++) {
        p = result.scanLine(row) + c1 * 4;
        for (int i = i1; i <= i2; i++)
            rgba[i] = p[i] << 4;

        p += 4;
        for (int j = c1; j < c2; j++, p += 4)
            for (int i = i1; i <= i2; i++)
                p[i] = static_cast<unsigned char>((rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4);
    }

    for (int col = c1; col <= c2; col++) {
        p = result.scanLine(r2) + col * 4;
        for (int i = i1; i <= i2; i++)
            rgba[i] = p[i] << 4;

        p -= bpl;
        for (int j = r1; j < r2; j++, p -= bpl)
            for (int i = i1; i <= i2; i++)
                p[i] = static_cast<unsigned char>((rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4);
    }

    for (int row = r1; row <= r2; row++) {
        p = result.scanLine(row) + c2 * 4;
        for (int i = i1; i <= i2; i++)
            rgba[i] = p[i] << 4;

        p -= 4;
        for (int j = c1; j < c2; j++, p -= 4)
            for (int i = i1; i <= i2; i++)
                p[i] = static_cast<unsigned char>((rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4);
    }

    return result;
}

bool TranslucentBlurEffect::eventFilter(QObject* object, QEvent* event)
{
    switch(event->type())
    {
    case QEvent::Move:
    {
        widgetToAddEffect->repaint();
        widgetToAddEffect->update();
        break;
    }
    case QEvent::WindowActivate:
    {
        emit hideNonQtWidgets();
        noDrawOtherWidgets = true;
        noDrawBackground = true;
        widgetToAddEffect->repaint();
        break;
    }
    case QEvent::WindowStateChange:
    {
        // Currently only used and necessary on macOS
        // If the user goes fullscreen switches back and e.g. moves windows around,
        // a new screenshot has to be taken when coming out of fullscreen mode
        QWindowStateChangeEvent* stateEvent = static_cast<QWindowStateChangeEvent*>(event);
        if ((stateEvent->oldState() & Qt::WindowFullScreen) && !(widgetToAddEffect->windowState() & Qt::WindowFullScreen))
        {
            emit hideNonQtWidgets();
            noDrawOtherWidgets = true;
            noDrawBackground = true;
            widgetToAddEffect->repaint();
        }
        break;
    }
    case QEvent::Paint:
    {
        QPaintEvent* paintEvent = reinterpret_cast<QPaintEvent*>(event);
        if(noDrawOtherWidgets && (object != widgetToAddEffect) )
        {
            QPainter painter;
            painter.begin(dynamic_cast<QWidget*>(object));
            painter.setOpacity(0.0);
            painter.setCompositionMode(QPainter::CompositionMode_Source);
            painter.fillRect(paintEvent->rect(),QColor(100,100,100,255));
            painter.end();
            return true;
        }

        if(object == widgetToAddEffect)
        {
            if(!noDrawBackground)
            {
                QPainter painter;
                QRect rectOfAllScreens = qApp->screens().first()->virtualGeometry();

                if(bTakeScreenshot)
                {
                    // Pixmap of all existing the screens
                    pixmap = qApp->screens().at(0)->grabWindow(0, rectOfAllScreens.x(), rectOfAllScreens.y(),
                                                               rectOfAllScreens.width(), rectOfAllScreens.height());
                    blurredScreenshot = blurImage(pixmap.toImage(), pixmap.rect(), blurStrength);
                    bTakeScreenshot = false;
                    noDrawOtherWidgets = false;
                    takingScreen = false;
                    emit showNonQtWidgets();
                }

                painter.begin(widgetToAddEffect);
                painter.setOpacity(1.0);
                painter.setCompositionMode(QPainter::CompositionMode_Clear);
                painter.fillRect(paintEvent->rect(),QColor(100,100,100,255));

                if(bIsActive)
                {
                    // Draw blurred background image (taking multi monitor setups into account)
                    painter.setOpacity(1.0);
                    painter.setCompositionMode(QPainter::CompositionMode_Source);
                    painter.drawImage(paintEvent->rect(), blurredScreenshot,
                                      QRect(widgetToAddEffect->mapToGlobal(paintEvent->rect().topLeft()) - QPoint(rectOfAllScreens.x(), rectOfAllScreens.y()),
                                            widgetToAddEffect->mapToGlobal(paintEvent->rect().bottomRight()) - QPoint(rectOfAllScreens.x(), rectOfAllScreens.y())));

                    painter.end();
                }
            }
            else
            {
                QPainter painter;
                painter.begin(widgetToAddEffect);
                painter.setOpacity(1.0);
                painter.setCompositionMode(QPainter::CompositionMode_Clear);
                painter.fillRect(paintEvent->rect(),QColor(250,100,100,255));
                painter.end();

                if(!takingScreen)
                {
                    takingScreen = true;
                    QTimer::singleShot(100, this, [this](){
                        bTakeScreenshot = true;
                        noDrawBackground = false;
                        widgetToAddEffect->repaint();
                    });
                }
                return true; // Do not draw anything
            }
        }
        break;
    }
    default:
        break;
    }


    return false;
}
