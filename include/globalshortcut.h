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

    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;
    Q_INVOKABLE void updateShortcut(const QString &key);

signals:
    void activated();

private:
    bool registered;
    QString shortcutKey;

    void registerShortcut();
    void unregisterShortcut();
};

#endif // GLOBALSHORTCUT_H
