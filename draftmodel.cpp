#include "draftmodel.h"

#include <array>
#include <QColor>

DraftModel::DraftModel(CardDatabase *db)
    : m_cardDB(db)
{
    connect(db, &CardDatabase::dataAvailable, this, &DraftModel::updateOverlay);
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
        const int id = m_currCards[index.row()];

        ScryfallCardData cardData;
        SeventeenLandsCardStats stats;
        m_cardDB->get(id, cardData, stats);

        switch (role) {
        case ROLE_INDEX: return index.row();
        case ROLE_ID: return id;
        case ROLE_NAME: return cardData.name;
        case ROLE_COLOR: return QColor(255, 255, 255, 255);
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

void DraftModel::updateOverlay()
{
    beginResetModel();

    m_currCards = m_requestedCards;
    std::sort(m_currCards.begin(), m_currCards.end(), [=](int idA, int idB) {
        const ScryfallCardData a = m_cardDB->scryfallDB().get(idA);
        const ScryfallCardData b = m_cardDB->scryfallDB().get(idB);

        // more rare = more first
        if (a.rarity != b.rarity)
            return a.rarity > b.rarity;

        // basic lands always go very last
        if (a.isBasicLand() != b.isBasicLand()) {
            return b.isBasicLand();
        }

        // lands always go last
        if (a.isLand() != b.isLand()) {
            return b.isLand();
        }

        // cards with multiple colors always go later than singular colors
        const int colorCountA = Card::colorCount(a.colors);
        const int colorCountB = Card::colorCount(b.colors);
        if (colorCountA == 2 && colorCountB < 2)
            return false;
        if (colorCountA < 2 && colorCountB == 2)
            return true;

        // next, cards are sorted by color identity
        static std::array<Card::Color, 5> COLORS{Card::White, Card::Blue, Card::Black, Card::Red, Card::Green};
        if (a.colorIdentity != b.colorIdentity) {
            for (Card::Color color : COLORS) {
                if (a.colorIdentity.testFlag(color) != b.colorIdentity.testFlag(color))
                    return a.colorIdentity.testFlag(color);
            }
        }

        // within one color identity, colorless cards go first
        if (a.colors != b.colors) {
            if (a.colors == Card::Uncolored)
                return true;
            if (b.colors == Card::Uncolored)
                return false;
        }

        return idA < idB;
    });

    endResetModel();
}

void DraftModel::onDraftPack(int pack, int pick, QVector<int> cards)
{
    m_requestedCards = cards;
    const bool available = m_cardDB->request(cards);

    if (available) {
        updateOverlay();
    } else {
        beginResetModel();
        m_currCards.clear();
        endResetModel();
    }
}

void DraftModel::onDraftPick(int pack, int pick, int card)
{

}
