#include "pingservice.h"
#include <QRegularExpression>
#include <QDebug>

PingService* PingService::m_instance = nullptr;

PingService* PingService::getInstance()
{
    if (!m_instance) {
        m_instance = new PingService();
    }
    return m_instance;
}

PingService::PingService(QObject *parent) : QObject(parent),
    m_lastPingTime(0.0),
    m_isRunning(false),
    m_targetHost("8.8.8.8"),
    m_interval(1000)
{
    connect(&m_pingProcess, &QProcess::finished, this, &PingService::onPingFinished);
    connect(&m_pingTimer, &QTimer::timeout, this, &PingService::doPing);
}

void PingService::setTargetHost(const QString &host)
{
    if (m_targetHost != host) {
        m_targetHost = host;
        emit targetHostChanged(host);
    }
}

void PingService::setInterval(int msec)
{
    if (m_interval != msec) {
        m_interval = msec;

        if (m_isRunning) {
            m_pingTimer.setInterval(msec);
        }

        emit intervalChanged(msec);
    }
}

void PingService::startPinging()
{
    if (!m_isRunning) {
        m_isRunning = true;
        m_pingTimer.setInterval(m_interval);
        m_pingTimer.start();
        emit isRunningChanged(true);
        doPing(); // Do first ping immediately
    }
}

void PingService::stopPinging()
{
    if (m_isRunning) {
        m_pingTimer.stop();
        m_isRunning = false;
        emit isRunningChanged(false);
    }
}

void PingService::singlePing()
{
    doPing();
}

void PingService::doPing()
{
    if (m_pingProcess.state() == QProcess::Running) {
        // Process is still running from the last ping, skip this one
        return;
    }

    // Windows-specific ping command
    QStringList args;
    args << "-n" << "1" << "-w" << "1000" << m_targetHost;

    m_pingProcess.start("ping", args);
}

void PingService::onPingFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitCode == 0 && exitStatus == QProcess::NormalExit) {
        QString output = QString::fromLocal8Bit(m_pingProcess.readAllStandardOutput());
        parsePingOutput(output);
    } else {
        // Error occurred
        QString errorMessage = QString::fromLocal8Bit(m_pingProcess.readAllStandardError());
        if (errorMessage.isEmpty()) {
            errorMessage = "Ping failed with exit code " + QString::number(exitCode);
        }
        emit pingError(errorMessage);

        // Set a fallback value to indicate timeout
        m_lastPingTime = -1.0;
        emit lastPingTimeChanged(m_lastPingTime);
    }
}

void PingService::parsePingOutput(const QString& output)
{
    // Parse "time=xxms" from the ping output
    QRegularExpression timeRegex("time=([0-9]+)ms");
    QRegularExpressionMatch match = timeRegex.match(output);

    if (match.hasMatch()) {
        bool ok;
        double pingTime = match.captured(1).toDouble(&ok);

        if (ok) {
            m_lastPingTime = pingTime;
            emit lastPingTimeChanged(pingTime);
        }
    } else {
        qDebug() << "Could not parse ping time from output:" << output;
        // Set a fallback value to indicate timeout
        m_lastPingTime = -1.0;
        emit lastPingTimeChanged(m_lastPingTime);
    }
}
