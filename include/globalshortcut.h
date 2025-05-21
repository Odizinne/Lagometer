#ifndef GLOBALSHORTCUT_H
#define GLOBALSHORTCUT_H

#include <QObject>
#include <QAbstractNativeEventFilter>

class GlobalShortcut : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT

public:
    explicit GlobalShortcut(QObject *parent = nullptr);
    ~GlobalShortcut();

    // Changed long* to qintptr* to match Qt 6 signature
    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;

signals:
    void activated();

private:
    bool registered;

    void registerShortcut();
    void unregisterShortcut();
};

#endif // GLOBALSHORTCUT_H
