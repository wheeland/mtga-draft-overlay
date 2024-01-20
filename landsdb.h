#pragma once

#include <QHash>
#include <QNetworkAccessManager>
#include <QObject>

struct SeventeenLandsCardStats
{
    int id = -1;
    float avgSeen = 0.f;
    float avgPick = 0.f;
    float winRate = 0.f;

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
    static void saveSetData(const QByteArray &set, const QByteArray &data);
    bool loadSetData(const QByteArray &set);
    void startDownload(const QByteArray &set);
    bool addCardData(const QByteArray &json);

    QNetworkAccessManager m_network;
    QHash<QNetworkReply*, QByteArray> m_currentRequests;
    QHash<int, SeventeenLandsCardStats> m_cards;
    QVector<QByteArray> m_sets;
};
