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

private slots:
    void playToggleAnimation();

private:
    int percentAnimationPlayed = -1;
    QTimer animationTimer;
};

#endif // TOGGLEBUTTON_H
