#pragma once

#include <QNetworkAccessManager>
#include <QTimer>

#include "carddata.h"

struct ScryfallCardData
{
    QString name;
    QByteArray scryfallId;
    QByteArray set;
    QByteArray typeLine;
    Card::Colors colors = Card::Uncolored;
    Card::Colors colorIdentity = Card::Uncolored;
    Card::Rarity rarity = Card::Unknown;

    bool valid() const { return !scryfallId.isEmpty(); }
    bool isBasicLand() const { return typeLine.startsWith("Basic Land"); }
    bool isLand() const {return typeLine.startsWith("Basic Land") || typeLine.startsWith("Land"); }
    operator bool() const { return valid(); }
};

class ScryfallDatabase : public QObject
{
    Q_OBJECT

public:
    ScryfallDatabase(QObject *parent = nullptr);
    ~ScryfallDatabase() = default;

    void request(int arenaId, const QByteArray &scryfallId);
    ScryfallCardData get(int id) const;

signals:
    void dataChanged();
    void cardAvailable(int id);

private slots:
    void maybeSendRequest();
    void onRequestFinished(QNetworkReply *reply);

private:
    struct Request {
        int arenaId;
        QByteArray scryfallId;
        bool operator==(const Request &other) const { return arenaId == other.arenaId && scryfallId == other.scryfallId; }
    };

    QNetworkAccessManager m_network;
    QTimer m_requestTimer;
    std::optional<Request> m_runningRequest;
    QVector<Request> m_leftToRequest;

    QHash<QByteArray, int> m_scryfallToArenaId;
    QHash<int, ScryfallCardData> m_cards;
};
