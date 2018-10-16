#include "TransparentBorderWidget.h"
#include <QVBoxLayout>
#include <QWindow>
#include <QStackedLayout>
#include <QLabel>
#include <QPainter>
#include <QPaintEvent>
#include <QPushButton>
#include <QScreen>
#include <QApplication>
#include "TranslucentBlurEffect/TranslucentBlurEffect.h"
#include "ToggleButton.h"

TransparentBorderWidget::TransparentBorderWidget(QWidget *parent) : QWidget(parent), framelessWindowConverter()
{
    exApp = new ExampleApplication(&framelessWindowConverter, this);

    // Setup frameless window
    setupFramelessWindow(false);

    // Convert window
    framelessWindowConverter.convertWindowToFrameless(winId(), [this](){ windowHandle()->setMouseGrabEnabled(false); });

    TopLevelLayout = new QVBoxLayout(this);
    TopLevelLayout->addWidget(exApp);
    TopLevelLayout->setContentsMargins(borderWidth, borderWidth, borderWidth, borderWidth);
    setGeometry(200,200,200,200);
}

void TransparentBorderWidget::setBorderWidth(int inBorderWidth) { borderWidth = inBorderWidth; TopLevelLayout->setContentsMargins(borderWidth, borderWidth, borderWidth, borderWidth); }
int TransparentBorderWidget::getBorderWidth() { return borderWidth; }

void TransparentBorderWidget::setupFramelessWindow(bool hasWindowDropShadow)
{
    setWindowFlags(Qt::Widget | Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    if(!hasWindowDropShadow)
    {
        setAttribute(Qt::WA_TranslucentBackground, true);
        setAttribute(Qt::WA_NoSystemBackground, true);
        framelessWindowConverter.setEnableShadow(false);
    }
    else
    {
        setAttribute(Qt::WA_TranslucentBackground, false);
        setAttribute(Qt::WA_NoSystemBackground, false);
        framelessWindowConverter.setEnableShadow(true);
        // Disable all transparency effects
        //        exApp->translucencyBlurEffect.deactivateEffect();
        //        exApp->translucentBlurSwitch->setChecked(true);
        //        exApp->transparencySwitch->setChecked(true);
        //        exApp->transparencyWidget->setEnabled(false);
        update();
    }

    setWindowTitle("Example Application");
}

bool TransparentBorderWidget::event(QEvent* event)
{
    switch(event->type())
    {
    case QEvent::WindowStateChange:
    {
        // Adjust transparent border when going to fullscreen or maximizing window
        QWindowStateChangeEvent* stateEvent = static_cast<QWindowStateChangeEvent*>(event);
        if((windowState() & Qt::WindowFullScreen) && !(stateEvent->oldState() & Qt::WindowFullScreen))
        {
            TopLevelLayout->setContentsMargins(0, 0, 0, 0);
            framelessWindowConverter.setEnableResizing(false);
            borderWidth = 0;
            framelessWindowConverter.setBorderWidth(borderWidth);
        }
        else if(!(windowState() & Qt::WindowFullScreen) && (stateEvent->oldState() & Qt::WindowFullScreen))
        {
            borderWidth = 10;
            TopLevelLayout->setContentsMargins(borderWidth, borderWidth, borderWidth, borderWidth);
            framelessWindowConverter.setEnableResizing(true);
            framelessWindowConverter.setBorderWidth(borderWidth);
        }

        if((windowState() & Qt::WindowMaximized) && !(stateEvent->oldState() & Qt::WindowMaximized))
        {
            TopLevelLayout->setContentsMargins(0, 0, 0, 0);
            framelessWindowConverter.setEnableResizing(false);
            borderWidth = 0;
            framelessWindowConverter.setBorderWidth(borderWidth);
        }
        else if(!(windowState() & Qt::WindowMaximized) && (stateEvent->oldState() & Qt::WindowMaximized))
        {
            borderWidth = 10;
            TopLevelLayout->setContentsMargins(borderWidth, borderWidth, borderWidth, borderWidth);
            framelessWindowConverter.setEnableResizing(true);
            framelessWindowConverter.setBorderWidth(borderWidth);
        }
        break;
    }
    case QEvent::Resize:
    {
        drawDropShadow();
        if(!(windowState() & Qt::WindowMaximized))
        {
            // Adjust transparent border when doing window snapping
            QResizeEvent* resizeEvent = static_cast<QResizeEvent*>(event);
            QRect screenRect = qApp->screenAt(QCursor::pos())->geometry();
            QSize screenSize = qApp->screenAt(QCursor::pos())->availableSize();

            if((resizeEvent->size().width() == screenRect.width()/2 && resizeEvent->size().height() == screenSize.height()) ||
                    (windowSnapped && resizeEvent->size().height() == screenSize.height()))
            {
                windowSnapped = true;
                TopLevelLayout->setContentsMargins(0, 0, 0, 0);
                borderWidth = 0;
                framelessWindowConverter.setBorderWidth(borderWidth);
            }
            else  if(resizeEvent->size().height() == screenSize.height() && resizeEvent->size().width() < screenRect.width())
            {
                borderWidth = 10;
                TopLevelLayout->setContentsMargins(borderWidth, 0, borderWidth, 0);
                borderWidth = 0;
                framelessWindowConverter.setBorderWidth(borderWidth);
            }
            else
            {
                windowSnapped = false;
                borderWidth = 10;
                TopLevelLayout->setContentsMargins(borderWidth, borderWidth, borderWidth, borderWidth);
                framelessWindowConverter.setBorderWidth(borderWidth);
            }
        }
        break;
    }
    default:
        break;
    }

    return QWidget::event(event);
}

bool TransparentBorderWidget::nativeEvent(const QByteArray& eventType, void* message, long* result)
{
    Q_UNUSED(eventType)
    return framelessWindowConverter.filterNativeEvents(message, result);
}

void TransparentBorderWidget::drawDropShadow()
{
    QColor color(0,0,0);
    int windowWidth = width();
    int windowHeight = height();

    shadowImage = QImage(QSize(width(), height()), QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&shadowImage);
    QRect rect2(0,0, windowWidth, windowHeight);

    painter.setOpacity(0.0);
    painter.setCompositionMode(QPainter::CompositionMode_Clear);
    painter.fillRect(rect2, QColor(0,0,0,0));

    painter.setOpacity(1.0);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    QRect rect(5,5, windowWidth-10, windowHeight-10);
    painter.fillRect(rect,color);

    QImage image2 = TranslucentBlurEffect::blurImage(shadowImage, rect2, 4);
    painter.drawImage(rect2, image2);

    // Only border is drawn -> erase inner color
    painter.setOpacity(0.0);
    painter.setCompositionMode(QPainter::CompositionMode_Clear);
    rect = QRect(10,10, windowWidth-20, windowHeight-20);
    painter.fillRect(rect,color);
}

void TransparentBorderWidget::paintEvent(QPaintEvent* ev)
{
    QPainter painter(this);
    painter.setOpacity(0.0);
    painter.setCompositionMode(QPainter::CompositionMode_Clear);
    painter.fillRect(ev->rect(), QColor(0,0,0,0));

    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.setOpacity(1.0);
    painter.drawImage(rect(), shadowImage);
}
