#ifndef PINGSERVICE_H
#define PINGSERVICE_H

#include <QObject>
#include <QProcess>
#include <QString>
#include <QTimer>

class PingService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double lastPingTime READ lastPingTime NOTIFY lastPingTimeChanged)
    Q_PROPERTY(bool isRunning READ isRunning NOTIFY isRunningChanged)
    Q_PROPERTY(QString targetHost READ targetHost WRITE setTargetHost NOTIFY targetHostChanged)
    Q_PROPERTY(int interval READ interval WRITE setInterval NOTIFY intervalChanged)

public:
    static PingService* getInstance();

    // Properties
    double lastPingTime() const { return m_lastPingTime; }
    bool isRunning() const { return m_isRunning; }
    QString targetHost() const { return m_targetHost; }
    int interval() const { return m_interval; }

    void setTargetHost(const QString &host);
    void setInterval(int msec);

    // Q_INVOKABLE methods for QML
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
    QProcess m_pingProcess;
    QTimer m_pingTimer;
    double m_lastPingTime;
    bool m_isRunning;
    QString m_targetHost;
    int m_interval;

    void parsePingOutput(const QString& output);

private slots:
    void onPingFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void doPing();
};

#endif // PINGSERVICE_H
