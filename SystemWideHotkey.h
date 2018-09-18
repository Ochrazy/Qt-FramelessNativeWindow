#ifndef SYSTEMWIDEHOTKEY_H
#define SYSTEMWIDEHOTKEY_H

#include <QAbstractEventDispatcher>
#include <QAbstractNativeEventFilter>

class SystemWideHotkey : public QObject, private QAbstractNativeEventFilter
{
     Q_OBJECT

public:
    SystemWideHotkey();
    ~SystemWideHotkey() override;

    void registerNewHotkey(Qt::Key key);

Q_SIGNALS:
    void hotkeyPressed();
    void newHotkeyRegistered();

protected:
     bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override;
     void initialize();

private:
     void unregisterHotkey();
     quint32 nativeKeycode(Qt::Key key);
     unsigned int currentKeycode;
};

#endif // SYSTEMWIDEHOTKEY_H
