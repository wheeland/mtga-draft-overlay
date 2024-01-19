#pragma once

#include <QHash>
#include <QNetworkAccessManager>
#include <QObject>
#include <QVector>

struct CardStatistics
{
    float avgSeen;
    float avgPick;
    float winRate;
};

class CardStatisticsDatabase : public QObject
{
    Q_OBJECT

public:
    CardStatisticsDatabase();
    ~CardStatisticsDatabase() = default;

    void addSet(const QByteArray &setName);

private slots:
    void onRequestFinished(QNetworkReply *reply);

private:
    QScopedPointer<QNetworkAccessManager> m_network;
    QHash<int, CardStatistics> m_cards;
    QVector<QString> m_sets;
};
