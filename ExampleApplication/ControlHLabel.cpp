#include "ControlHLabel.h"
#include "ToggleButton.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

ControlHLabel::ControlHLabel(QWidget* inControl, bool onOffButton, const QString& inDescription, QWidget* parent) : QWidget(parent), control(inControl)
{
    label = new QLabel;
    label->setText(inDescription);
    label->setStyleSheet("QLabel { background-color : none; color : white; font-size: 16px; }");
    if(onOffButton)
    {
        label->setText("On");
        connect(dynamic_cast<QAbstractButton*>(control), &QAbstractButton::toggled, [this](bool checked) {
            if(checked) label->setText("Off");
            else label->setText("On");
        });
    }

    QHBoxLayout* buttonSwitchLayout = new QHBoxLayout(this);
    buttonSwitchLayout->addWidget(control, Qt::AlignLeft);
    buttonSwitchLayout->addWidget(label, Qt::AlignLeft);
}

QWidget* ControlHLabel::getControl()
{
    return control;
}

QLabel* ControlHLabel::getLabel()
{
    return label;
}
