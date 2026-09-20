#ifndef QLOG_CORE_EXPEDITIONLIST_H
#define QLOG_CORE_EXPEDITIONLIST_H

#include <QByteArray>
#include <QDate>
#include <QList>
#include <QString>

/* Active DXpeditions published by Club Log (https://clublog.org/expeditions.php).
 * QLog keeps them as the built-in club list "DXPED", so that a DXpedition can be
 * combined with any other list in the Member filters - typically with "LoTW",
 * because most DXpeditions upload to LoTW only after they are over. */
class ExpeditionList
{
public:
    struct Entry
    {
        QString callsign;
        QDate lastQSO;
        int qsoCount = 0;
    };

    static const QString CLUBID;        // "DXPED"
    static const QString DIRECTORY_FILENAME;
    static const QString URL;
    static const int ACTIVE_MONTHS;     // a DXpedition counts as active this long after its last QSO

    /* Parses the Club Log JSON ([["CALL","yyyy-MM-dd HH:mm:ss","qsos"], ...]) and
     * returns the expeditions with a QSO on or after the cutoff date.
     * Returns an empty list and sets error when the data is not the expected format */
    static QList<Entry> parse(const QByteArray &json, const QDate &cutoff, QString *error = nullptr);
    static QDate cutoffDate(const QDate &today);
    static bool isDirectoryFilename(const QString &filename) { return filename == DIRECTORY_FILENAME; }
};

#endif // QLOG_CORE_EXPEDITIONLIST_H
