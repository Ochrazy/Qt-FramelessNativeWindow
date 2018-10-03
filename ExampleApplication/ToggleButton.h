#ifndef TOGGLEBUTTON_H
#define TOGGLEBUTTON_H

#include <QAbstractButton>
#include <QTimer>

class ToggleButton : public QAbstractButton
{
    Q_OBJECT
public:
    explicit ToggleButton(QWidget *parent = nullptr);

    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    bool event(QEvent* event) override;

private slots:
    void playToggleAnimation();

private:
    float percentAnimationPlayed = 100;
    QTimer animationTimer;
    bool mouseHover = false;
    void startAnimation();
};

#endif // TOGGLEBUTTON_H
