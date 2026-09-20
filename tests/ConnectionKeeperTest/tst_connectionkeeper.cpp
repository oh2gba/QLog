#include <QtTest>
#include <QSignalSpy>
#include <QLoggingCategory>

#include "core/ConnectionKeeper.h"

class ConnectionKeeperTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void wanted_opens_and_unwanted_closes();
    void failure_schedules_growing_retries();
    void connected_resets_and_lost_connection_retries();
    void errors_are_ignored_when_not_wanted();
    void retry_fires_open_request();
    void unavailable_device();
};

void ConnectionKeeperTest::initTestCase()
{
    QLoggingCategory::setFilterRules(QStringLiteral("*.debug=false"));
}

void ConnectionKeeperTest::wanted_opens_and_unwanted_closes()
{
    ConnectionKeeper keeper("Rig");
    QSignalSpy opens(&keeper, &ConnectionKeeper::openRequested);
    QSignalSpy closes(&keeper, &ConnectionKeeper::closeRequested);
    QSignalSpy states(&keeper, &ConnectionKeeper::stateChanged);

    QCOMPARE(keeper.state(), ConnectionKeeper::State::Off);
    QVERIFY(keeper.description().isEmpty());

    keeper.setWanted(true);
    QCOMPARE(opens.count(), 1);
    QCOMPARE(keeper.state(), ConnectionKeeper::State::Connecting);
    QVERIFY(keeper.description().contains("Rig"));

    keeper.setWanted(false);
    QCOMPARE(closes.count(), 1);
    QCOMPARE(keeper.state(), ConnectionKeeper::State::Off);
    QCOMPARE(states.count(), 2);
}

void ConnectionKeeperTest::failure_schedules_growing_retries()
{
    ConnectionKeeper keeper("Rig");
    QSignalSpy messages(&keeper, &ConnectionKeeper::message);

    keeper.setWanted(true);

    /* the delays grow and the last one repeats */
    const QList<int> expected = {5, 10, 20, 30, 30, 30};
    for ( int delay : expected )
    {
        keeper.deviceFailed("Communication timed out", "detail");
        QCOMPARE(keeper.state(), ConnectionKeeper::State::Connecting);
        QCOMPARE(keeper.nextAttemptSeconds(), delay);
        QCOMPARE(keeper.lastError(), QStringLiteral("Communication timed out"));
        QVERIFY(keeper.description().contains("Communication timed out"));
        QVERIFY(keeper.description().contains(QString::number(delay)));
    }

    QCOMPARE(messages.count(), expected.size());
    QVERIFY(messages.last().at(0).toString().contains("Next attempt in 30 s"));

    /* switching off stops everything */
    keeper.setWanted(false);
    QCOMPARE(keeper.nextAttemptSeconds(), 0);
    QCOMPARE(keeper.state(), ConnectionKeeper::State::Off);
}

void ConnectionKeeperTest::connected_resets_and_lost_connection_retries()
{
    ConnectionKeeper keeper("Rotator");
    QSignalSpy messages(&keeper, &ConnectionKeeper::message);

    keeper.setWanted(true);
    keeper.deviceFailed("Cannot open Rotator", "");
    keeper.deviceFailed("Cannot open Rotator", "");
    QCOMPARE(keeper.nextAttemptSeconds(), 10);

    /* a successful connection resets the backoff and reports the recovery */
    keeper.deviceConnected();
    QCOMPARE(keeper.state(), ConnectionKeeper::State::Connected);
    QCOMPARE(keeper.nextAttemptSeconds(), 0);
    QVERIFY(keeper.lastError().isEmpty());
    QCOMPARE(keeper.description(), QStringLiteral("Rotator connected"));
    QVERIFY(messages.last().at(0).toString().contains("Rotator connected"));

    /* the connection is lost: the first delay again */
    keeper.deviceDisconnected();
    QCOMPARE(keeper.state(), ConnectionKeeper::State::Connecting);
    QCOMPARE(keeper.nextAttemptSeconds(), 5);
    QVERIFY(keeper.description().contains("Connection lost"));

    /* a disconnect while an attempt is pending is not a second loss */
    keeper.deviceDisconnected();
    QCOMPARE(keeper.nextAttemptSeconds(), 5);
}

void ConnectionKeeperTest::errors_are_ignored_when_not_wanted()
{
    ConnectionKeeper keeper("CW Keyer");
    QSignalSpy messages(&keeper, &ConnectionKeeper::message);
    QSignalSpy opens(&keeper, &ConnectionKeeper::openRequested);

    keeper.deviceFailed("Connection Error", "");
    keeper.deviceDisconnected();
    QCOMPARE(keeper.state(), ConnectionKeeper::State::Off);
    QCOMPARE(keeper.nextAttemptSeconds(), 0);
    QCOMPARE(messages.count(), 0);
    QCOMPARE(opens.count(), 0);
}

void ConnectionKeeperTest::retry_fires_open_request()
{
    ConnectionKeeper keeper("Rig");
    keeper.setRetryDelays({1});
    QSignalSpy opens(&keeper, &ConnectionKeeper::openRequested);

    keeper.setWanted(true);
    QCOMPARE(opens.count(), 1);

    keeper.deviceFailed("Cannot open Rig", "");
    QCOMPARE(keeper.nextAttemptSeconds(), 1);
    QVERIFY(opens.wait(3000));
    QCOMPARE(opens.count(), 2);

    /* once connected, a stray timer must not reopen */
    keeper.deviceFailed("Cannot open Rig", "");
    keeper.deviceConnected();
    QVERIFY(!opens.wait(1500));
    QCOMPARE(opens.count(), 2);
}

void ConnectionKeeperTest::unavailable_device()
{
    ConnectionKeeper keeper("Rig");
    QSignalSpy opens(&keeper, &ConnectionKeeper::openRequested);
    QSignalSpy messages(&keeper, &ConnectionKeeper::message);

    /* ignored while not connected */
    keeper.deviceUnavailable("Rig is not powered on");
    QCOMPARE(keeper.state(), ConnectionKeeper::State::Off);

    keeper.setWanted(true);
    keeper.deviceConnected();

    /* the rig is switched off behind rigctld: yellow, no retry scheduled */
    keeper.deviceUnavailable("Rig is not powered on");
    QCOMPARE(keeper.state(), ConnectionKeeper::State::Connecting);
    QCOMPARE(keeper.nextAttemptSeconds(), 0);
    QCOMPARE(keeper.description(), QStringLiteral("Rig: Rig is not powered on"));
    QCOMPARE(messages.count(), 1);

    /* the rig is switched on again: green, recovery reported, nothing reopened */
    keeper.deviceConnected();
    QCOMPARE(keeper.state(), ConnectionKeeper::State::Connected);
    QCOMPARE(messages.count(), 2);
    QCOMPARE(opens.count(), 1);
}

QTEST_GUILESS_MAIN(ConnectionKeeperTest)

#include "tst_connectionkeeper.moc"
