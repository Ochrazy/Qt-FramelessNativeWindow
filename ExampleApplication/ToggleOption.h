#ifndef TOGGLEOPTION_H
#define TOGGLEOPTION_H

#include <QWidget>

class ToggleOption : public QWidget
{
    Q_OBJECT
public:
    explicit ToggleOption(QWidget *parent = nullptr);

    class ToggleButton* getButton();
    class QLabel* getOnOffLabel();

private:
    class ToggleButton* toggleButton;
    QLabel* onOffText;
};

#endif // TOGGLEOPTION_H
