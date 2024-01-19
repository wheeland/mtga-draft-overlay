#pragma once

#include <QHash>
#include <QNetworkAccessManager>
#include <QObject>
#include <QVector>

struct CardStatistics
{
    float avgSeen = 0.f;
    float avgPick = 0.f;
    float winRate = 0.f;
};

class CardStatisticsDatabase : public QObject
{
    Q_OBJECT

public:
    CardStatisticsDatabase();
    ~CardStatisticsDatabase() = default;

    void addSet(const QByteArray &setName);
    CardStatistics stats(int card) const { return m_cards[card]; }

signals:
    void dataChanged();

public slots:
    void clearCache();

private slots:
    void onRequestFinished(QNetworkReply *reply);

private:
    void saveSetData(const QByteArray &set, const QByteArray &data) const;
    bool loadSetData(const QByteArray &set);
    void startDownload(const QByteArray &set);
    bool addCardData(const QByteArray &json);

    QScopedPointer<QNetworkAccessManager> m_network;
    QHash<QNetworkReply*, QByteArray> m_currentRequests;
    QHash<int, CardStatistics> m_cards;
    QVector<QByteArray> m_sets;
};
