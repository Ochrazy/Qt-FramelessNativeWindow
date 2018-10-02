#ifndef MINIMALSCROLLBAR_H
#define MINIMALSCROLLBAR_H

#include <QScrollBar>

class MinimalScrollBar : public QScrollBar
{
    Q_OBJECT
public:
    explicit MinimalScrollBar(QWidget *parent = nullptr);

    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    QString styleSheetString;
};

#endif // MINIMALSCROLLBAR_H
