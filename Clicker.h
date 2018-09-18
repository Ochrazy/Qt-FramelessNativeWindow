#ifndef CLICKER_H
#define CLICKER_H

#include <QObject>

class Clicker : public QObject
{
    Q_OBJECT
public:
    explicit Clicker(int clickRate = 1000, QObject *parent = nullptr);
    ~Clicker();

    void initialize();
    void uninizialize();

public slots:
    void startClicking();
    void stopClicking();
    void click();
    void setClickRate(int newClickRate);
private:
   class QTimer* timer;

#ifdef Q_OS_MACOS
   void suspendAppNap();
   void resumeAppNap();
#endif
};

#endif // CLICKER_H
