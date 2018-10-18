#ifndef BASEWIDGET_H
#define BASEWIDGET_H

#include <QWidget>
#include "FramelessWindowConverter/FramelessWindowConverter.h"
#include "ExampleApplication.h"

class TransparentBorderWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TransparentBorderWidget(QWidget *parent = nullptr);

    bool nativeEvent(const QByteArray& eventType, void* message, long* result) override;
    void paintEvent(QPaintEvent* ev) override;
    bool event(QEvent* event) override;

    void setBorderWidth(int inBorderWidth);
    int getBorderWidth();

private:
    ExampleApplication* exApp;
    FWC::FramelessWindowConverter framelessWindowConverter;
    void setupFramelessWindow(bool hasWindowDropShadow = false);
    QVBoxLayout* TopLevelLayout;
    bool windowSnapped = false;
    int borderWidth = 10;
    void updateDropShadow();
    QImage shadowImage = QImage(QSize(width(), height()), QImage::Format_ARGB32_Premultiplied);
};

#endif // BASEWIDGET_H
