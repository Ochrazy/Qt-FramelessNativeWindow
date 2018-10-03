#include "ToggleOption.h"
#include "ToggleButton.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

ToggleOption::ToggleOption(QWidget *parent) : QWidget(parent)
{
    toggleButton = new ToggleButton;
    optionDescription = new QLabel;
    optionDescription->setStyleSheet("QLabel { background-color : none; color : white; font-size: 15px; }");

    onOffText = new QLabel;
    onOffText->setText("On");
    onOffText->setStyleSheet("QLabel { background-color : none; color : white; font-size: 16px; }");
    connect(toggleButton, &ToggleButton::toggled, [this](bool checked) {
        if(checked) onOffText->setText("Off");
        else onOffText->setText("On");
    });

    QHBoxLayout* buttonSwitchLayout = new QHBoxLayout;
    buttonSwitchLayout->addWidget(toggleButton, Qt::AlignLeft);
    buttonSwitchLayout->addWidget(onOffText, Qt::AlignLeft);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(14);
    mainLayout->addWidget(optionDescription);
    mainLayout->addLayout(buttonSwitchLayout);
    mainLayout->addStretch();
}

void ToggleOption::setDescription(const QString& inDescription)
{
    optionDescription->setText(inDescription);
}

ToggleButton* ToggleOption::getButton()
{
    return toggleButton;
}

QLabel* ToggleOption::getDescriptionLabel()
{
    return optionDescription;
}

QLabel* ToggleOption::getOnOffLabel()
{
    return onOffText;
}
