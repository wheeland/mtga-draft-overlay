#pragma once

#include <QHash>
#include <QNetworkAccessManager>
#include <QObject>

#include "carddata.h"

struct SeventeenLandsCardStats
{
    QByteArray scryfallId;
    int id = -1;
    float avgSeen = 0.f;
    float avgPick = 0.f;
    float winRate = 0.f;
    QVector<QPair<Card::Colors, float>> colorWinRate;

    bool valid() const { return id >= 0; }
    operator bool() const { return valid(); }
};

class SeventeenLandsDatabase : public QObject
{
    Q_OBJECT

public:
    SeventeenLandsDatabase();
    ~SeventeenLandsDatabase() = default;

    void addSet(const QByteArray &setName);
    SeventeenLandsCardStats stats(int cardId) const;

signals:
    void dataChanged();

public slots:
    void clearCache();

private slots:
    void onRequestFinished(QNetworkReply *reply);

private:
    static void saveSetData(const QByteArray &set, Card::Colors colors, const QByteArray &data);
    bool loadSetData(const QByteArray &set, Card::Colors colors);
    void startDownload(const QByteArray &set, Card::Colors colors);
    bool addCardData(const QByteArray &json);

    QNetworkAccessManager m_network;
    QHash<QNetworkReply*, QPair<QByteArray, Card::Colors>> m_currentRequests;
    QHash<int, SeventeenLandsCardStats> m_cards;
    QVector<QByteArray> m_sets;
};
