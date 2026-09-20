#include <QtTest>
#include <QLoggingCategory>

#include "core/ExpeditionList.h"

class ExpeditionListTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void parse_active_only();
    void parse_bad_input();
    void cutoff();
};

void ExpeditionListTest::initTestCase()
{
    QLoggingCategory::setFilterRules(QStringLiteral("*.debug=false"));
}

void ExpeditionListTest::parse_active_only()
{
    const QByteArray json = R"([["KH8WW","2026-09-19 17:03:01","23348"],
                                ["ri1fjz ","2026-09-19 08:34:00","23404"],
                                ["3Y0J","2023-02-05 12:00:00","18832"],
                                ["OLD1","2025-09-19 23:59:59","1"],
                                ["EDGE","2025-09-20 00:00:00","2"],
                                ["NODATE","not a date","5"],
                                ["","2026-01-01 00:00:00","5"],
                                ["SHORT"]])";

    QString error;
    const QList<ExpeditionList::Entry> entries = ExpeditionList::parse(json, QDate(2025, 9, 20), &error);

    QVERIFY(error.isEmpty());
    QCOMPARE(entries.size(), 3);

    QCOMPARE(entries.at(0).callsign, QStringLiteral("KH8WW"));
    QCOMPARE(entries.at(0).lastQSO, QDate(2026, 9, 19));
    QCOMPARE(entries.at(0).qsoCount, 23348);

    /* callsigns are normalised */
    QCOMPARE(entries.at(1).callsign, QStringLiteral("RI1FJZ"));

    /* a QSO exactly on the cutoff day still counts */
    QCOMPARE(entries.at(2).callsign, QStringLiteral("EDGE"));
}

void ExpeditionListTest::parse_bad_input()
{
    QString error;

    QVERIFY(ExpeditionList::parse("not json", QDate(2025, 9, 20), &error).isEmpty());
    QVERIFY(!error.isEmpty());

    error.clear();
    QVERIFY(ExpeditionList::parse(R"({"a":1})", QDate(2025, 9, 20), &error).isEmpty());
    QVERIFY(!error.isEmpty());

    error.clear();
    QVERIFY(ExpeditionList::parse("[]", QDate(2025, 9, 20), &error).isEmpty());
    QVERIFY(error.isEmpty());
}

void ExpeditionListTest::cutoff()
{
    QCOMPARE(ExpeditionList::cutoffDate(QDate(2026, 9, 20)), QDate(2025, 9, 20));
    QCOMPARE(ExpeditionList::CLUBID, QStringLiteral("DXPED"));
    QVERIFY(ExpeditionList::isDirectoryFilename(ExpeditionList::DIRECTORY_FILENAME));
    QVERIFY(!ExpeditionList::isDirectoryFilename(QStringLiteral("lotw.csv")));
}

QTEST_APPLESS_MAIN(ExpeditionListTest)

#include "tst_expeditionlist.moc"
