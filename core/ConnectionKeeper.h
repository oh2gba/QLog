#ifndef QLOG_CORE_CONNECTIONKEEPER_H
#define QLOG_CORE_CONNECTIONKEEPER_H

#include <QObject>
#include <QTimer>
#include <QList>
#include <QString>

/* Keeps a piece of equipment (rig, rotator, CW keyer) connected as long as
 * the operator wants it connected. The connect button of the equipment no
 * longer means "connected" but "I want this connected": when the connection
 * cannot be opened or is lost, the keeper tries again with a growing delay
 * until it succeeds or the operator switches the button off. */
class ConnectionKeeper : public QObject
{
    Q_OBJECT

public:
    enum class State
    {
        Off,          // the operator does not want the connection
        Connecting,   // wanted, but not connected - an attempt is running or scheduled
        Connected
    };
    Q_ENUM(State)

    explicit ConnectionKeeper(const QString &deviceName, QObject *parent = nullptr);

    State state() const { return currentState; }
    bool isWanted() const { return wanted; }
    const QString &lastError() const { return lastErrorText; }
    int nextAttemptSeconds() const { return nextDelay; }
    QString description() const;

    // delays between attempts in seconds; the last one repeats
    void setRetryDelays(const QList<int> &seconds);
    static const QList<int> DEFAULT_RETRY_DELAYS;

public slots:
    void setWanted(bool wanted);
    void deviceConnected();
    void deviceDisconnected();
    void deviceFailed(const QString &error, const QString &detail);
    // the device answers but cannot be used (e.g. the rig is switched off behind rigctld);
    // the device itself reports when it is usable again via deviceConnected()
    void deviceUnavailable(const QString &reason);
    void retryNow();

signals:
    void openRequested();
    void closeRequested();
    void stateChanged(ConnectionKeeper::State state, const QString &description);
    void message(const QString &text);

private:
    void scheduleRetry();
    void setState(State state);

    QString deviceName;
    QList<int> retryDelays;
    QTimer retryTimer;
    bool wanted = false;
    State currentState = State::Off;
    QString lastErrorText;
    QString lastErrorDetail;
    int attempt = 0;
    int nextDelay = 0;
};

#endif // QLOG_CORE_CONNECTIONKEEPER_H
