#ifndef SETTINGWIDGET_H
#define SETTINGWIDGET_H

#include <QWidget>

class SettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SettingWidget(const QString& inDescription, QWidget* inControl, QWidget *parent = nullptr);

    QWidget* getControl();
    void setDescription(const QString& inDescription);
    class QLabel* getDescription();

private:
    QWidget* control;
    class QLabel* description;
};

#endif // SETTINGWIDGET_H
