#ifndef CONTROLHLABEL_H
#define CONTROLHLABEL_H

#include <QWidget>

class ControlHLabel : public QWidget
{
    Q_OBJECT
public:
    explicit ControlHLabel(QWidget* inControl, bool onOffButton = true, const QString& inDescription = "", QWidget* parent = nullptr);

    class QWidget* getControl();
    class QLabel* getLabel();

private:
    class QWidget* control;
    QLabel* label;
};

#endif // CONTROLHLABEL_H
