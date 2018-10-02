#ifndef TOGGLEBUTTON_H
#define TOGGLEBUTTON_H

#include <QAbstractButton>

class ToggleButton : public QAbstractButton
{
    Q_OBJECT
public:
    explicit ToggleButton(QWidget *parent = nullptr);

    void paintEvent(QPaintEvent* event) override;
};

#endif // TOGGLEBUTTON_H
