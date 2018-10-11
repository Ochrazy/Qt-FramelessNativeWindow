#include "LabelVControl.h"
#include <QLabel>
#include <QVBoxLayout>

LabelVControl::LabelVControl(const QString& inDescription, QWidget* inControl, QWidget *parent) : QWidget(parent), control(inControl)
{
    description = new QLabel;
    description->setStyleSheet("QLabel { background-color : none; color : white; font-size: 15px; }");
    setDescription(inDescription);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(14);
    mainLayout->addWidget(description);
    mainLayout->addWidget(control);
    mainLayout->addStretch();
}

QWidget* LabelVControl::getControl()
{
    return control;
}

void LabelVControl::setDescription(const QString& inDescription)
{
    description->setText(inDescription);
}

QLabel* LabelVControl::getDescription()
{
    return description;
}
