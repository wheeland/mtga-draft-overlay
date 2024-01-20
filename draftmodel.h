#pragma once

#include <QAbstractItemModel>
#include <QPointer>

#include "carddb.h"

class DraftModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    DraftModel(CardDatabase *db);

    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

public slots:
    void onDraftPack(int pack, int pick, QVector<int> cards);
    void onDraftPick(int pack, int pick, int card);

private slots:
    void updateOverlay();

private:
    // struct DraftRound {
    //     QVector<int> cards;
    //     int pick;
    // };

    QVector<int> m_requestedCards;
    QVector<int> m_currCards;

    QPointer<CardDatabase> m_cardDB;
};
