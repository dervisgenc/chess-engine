#include "types.h"


class Board
{
public:
    Board(/* args */);
    ~Board();
private:
    uint64_t        m_bitboards[12];
    Color           m_side_to_move;
    int             m_total_moves;
    uint8_t         m_fifty_moves_rule_counter;
    Castling_Rights m_casling_rights;
    Square          m_en_passant_square;


};

