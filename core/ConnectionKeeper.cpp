#include "ConnectionKeeper.h"
#include "debug.h"

MODULE_IDENTIFICATION("qlog.core.connectionkeeper");

const QList<int> ConnectionKeeper::DEFAULT_RETRY_DELAYS = {5, 10, 20, 30};

ConnectionKeeper::ConnectionKeeper(const QString &deviceName, QObject *parent)
    : QObject(parent),
      deviceName(deviceName),
      retryDelays(DEFAULT_RETRY_DELAYS)
{
    FCT_IDENTIFICATION;

    retryTimer.setSingleShot(true);
    connect(&retryTimer, &QTimer::timeout, this, &ConnectionKeeper::retryNow);
}

void ConnectionKeeper::setRetryDelays(const QList<int> &seconds)
{
    FCT_IDENTIFICATION;

    if ( !seconds.isEmpty() )
        retryDelays = seconds;
}

QString ConnectionKeeper::description() const
{
    FCT_IDENTIFICATION;

    switch ( currentState )
    {
    case State::Connected:
        return tr("%1 connected").arg(deviceName);
    case State::Connecting:
        if ( lastErrorText.isEmpty() )
            return tr("Connecting to %1 ...").arg(deviceName);
        return tr("%1: %2").arg(deviceName, lastErrorText)
               + ( lastErrorDetail.isEmpty() ? QString() : QStringLiteral("\n") + lastErrorDetail )
               + ( nextDelay > 0 ? QStringLiteral("\n") + tr("Next attempt in %1 s").arg(nextDelay)
                                 : QString() );
    default:
        return QString();
    }
}

void ConnectionKeeper::setWanted(bool in_wanted)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << deviceName << in_wanted;

    wanted = in_wanted;
    retryTimer.stop();
    attempt = 0;
    nextDelay = 0;
    lastErrorText.clear();
    lastErrorDetail.clear();

    if ( wanted )
    {
        setState(State::Connecting);
        emit openRequested();
    }
    else
    {
        setState(State::Off);
        emit closeRequested();
    }
}

void ConnectionKeeper::deviceConnected()
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << deviceName;

    retryTimer.stop();

    const bool recovered = ( attempt > 0 || !lastErrorText.isEmpty() );

    attempt = 0;
    nextDelay = 0;
    lastErrorText.clear();
    lastErrorDetail.clear();
    setState(State::Connected);

    if ( recovered )
        emit message(tr("%1 connected").arg(deviceName));
}

void ConnectionKeeper::deviceDisconnected()
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << deviceName << wanted << static_cast<int>(currentState);

    if ( !wanted )
    {
        setState(State::Off);
        return;
    }

    // a lost connection; a failed attempt is reported via deviceFailed()
    if ( currentState == State::Connected )
    {
        lastErrorText = tr("Connection lost");
        lastErrorDetail.clear();
        scheduleRetry();
        emit message(description().replace(QLatin1Char('\n'), QLatin1String(" - ")));
    }
}

void ConnectionKeeper::deviceFailed(const QString &error, const QString &detail)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << deviceName << error << detail;

    if ( !wanted )
    {
        qCDebug(runtime) << "Not wanted - error ignored";
        return;
    }

    lastErrorText = error;
    lastErrorDetail = detail;
    scheduleRetry();
    emit message(description().replace(QLatin1Char('\n'), QLatin1String(" - ")));
}

void ConnectionKeeper::deviceUnavailable(const QString &reason)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << deviceName << reason;

    if ( !wanted || currentState != State::Connected )
        return;

    // no retry: the connection is fine, the device will report when it is back
    lastErrorText = reason;
    lastErrorDetail.clear();
    nextDelay = 0;
    setState(State::Connecting);
    emit message(description());
}

void ConnectionKeeper::retryNow()
{
    FCT_IDENTIFICATION;

    if ( !wanted || currentState == State::Connected )
        return;

    emit openRequested();
}

void ConnectionKeeper::scheduleRetry()
{
    FCT_IDENTIFICATION;

    nextDelay = retryDelays.at(qMin(attempt, retryDelays.size() - 1));
    attempt++;
    retryTimer.start(nextDelay * 1000);
    setState(State::Connecting);

    qCDebug(runtime) << deviceName << "next attempt in" << nextDelay << "s";
}

void ConnectionKeeper::setState(State state)
{
    FCT_IDENTIFICATION;

    currentState = state;
    emit stateChanged(currentState, description());
}
