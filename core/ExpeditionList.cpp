#include <QJsonDocument>
#include <QJsonArray>
#include <QDateTime>

#include "ExpeditionList.h"
#include "debug.h"

MODULE_IDENTIFICATION("qlog.core.expeditionlist");

const QString ExpeditionList::CLUBID = QStringLiteral("DXPED");
const QString ExpeditionList::DIRECTORY_FILENAME = QStringLiteral("clublog-expeditions");
const QString ExpeditionList::URL = QStringLiteral("https://clublog.org/expeditions.php?api=1");
const int ExpeditionList::ACTIVE_MONTHS = 12;

QDate ExpeditionList::cutoffDate(const QDate &today)
{
    FCT_IDENTIFICATION;

    return today.addMonths(-ACTIVE_MONTHS);
}

QList<ExpeditionList::Entry> ExpeditionList::parse(const QByteArray &json,
                                                   const QDate &cutoff,
                                                   QString *error)
{
    FCT_IDENTIFICATION;

    qCDebug(function_parameters) << json.size() << cutoff;

    QList<Entry> ret;
    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(json, &parseError);

    if ( parseError.error != QJsonParseError::NoError || !doc.isArray() )
    {
        if ( error )
            *error = ( parseError.error != QJsonParseError::NoError ) ? parseError.errorString()
                                                                        : QStringLiteral("not a JSON array");
        return ret;
    }

    const QJsonArray rows = doc.array();

    for ( const QJsonValue &row : rows )
    {
        const QJsonArray fields = row.toArray();

        if ( fields.size() < 2 )
            continue;

        Entry entry;
        entry.callsign = fields.at(0).toString().trimmed().toUpper();
        entry.lastQSO = QDateTime::fromString(fields.at(1).toString(), QStringLiteral("yyyy-MM-dd HH:mm:ss")).date();
        entry.qsoCount = ( fields.size() > 2 ) ? fields.at(2).toString().toInt() : 0;

        if ( entry.callsign.isEmpty() || !entry.lastQSO.isValid() )
            continue;

        if ( entry.lastQSO < cutoff )
            continue;

        ret << entry;
    }

    qCDebug(runtime) << "Active DXpeditions:" << ret.size() << "of" << rows.size();

    return ret;
}
