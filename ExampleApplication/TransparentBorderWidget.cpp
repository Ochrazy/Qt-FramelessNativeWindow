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
#include <QTimer>
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
    TopLevelLayout->setContentsMargins(resizeOutsideBorderWidth, resizeOutsideBorderWidth, resizeOutsideBorderWidth, resizeOutsideBorderWidth);
    setGeometry(200,200,200,200);
}

void TransparentBorderWidget::setBorderWidth(int inBorderWidth) { resizeOutsideBorderWidth = inBorderWidth; TopLevelLayout->setContentsMargins(resizeOutsideBorderWidth, resizeOutsideBorderWidth, resizeOutsideBorderWidth, resizeOutsideBorderWidth); }
int TransparentBorderWidget::getBorderWidth() { return resizeOutsideBorderWidth; }

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
            resizeOutsideBorderWidth = 0;
            framelessWindowConverter.setBorderWidth(resizeOutsideBorderWidth + exApp->getResizeInsideBorderWidth());
        }
        else if(!(windowState() & Qt::WindowFullScreen) && (stateEvent->oldState() & Qt::WindowFullScreen))
        {
            resizeOutsideBorderWidth = 10;
            TopLevelLayout->setContentsMargins(resizeOutsideBorderWidth, resizeOutsideBorderWidth, resizeOutsideBorderWidth, resizeOutsideBorderWidth);
            framelessWindowConverter.setEnableResizing(true);
            framelessWindowConverter.setBorderWidth(resizeOutsideBorderWidth + exApp->getResizeInsideBorderWidth());
        }

        if((windowState() & Qt::WindowMaximized) && !(stateEvent->oldState() & Qt::WindowMaximized))
        {
            TopLevelLayout->setContentsMargins(0, 0, 0, 0);
            framelessWindowConverter.setEnableResizing(false);
            resizeOutsideBorderWidth = 0;
            framelessWindowConverter.setBorderWidth(resizeOutsideBorderWidth + exApp->getResizeInsideBorderWidth());
        }
        else if(!(windowState() & Qt::WindowMaximized) && (stateEvent->oldState() & Qt::WindowMaximized))
        {
            resizeOutsideBorderWidth = 10;
            TopLevelLayout->setContentsMargins(resizeOutsideBorderWidth, resizeOutsideBorderWidth, resizeOutsideBorderWidth, resizeOutsideBorderWidth);
            framelessWindowConverter.setEnableResizing(true);
           framelessWindowConverter.setBorderWidth(resizeOutsideBorderWidth + exApp->getResizeInsideBorderWidth());
        }
        break;
    }
    case QEvent::Resize:
    {
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
                resizeOutsideBorderWidth = 0;
               framelessWindowConverter.setBorderWidth(resizeOutsideBorderWidth + exApp->getResizeInsideBorderWidth());
            }
            else  if(resizeEvent->size().height() == screenSize.height() && resizeEvent->size().width() < screenRect.width())
            {
                resizeOutsideBorderWidth = 10;
                TopLevelLayout->setContentsMargins(resizeOutsideBorderWidth, 0, resizeOutsideBorderWidth, 0);
                resizeOutsideBorderWidth = 0;
            }
            else
            {
                windowSnapped = false;
                resizeOutsideBorderWidth = 10;
                TopLevelLayout->setContentsMargins(resizeOutsideBorderWidth, resizeOutsideBorderWidth, resizeOutsideBorderWidth, resizeOutsideBorderWidth);
                framelessWindowConverter.setBorderWidth(resizeOutsideBorderWidth + exApp->getResizeInsideBorderWidth());
            }
        }

        updateDropShadow();
        update();
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

void TransparentBorderWidget::updateDropShadow()
{
    QColor color(0,0,0);
    int windowWidth = width();
    int windowHeight = height();

    shadowImage = QImage(QSize(windowWidth, windowHeight), QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&shadowImage);
    QRect rect2(0,0, windowWidth, windowHeight);

    painter.setOpacity(0.0);
    painter.setCompositionMode(QPainter::CompositionMode_Clear);
    painter.fillRect(rect2, QColor(0,0,0,0));

    painter.setOpacity(1.0);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    QRect rect(5,5, windowWidth-10, windowHeight-10);
    painter.fillRect(rect,color);

    QImage image2 = TranslucentBlurEffect::blurImage(shadowImage, rect2, 5);
    painter.drawImage(rect2, image2);

    // Only border is drawn -> erase inner color
    painter.setOpacity(0.0);
    painter.setCompositionMode(QPainter::CompositionMode_Clear);
    rect = QRect(10,10, windowWidth-20, windowHeight-20);
    painter.fillRect(rect,color);
}

void TransparentBorderWidget::paintEvent(QPaintEvent* ev)
{
    (void)ev;
    QPainter painter(this);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.setOpacity(0.15);
    painter.drawImage(rect(), shadowImage);
}
