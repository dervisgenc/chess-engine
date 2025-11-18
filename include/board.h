#include "types.h"
#include <string>

class Board
{
public:
    Board(/* args */);
    ~Board();

    void parse_fen(const std::string& fen);
private:

    uint64_t        m_bitboards[PIECE_NUMBERS];     //4 bitboards are wasted but simplifies indexing
    Color           m_side_to_move;
    int             m_total_moves;
    uint8_t         m_fifty_moves_rule_counter;
    Castling_Rights m_castling_rights;
    Square          m_en_passant_square;
    


};

