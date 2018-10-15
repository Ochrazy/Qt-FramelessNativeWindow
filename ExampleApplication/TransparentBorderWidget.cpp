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
        }
        else if(!(windowState() & Qt::WindowFullScreen) && (stateEvent->oldState() & Qt::WindowFullScreen))
        {
            TopLevelLayout->setContentsMargins(borderWidth, borderWidth, borderWidth, borderWidth);
             framelessWindowConverter.setEnableResizing(true);
        }

        if((windowState() & Qt::WindowMaximized) && !(stateEvent->oldState() & Qt::WindowMaximized))
        {
            TopLevelLayout->setContentsMargins(0, 0, 0, 0);
            framelessWindowConverter.setEnableResizing(false);
        }
        else if(!(windowState() & Qt::WindowMaximized) && (stateEvent->oldState() & Qt::WindowMaximized))
        {
            TopLevelLayout->setContentsMargins(borderWidth, borderWidth, borderWidth, borderWidth);
            framelessWindowConverter.setEnableResizing(true);
        }
        break;
    }
    case QEvent::Resize:
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
        }
        else  if(resizeEvent->size().height() == screenSize.height() && resizeEvent->size().width() < screenRect.width())
        {
            TopLevelLayout->setContentsMargins(borderWidth, 0, borderWidth, 0);
        }
        else
        {
            windowSnapped = false;
            TopLevelLayout->setContentsMargins(borderWidth, borderWidth, borderWidth, borderWidth);
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

void TransparentBorderWidget::paintEvent(QPaintEvent* ev)
{
    // Color the blurred background
    QPainter painter(this);
    QColor color(255,255,255);

    color.setAlpha(255);
    painter.setOpacity(0.004);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(ev->rect(),color);
}
