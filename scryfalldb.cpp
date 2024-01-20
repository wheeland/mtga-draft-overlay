#include "scryfalldb.h"

#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkReply>
#include <QStandardPaths>

static QFile cardCacheFile(int id)
{
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    dir += QDir::separator();
    dir += "scryfall-card-data";
    QDir().mkpath(dir);

    return QFile(dir + QDir::separator() + QString::number(id) + ".json");
}

static Card::Colors getColors(const QJsonArray &json)
{
    Card::Colors result = Card::Uncolored;
    for (const QJsonValue &c : json) {
        const QString s = c.toString();
        if (s == "W")
            result |= Card::White;
        else if (s == "U")
            result |= Card::Blue;
        else if (s == "B")
            result |= Card::Black;
        else if (s == "R")
            result |= Card::Red;
        else if (s == "G")
            result |= Card::Green;
    }
    return result;
}

static Card::Rarity getRarity(const QString &str)
{
    if (str == "common")
        return Card::Common;
    else if (str == "uncommon")
        return Card::Uncommon;
    else if (str == "rare")
        return Card::Rare;
    else if (str == "mythic")
        return Card::Mythic;
    return Card::Unknown;
}

static bool parseCardJson(const QJsonObject &json, ScryfallCardData &card)
{
    const int arenaId = json["arena_id"].toInt(-1);
    const QString name = json["name"].toString().toLocal8Bit();
    const QByteArray scryfallId = json["id"].toString().toLocal8Bit();
    const QByteArray set = json["set"].toString().toLocal8Bit();
    const Card::Colors colors = getColors(json["colors"].toArray());
    const Card::Colors colorIdentity = getColors(json["colors"].toArray());
    const Card::Rarity rarity = getRarity(json["rarity"].toString());
    if (arenaId > 0 && !name.isEmpty() && !scryfallId.isEmpty() && set.size() == 3) {
        card.arenaId = arenaId;
        card.name = name;
        card.scryfallId = scryfallId;
        card.set = set;
        card.colors = colors;
        card.colorIdentity = colorIdentity;
        card.rarity = rarity;
        return true;
    }
    return false;
}

ScryfallDatabase::ScryfallDatabase(QObject *parent)
    : QObject(parent)
{
    connect(
        &m_network,
        &QNetworkAccessManager::finished,
        this,
        &ScryfallDatabase::onRequestFinished
    );

    m_requestTimer.setSingleShot(true);
    m_requestTimer.setInterval(100);
    connect(
        &m_requestTimer,
        &QTimer::timeout,
        this,
        &ScryfallDatabase::maybeSendRequest
    );
}

void ScryfallDatabase::request(int arenaId, const QByteArray &scryfallId)
{
    if (m_cards.contains(arenaId))
        return;

    if (m_runningRequestId == arenaId)
        return;

    // try to load from cache file?
    QFile cache = cardCacheFile(arenaId);
    if (cache.open(QFile::ReadOnly)) {
        const QByteArray json = cache.readAll();
        const QJsonObject root = QJsonDocument::fromJson(json).object();
        ScryfallCardData card;
        if (parseCardJson(root, card)) {
            m_cards[card.arenaId] = card;
            emit cardAvailable(card.arenaId);
            return;
        }
    }

    if (!m_leftToRequest.contains(arenaId)) {
        m_leftToRequest << arenaId;
        if (!m_requestTimer.isActive())
            m_requestTimer.start(100);
    }
}

ScryfallCardData ScryfallDatabase::get(int id) const
{
    return m_cards[id];
}

void ScryfallDatabase::onRequestFinished(QNetworkReply *reply)
{
    const QByteArray json = reply->readAll();
    const QJsonObject root = QJsonDocument::fromJson(json).object();

    ScryfallCardData card;
    if (parseCardJson(root, card)) {
        QFile file = cardCacheFile(card.arenaId);
        if (file.open(QFile::WriteOnly)) {
            file.write(json);
        }
        m_cards[card.arenaId] = card;
        emit cardAvailable(card.arenaId);
        emit dataChanged();
    } else {
        qWarning() << "Failed to parse Scryfall JSON data for" << m_runningRequestId;
        m_leftToRequest << m_runningRequestId;
    }

    m_runningRequestId = -1;
    m_requestTimer.start(100);
}

void ScryfallDatabase::maybeSendRequest()
{
    if (m_runningRequestId > 0 || m_leftToRequest.isEmpty())
        return;

    m_runningRequestId = m_leftToRequest.takeFirst();
    const QByteArray url = "https://api.scryfall.com/cards/arena/" + QByteArray::number(m_runningRequestId);
    m_network.get(QNetworkRequest(QUrl(url)));
}
