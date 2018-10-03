#ifndef TOGGLEOPTION_H
#define TOGGLEOPTION_H

#include <QWidget>

class ToggleOption : public QWidget
{
    Q_OBJECT
public:
    explicit ToggleOption(QWidget *parent = nullptr);

    void setDescription(const QString& inDescription);

    class ToggleButton* getButton();
    class QLabel* getDescriptionLabel();
    QLabel* getOnOffLabel();

private:
    class ToggleButton* toggleButton;
    class QLabel* optionDescription;
    QLabel* onOffText;
};

#endif // TOGGLEOPTION_H
