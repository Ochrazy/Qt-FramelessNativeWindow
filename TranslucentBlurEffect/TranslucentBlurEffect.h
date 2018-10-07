#ifndef TRANSLUCENTBLUREFFECT_H
#define TRANSLUCENTBLUREFFECT_H

#include <QObject>
#include <QWidget>

class TranslucentBlurEffect : public QObject
{
    Q_OBJECT
public:
    explicit TranslucentBlurEffect(QWidget* inWidgetToAddEffect, QObject *parent = nullptr, int inBlurStrength = 25);

    bool eventFilter(QObject *object, QEvent *event) override;

    void reactivateEffect();
    void deactivateEffect();

    void setBlurStrength(int inBlurStrength);
    int getBlurStrength();

    bool isActive();

signals:
    void hideNonQtWidgets();
    void showNonQtWidgets();

private:
    QWidget* widgetToAddEffect;
    bool noDrawBackground = false;
    QPixmap pixmap;
    int blurStrength;
    bool takingScreen = false;
    bool noDrawOtherWidgets = false;
    bool bTakeScreenshot = false;
    QImage blurredScreenshot;
    bool bIsActive = true;

    QImage blurImage(const QImage& image, const QRect& rect, int radius);
};

#endif // TRANSLUCENTBLUREFFECT_H
