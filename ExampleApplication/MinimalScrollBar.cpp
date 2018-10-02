#include "MinimalScrollBar.h"

MinimalScrollBar::MinimalScrollBar(QWidget *parent) : QScrollBar(parent),
    styleSheetString("QScrollBar:vertical {"
                     "border: 0px solid none;"
                     "background-color: transparent;"
                     "width: 6px;"
                     "margin: 0px 4px 0px 0px;"
                     "}"
                     "QScrollBar::sub-page:vertical {"
                     "background-color: transparent;"
                     "}"

                     "QScrollBar::add-page:vertical {"
                     "background-color: transparent;"
                     "}"

                     "QScrollBar::handle:vertical {"
                     "border: 0px solid transparent;"
                     "background: lightgrey;"
                     "}"

                     "QScrollBar::add-line:vertical {"
                     "width: 0px;"
                     "height: 0px;"
                     "}"

                     "QScrollBar::sub-line:vertical {"
                     "width: 0px;"
                     "height: 0px;"
                     "}")
{
    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet(styleSheetString);
}

void MinimalScrollBar::enterEvent(QEvent* event)
{
    Q_UNUSED(event)
    setStyleSheet("");
}

void MinimalScrollBar::leaveEvent(QEvent* event)
{
    Q_UNUSED(event)
    setStyleSheet(styleSheetString);
}
