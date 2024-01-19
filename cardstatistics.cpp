#include "cardstatistics.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>

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

void CardStatisticsDatabase::addSet(const QByteArray &setName)
{
    if (!m_sets.contains(setName)) {
        QByteArray url = "https://www.17lands.com/card_ratings/data?expansion={SET}&format={FMT}";
        url.replace("{SET}", setName);
        url.replace("{FMT}", DRAFT_TYPES[1]);

        m_network->get(QNetworkRequest(QUrl(url)));
        m_sets << setName;
    }
}

void CardStatisticsDatabase::onRequestFinished(QNetworkReply *reply)
{
    const QByteArray data = reply->readAll();
    const QJsonArray array = QJsonDocument::fromJson(data).array();

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
}
