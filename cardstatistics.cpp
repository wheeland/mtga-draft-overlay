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

CardDatabase::CardDatabase()
    : m_network(new QNetworkAccessManager(this))
{
    connect(
        m_network.data(),
        &QNetworkAccessManager::finished,
        this,
        &CardDatabase::onRequestFinished
    );
}

static QFile setCacheFile(const QByteArray &set)
{
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    return QFile(dir + QDir::separator() + set + ".json");
}

void CardDatabase::saveSetData(const QByteArray &set, const QByteArray &data) const
{
    QFile file = setCacheFile(set);
    if (file.open(QFile::WriteOnly)) {
        file.write(data);
        qDebug() << "Saved set data to" << file.fileName();
    } else {
        qDebug() << "Failed to open" << file.fileName() << "for writing";
    }
}

bool CardDatabase::loadSetData(const QByteArray &set)
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

void CardDatabase::addSet(const QByteArray &setName)
{
    if (m_sets.contains(setName))
        return;

    m_sets << setName;

    if (loadSetData(setName))
        return;

    startDownload(setName);
}

void CardDatabase::startDownload(const QByteArray &set)
{
    QByteArray url = "https://www.17lands.com/card_ratings/data?expansion={SET}&format={FMT}";
    url.replace("{SET}", set);
    url.replace("{FMT}", DRAFT_TYPES[1]);

    QNetworkReply *reply = m_network->get(QNetworkRequest(QUrl(url)));
    m_currentRequests[reply] = set;
}

void CardDatabase::clearCache()
{
    for (QByteArray set : m_sets)
        startDownload(set);
}

void CardDatabase::onRequestFinished(QNetworkReply *reply)
{
    const QByteArray data = reply->readAll();
    const QByteArray set = m_currentRequests.take(reply);

    addCardData(data);
    saveSetData(set, data);
}

bool CardDatabase::addCardData(const QByteArray &json)
{
    const QJsonArray array = QJsonDocument::fromJson(json).array();

    for (const QJsonValue &cardJson : array) {
        const QJsonObject cardObj = cardJson.toObject();
        const float avgSeen = cardObj["avg_seen"].toDouble();
        const float avgPick = cardObj["avg_pick"].toDouble();
        const float winRate = cardObj["win_rate"].toDouble();
        const int id = cardObj["mtga_id"].toInt();
        const QString name = cardObj["name"].toString();

        if (id > 0) {
            m_cards[id] = CardStatistics { id, name, avgSeen, avgPick, winRate };
        }
    }

    emit dataChanged();

    return !array.isEmpty();
}
