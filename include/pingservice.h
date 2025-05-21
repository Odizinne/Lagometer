#ifndef PINGSERVICE_H
#define PINGSERVICE_H

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WIN32_WINNT 0x0601
#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <icmpapi.h>

#include <QObject>
#include <QString>
#include <QTimer>
#include <QElapsedTimer>
#include <QHostInfo>

class PingService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double lastPingTime READ lastPingTime NOTIFY lastPingTimeChanged)
    Q_PROPERTY(bool isRunning READ isRunning NOTIFY isRunningChanged)
    Q_PROPERTY(QString targetHost READ targetHost WRITE setTargetHost NOTIFY targetHostChanged)
    Q_PROPERTY(int interval READ interval WRITE setInterval NOTIFY intervalChanged)

public:
    static PingService* getInstance();

    double lastPingTime() const { return m_lastPingTime; }
    bool isRunning() const { return m_isRunning; }
    QString targetHost() const { return m_targetHost; }
    int interval() const { return m_interval; }

    void setTargetHost(const QString &host);
    void setInterval(int msec);

    Q_INVOKABLE void startPinging();
    Q_INVOKABLE void stopPinging();
    Q_INVOKABLE void singlePing();

signals:
    void lastPingTimeChanged(double pingTime);
    void isRunningChanged(bool running);
    void targetHostChanged(QString host);
    void intervalChanged(int interval);
    void pingError(QString errorMessage);

private:
    explicit PingService(QObject *parent = nullptr);
    PingService(const PingService&) = delete;
    PingService& operator=(const PingService&) = delete;

    static PingService* m_instance;

    QTimer m_pingTimer;
    double m_lastPingTime;
    bool m_isRunning;
    QString m_targetHost;
    int m_interval;

    void doPing();
    double sendIcmpPing(const QString &host, int timeoutMs);
};

#endif // PINGSERVICE_H
