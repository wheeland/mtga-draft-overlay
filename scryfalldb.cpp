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
    const QString name = json["name"].toString().toLocal8Bit();
    const QByteArray scryfallId = json["id"].toString().toLocal8Bit();
    const QByteArray set = json["set"].toString().toLocal8Bit();
    const Card::Colors colors = getColors(json["colors"].toArray());
    const Card::Colors colorIdentity = getColors(json["color_identity"].toArray());
    const Card::Rarity rarity = getRarity(json["rarity"].toString());
    const QByteArray typeLine = json["type_line"].toString().toLocal8Bit();

    if (!name.isEmpty() && !scryfallId.isEmpty() && set.size() == 3) {
        card.name = name;
        card.scryfallId = scryfallId;
        card.set = set;
        card.typeLine = typeLine;
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
    if (!scryfallId.isEmpty())
        m_scryfallToArenaId[scryfallId] = arenaId;

    if (m_cards.contains(arenaId))
        return;

    if (m_runningRequest && m_runningRequest->arenaId == arenaId)
        return;

    // try to load from cache file?
    QFile cache = cardCacheFile(arenaId);
    if (cache.open(QFile::ReadOnly)) {
        const QByteArray json = cache.readAll();
        const QJsonObject root = QJsonDocument::fromJson(json).object();
        ScryfallCardData card;
        if (parseCardJson(root, card)) {
            m_cards[arenaId] = card;
            qInfo() << "[Scryfall] Card loaded from cache:" << arenaId;
            emit cardAvailable(arenaId);
            return;
        }
    }

    const Request req{ arenaId, scryfallId };
    if (!m_leftToRequest.contains(req)) {
        m_leftToRequest << req;
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
        const int arenaId = m_scryfallToArenaId.value(card.scryfallId, root["arena_id"].toInt());
        QFile file = cardCacheFile(arenaId);
        if (file.open(QFile::WriteOnly)) {
            file.write(json);
        }
        qInfo() << "[Scryfall] Card downloaded from web:" << arenaId;
        m_cards[arenaId] = card;

        emit cardAvailable(m_runningRequest->arenaId);
        emit dataChanged();
    } else {
        m_cards[m_runningRequest->arenaId] = card;
        qWarning() << "[Scryfall] Failed to parse JSON data for" << m_runningRequest->arenaId << ":" << reply->url().toString() << ".. retrying ...";

        m_leftToRequest << *m_runningRequest;
    }

    m_runningRequest = std::nullopt;
    m_requestTimer.start(100);
}

void ScryfallDatabase::maybeSendRequest()
{
    if (m_runningRequest || m_leftToRequest.isEmpty())
        return;

    m_runningRequest = m_leftToRequest.takeFirst();

    const QByteArray url = m_runningRequest->scryfallId.isEmpty()
        ? "https://api.scryfall.com/cards/arena/" + QByteArray::number(m_runningRequest->arenaId)
        : "https://api.scryfall.com/cards/" + m_runningRequest->scryfallId;
    m_network.get(QNetworkRequest(QUrl(url)));
}
