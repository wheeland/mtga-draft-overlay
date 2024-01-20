#include "cardstatistics.h"

#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QStandardPaths>
#include <QVector4D>

static const QVector<QByteArray> DRAFT_TYPES = {
    "PremierDraft",
    "QuickDraft",
    "BotDraft",
    "TradDraft",
    "Sealed",
    "TradSealed",
};

QColor CardStatistics::colorMix() const
{
    QVector4D ret(0, 0, 0, 0);

    if (color & CardDatabase::White)
        ret += QVector4D(255, 255, 255, 1);
    if (color & CardDatabase::Blue)
        ret += QVector4D(100, 100, 255, 1);
    if (color & CardDatabase::Black)
        ret += QVector4D(100, 100, 100, 1);
    if (color & CardDatabase::Red)
        ret += QVector4D(255, 100, 100, 1);
    if (color & CardDatabase::Green)
        ret += QVector4D(100, 255, 100, 1);

    if (ret.w() == 0)
        return QColor(160, 160, 160);

    ret /= ret.w();

    return QColor(ret.x(), ret.y(), ret.z(), 255);
}

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

CardStatistics CardDatabase::stats(int card) const
{
    return m_cards[card];
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
    for (const QByteArray &set : m_sets)
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
        const QString colorS = cardObj["color"].toString();
        const QString rarityS = cardObj["rarity"].toString();

        CardDatabase::Colors colors = CardDatabase::None;
        colors |= colorS.contains("W") ? CardDatabase::White : CardDatabase::None;
        colors |= colorS.contains("U") ? CardDatabase::Blue : CardDatabase::None;
        colors |= colorS.contains("B") ? CardDatabase::Black : CardDatabase::None;
        colors |= colorS.contains("R") ? CardDatabase::Red : CardDatabase::None;
        colors |= colorS.contains("G") ? CardDatabase::Green : CardDatabase::None;

        CardDatabase::Rarity rarity = CardDatabase::Unknown;
        if (rarityS == "common")
            rarity = CardDatabase::Common;
        else if (rarityS == "uncommon")
            rarity = CardDatabase::Uncommon;
        else if (rarityS == "rare")
            rarity = CardDatabase::Rare;
        else if (rarityS == "mythic")
            rarity = CardDatabase::Mythic;
        else
            qWarning() << "Invalid rarity:" << rarityS;

        if (id > 0) {
            m_cards[id] = CardStatistics { id, name, colors, rarity, avgSeen, avgPick, winRate };
        }
    }

    emit dataChanged();

    return !array.isEmpty();
}
