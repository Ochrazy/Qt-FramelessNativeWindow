#include "ToggleOption.h"
#include "ToggleButton.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

ToggleOption::ToggleOption(QWidget *parent) : QWidget(parent)
{
    toggleButton = new ToggleButton;

    onOffText = new QLabel;
    onOffText->setText("On");
    onOffText->setStyleSheet("QLabel { background-color : none; color : white; font-size: 16px; }");
    connect(toggleButton, &ToggleButton::toggled, [this](bool checked) {
        if(checked) onOffText->setText("Off");
        else onOffText->setText("On");
    });

    QHBoxLayout* buttonSwitchLayout = new QHBoxLayout(this);
    buttonSwitchLayout->addWidget(toggleButton, Qt::AlignLeft);
    buttonSwitchLayout->addWidget(onOffText, Qt::AlignLeft);
}

ToggleButton* ToggleOption::getButton()
{
    return toggleButton;
}

QLabel* ToggleOption::getOnOffLabel()
{
    return onOffText;
}
