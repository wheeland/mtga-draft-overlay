#include "draftmodel.h"

DraftModel::DraftModel(CardDatabase *db)
    : m_cardDB(db)
{
    connect(db, &CardDatabase::dataChanged, this, [this]() {
        emit dataChanged(createIndex(0, 0), createIndex(20, 0));
    });
}

enum Roles {
    ROLE_INDEX = Qt::UserRole + 1,
    ROLE_ID,
    ROLE_NAME,
    ROLE_COLOR,
    ROLE_AVG_SEEN,
    ROLE_AVG_PICK,
    ROLE_WIN_RATE,
};

QModelIndex DraftModel::index(int row, int column, const QModelIndex &parent) const
{
    return createIndex(row, column);
}

QModelIndex DraftModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}

int DraftModel::rowCount(const QModelIndex &parent) const
{
    return m_currCards.size();
}

int DraftModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant DraftModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < m_currCards.size()) {
        const CardStatistics stats = m_cardDB->stats(m_currCards[index.row()]);

        switch (role) {
        case ROLE_INDEX: return index.row();
        case ROLE_ID: return m_currCards[index.row()];
        case ROLE_NAME: return stats.name;
        case ROLE_COLOR: return stats.colorMix();
        case ROLE_AVG_SEEN: return stats.avgSeen;
        case ROLE_AVG_PICK: return stats.avgPick;
        case ROLE_WIN_RATE: return stats.winRate;
        }
    }
    return QVariant();
}

QHash<int, QByteArray> DraftModel::roleNames() const
{
    QHash<int, QByteArray> result = QAbstractItemModel::roleNames();
    result[ROLE_INDEX] = "index";
    result[ROLE_ID] = "id";
    result[ROLE_NAME] = "name";
    result[ROLE_COLOR] = "color";
    result[ROLE_AVG_SEEN] = "avgSeen";
    result[ROLE_AVG_PICK] = "avgPick";
    result[ROLE_WIN_RATE] = "winRate";
    return result;
}

void DraftModel::onDraftPack(int pack, int pick, QVector<int> cards)
{
    beginResetModel();

    m_currCards = cards;
    std::sort(m_currCards.begin(), m_currCards.end(), [=](int a, int b) {
        const CardStatistics sa = m_cardDB->stats(a);
        const CardStatistics sb = m_cardDB->stats(b);
        if (sa.rarity != sb.rarity)
            return sa.rarity > sb.rarity;
        if (sa.isWhite() != sb.isWhite())
            return sa.isWhite();
        if (sa.isBlue() != sb.isBlue())
            return sa.isBlue();
        if (sa.isBlack() != sb.isBlack())
            return sa.isBlack();
        if (sa.isRed() != sb.isRed())
            return sa.isRed();
        if (sa.isGreen() != sb.isGreen())
            return sa.isGreen();
        return a < b;
    });

    endResetModel();
}

void DraftModel::onDraftPick(int pack, int pick, int card)
{

}
