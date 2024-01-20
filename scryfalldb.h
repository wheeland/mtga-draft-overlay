#pragma once

#include <QNetworkAccessManager>
#include <QTimer>

#include "carddata.h"

struct ScryfallCardData
{
    QString name;
    QByteArray scryfallId;
    QByteArray set;
    Card::Colors colors = Card::Uncolored;
    Card::Colors colorIdentity = Card::Uncolored;
    Card::Rarity rarity = Card::Unknown;

    bool valid() const { return !scryfallId.isEmpty(); }
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

    QNetworkAccessManager m_network;
    QTimer m_requestTimer;
    int m_runningRequestId = -1;
    QVector<QPair<int, QByteArray>> m_leftToRequest;

    QHash<QByteArray, int> m_scryfallToArenaId;
    QHash<int, ScryfallCardData> m_cards;
};
