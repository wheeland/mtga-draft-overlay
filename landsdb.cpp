#include "landsdb.h"

#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QStandardPaths>
#include <QVector4D>

static const QVector<const char*> DRAFT_TYPES = {
    "PremierDraft",
    "QuickDraft",
    "BotDraft",
    "TradDraft",
    "Sealed",
    "TradSealed",
};

SeventeenLandsDatabase::SeventeenLandsDatabase()
{
    connect(
        &m_network,
        &QNetworkAccessManager::finished,
        this,
        &SeventeenLandsDatabase::onRequestFinished
    );
}

SeventeenLandsCardStats SeventeenLandsDatabase::stats(int card) const
{
    return m_cards[card];
}

static QFile setCacheFile(const QByteArray &set)
{
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    dir += QDir::separator();
    dir += "17lands-data";

    QDir().mkpath(dir);
    return QFile(dir + QDir::separator() + set + ".json");
}

void SeventeenLandsDatabase::saveSetData(const QByteArray &set, const QByteArray &data)
{
    QFile file = setCacheFile(set);
    if (file.open(QFile::WriteOnly)) {
        file.write(data);
        qDebug() << "Saved set data to" << file.fileName();
    } else {
        qDebug() << "Failed to open" << file.fileName() << "for writing";
    }
}

bool SeventeenLandsDatabase::loadSetData(const QByteArray &set)
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

void SeventeenLandsDatabase::addSet(const QByteArray &setName)
{
    if (m_sets.contains(setName))
        return;

    m_sets << setName;

    if (loadSetData(setName))
        return;

    startDownload(setName);
}

void SeventeenLandsDatabase::startDownload(const QByteArray &set)
{
    QByteArray url = "https://www.17lands.com/card_ratings/data?expansion={SET}&format={FMT}&start_date={START}&end_date={END}";
    url.replace("{SET}", set.toUpper());
    url.replace("{FMT}", DRAFT_TYPES[1]);
    url.replace("{START}", "2023-01-01");
    url.replace("{END}", "2026-01-01");

    QNetworkReply *reply = m_network.get(QNetworkRequest(QUrl(url)));
    m_currentRequests[reply] = set;
}

void SeventeenLandsDatabase::clearCache()
{
    for (const QByteArray &set : m_sets)
        startDownload(set);
}

void SeventeenLandsDatabase::onRequestFinished(QNetworkReply *reply)
{
    const QByteArray data = reply->readAll();
    const QByteArray set = m_currentRequests.take(reply);

    addCardData(data);
    saveSetData(set, data);
}

bool SeventeenLandsDatabase::addCardData(const QByteArray &json)
{
    const QJsonArray array = QJsonDocument::fromJson(json).array();

    for (const QJsonValue &cardJson : array) {
        const QJsonObject cardObj = cardJson.toObject();
        const float avgSeen = cardObj["avg_seen"].toDouble();
        const float avgPick = cardObj["avg_pick"].toDouble();
        const float winRate = cardObj["ever_drawn_win_rate"].toDouble();
        const int id = cardObj["mtga_id"].toInt();

        const QString url = cardObj["url"].toString();
        const int lastSlash = url.lastIndexOf('/');
        const int lastDot = url.lastIndexOf('.');
        const QByteArray scryfallId = url.mid(lastSlash + 1, lastDot - lastSlash - 1).toLocal8Bit();

        if (id > 0) {
            m_cards[id] = SeventeenLandsCardStats { scryfallId, id, avgSeen, avgPick, winRate };
        }
    }

    emit dataChanged();

    return !array.isEmpty();
}
