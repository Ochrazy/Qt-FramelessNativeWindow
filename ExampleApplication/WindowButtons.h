#ifndef WINDOWBUTTONS_H
#define WINDOWBUTTONS_H

#include <QWidget>

class WindowButtons : public QWidget
{
    Q_OBJECT
public:
    explicit WindowButtons(int inTitleBarHeight, QWidget *parent = nullptr);

    bool eventFilter(QObject* obj, QEvent* event) override;

    class QPushButton* getCloseButton();
    QPushButton* getMinimizeButton();
    QPushButton* getMaximizeButton();

    static QString getStyleSheetString(QString iconName, QString hoverBackgroundColor, QString pressedBackgroundColor);

private:
    QPushButton* CloseButton;
    QPushButton* MinimizeButton;
    QPushButton* MaximizeButton;
};

#endif // WINDOWBUTTONS_H
