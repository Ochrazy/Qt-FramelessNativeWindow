#ifndef TRANSLUCENCYBLUREFFECT_H
#define TRANSLUCENCYBLUREFFECT_H

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

signals:
    void hideNonQtWidgets();
    void showNonQtWidgets();

private:
    QWidget* widgetToAddEffect;
    bool noDrawBackground = true;
    QPixmap pixmap;
    int blurStrength;
    bool takingScreen = false;
    bool noDrawOtherWidgets = false;
    bool bTakeScreenshot = false;
    QImage blurredScreenshot;
};

#endif // TRANSLUCENCYBLUREFFECT_H
