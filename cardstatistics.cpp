#include "cardstatistics.h"

#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QStandardPaths>

static QVector<QByteArray> DRAFT_TYPES = {
    "PremierDraft",
    "QuickDraft",
    "BotDraft",
    "TradDraft",
    "Sealed",
    "TradSealed",
};

CardStatisticsDatabase::CardStatisticsDatabase()
    : m_network(new QNetworkAccessManager(this))
{
    connect(
        m_network.data(),
        &QNetworkAccessManager::finished,
        this,
        &CardStatisticsDatabase::onRequestFinished
    );
}

static QFile setCacheFile(const QByteArray &set)
{
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    return QFile(dir + QDir::separator() + set + ".json");
}

void CardStatisticsDatabase::saveSetData(const QByteArray &set, const QByteArray &data) const
{
    QFile file = setCacheFile(set);
    if (file.open(QFile::WriteOnly)) {
        file.write(data);
        qDebug() << "Saved set data to" << file.fileName();
    } else {
        qDebug() << "Failed to open" << file.fileName() << "for writing";
    }
}

bool CardStatisticsDatabase::loadSetData(const QByteArray &set)
{
    QFile file = setCacheFile(set);
    if (file.open(QFile::ReadOnly)) {
        const QByteArray data = file.readAll();
        if (addCardData(data)) {
            qDebug() << "Loaded set data from" << file.fileName();
            return true;
        } else {
            qDebug() << "Failed to load set data from" << file.fileName();
        }
    }
    return false;
}

void CardStatisticsDatabase::addSet(const QByteArray &setName)
{
    if (m_sets.contains(setName))
        return;

    if (loadSetData(setName))
        return;

    QByteArray url = "https://www.17lands.com/card_ratings/data?expansion={SET}&format={FMT}";
    url.replace("{SET}", setName);
    url.replace("{FMT}", DRAFT_TYPES[1]);

    QNetworkReply *reply = m_network->get(QNetworkRequest(QUrl(url)));
    m_currentRequests[reply] = setName;
    m_sets << setName;
}

void CardStatisticsDatabase::onRequestFinished(QNetworkReply *reply)
{
    const QByteArray data = reply->readAll();
    const QByteArray set = m_currentRequests.take(reply);

    addCardData(data);
    saveSetData(set, data);
}

bool CardStatisticsDatabase::addCardData(const QByteArray &json)
{
    const QJsonArray array = QJsonDocument::fromJson(json).array();

    for (const QJsonValue &cardJson : array) {
        const QJsonObject cardObj = cardJson.toObject();
        const float avgSeen = cardObj["avg_seen"].toDouble();
        const float avgPick = cardObj["avg_pick"].toDouble();
        const float winRate = cardObj["win_rate"].toDouble();
        const int id = cardObj["mtga_id"].toInt();

        if (id > 0 && avgSeen > 0 && avgPick > 0 && winRate > 0) {
            m_cards[id] = CardStatistics { avgSeen, avgPick, winRate };
        }
    }

    emit dataChanged();

    return !array.isEmpty();
}
